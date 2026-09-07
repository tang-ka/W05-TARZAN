# TARZAN - Deferred Rendering

> Direct3D 11 자체 엔진에 Deferred Rendering 기반 Multi-Pass 구조를 도입한 4인 팀 프로젝트

<!-- TODO: ViewMode 전환(Lit → Normal → Depth → WorldPosition) 대표 GIF 1개 -->

<table>
  <tr><td><b>기간</b></td><td>2025.04.04 ~ 2025.04.10 (1주)</td></tr>
  <tr><td><b>인원</b></td><td>4인</td></tr>
  <tr><td><b>엔진</b></td><td>자체 엔진 (Direct3D 11 / C++ / HLSL)</td></tr>
  <tr><td><b>담당</b></td><td>렌더 파이프라인 멀티 패스 분리 · GBuffer 설계 · Light Pass · ViewMode</td></tr>
</table>

---

## 목차

- [코드 가이드](#코드-가이드)
  - [1. 멀티 패스 렌더링 구조](#1-멀티-패스-렌더링-구조)
  - [2. GBuffer 레이아웃 설계](#2-gbuffer-레이아웃-설계)
  - [3. Light Pass와 ViewMode](#3-light-pass와-viewmode)
- [그 외 담당](#그-외-담당)
- [팀 담당 범위](#팀-담당-범위)
- [실행](#실행)

---

## 코드 가이드

### 1. 멀티 패스 렌더링 구조

조명 수가 늘어날수록 메시 픽셀 셰이더 비용이 함께 증가하는 Forward 구조를 개선하기 위해,
흩어져 있던 렌더 로직을 `FRenderer`로 모으고 `RenderPass()`를 4개의 패스로 분리했다.

```cpp
void FRenderer::RenderPass()
{
    ...
    RenderGBuffer();          // 메시 · 빌보드 → GBuffer 4장
    RenderLightPass();        // GBuffer SRV → 조명 계산 (FullScreen Quad 1회)
    RenderPostProcessPass();  // Light Pass 결과 → Height Fog 합성
    RenderOverlayPass();      // 라인 배치 · 기즈모 (조명 미적용)
}
```

| 패스 | Draw | 입력 (SRV) | 출력 (RTV) |
|---|---|---|---|
| GBuffer | StaticMesh · Billboard | Diffuse Texture | Normal · Albedo · Ambient · WorldPos |
| Light | FullScreen Quad 1회 | GBuffer 4장 | LightPass Color · Position |
| PostProcess | FullScreen Quad 1회 | LightPass Color · Position | FrameBuffer |
| Overlay | Line Batch · Gizmo | Line · Gizmo 정점 버퍼 | FrameBuffer |

- **패스 간 연결은 RTV → SRV 전환으로만 이루어진다.** 각 패스는 이전 패스가 남긴 텍스처만 읽고, 씬 오브젝트 목록을 다시 순회하지 않는다
- 조명 계산이 화면 크기 1회로 고정되므로 오브젝트 수 × 조명 수가 아니라 픽셀 수 × 조명 수로 비용이 정리된다
- Overlay 패스는 Depth Test를 켜고 FrameBuffer에 직접 그려 라인·기즈모가 씬 깊이에 가려지되 조명 영향은 받지 않도록 했다
- 패스마다 SRV를 명시적으로 언바인딩해 다음 패스에서 같은 텍스처를 RTV로 다시 잡을 때 충돌이 나지 않도록 했다
- 멀티 뷰포트 모드에서는 뷰포트 4개에 대해 `PrepareRender()` → `RenderPass()`를 반복 실행한다

| 역할 | 파일 |
|---|---|
| 패스 순서 · 멀티 뷰포트 루프 | [`Renderer.cpp#L56-L100`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Engine/Source/Runtime/Renderer/Renderer.cpp#L56-L100) |
| GBuffer · Light · Overlay 패스 본체 | [`Renderer.cpp#L773-L916`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Engine/Source/Runtime/Renderer/Renderer.cpp#L773-L916) |
| FullScreen Quad 정점 셰이더 | [`FullScreenVertexShader.hlsl`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/FullScreenVertexShader.hlsl) |

---

### 2. GBuffer 레이아웃 설계

RenderTarget을 최소한으로 쓰면서 조명 계산에 필요한 정보를 전부 넘기는 것이 목표였다.
`R16G16B16A16_FLOAT` 4장을 MRT로 묶고, 비어 있는 알파 채널에 부가 정보를 실었다.

| RT | RGB | Alpha | 설계 판단 |
|---|---|---|---|
| RT0 Normal | 월드 노멀 (`normal * 0.5 + 0.5`) | — | 부호 있는 노멀을 [0,1]로 인코딩해 이후 UNORM 계열 포맷으로 교체할 여지를 남겼다 |
| RT1 Albedo | 텍스처 × Diffuse 합성 색상 | **Object Mask** | 배경 0.0 / 스태틱 메시 0.5 / 빌보드 0.7을 기록. Light Pass가 이 값으로 조명 대상 여부를 분기한다 |
| RT2 Ambient | 머티리얼 Ambient 색상 | — | |
| RT3 WorldPos | 월드 좌표 | **Linear Depth** | VS에서 미리 선형화한 정규화 Depth를 저장. Depth 시각화와 Fog 합성에 사용한다 |

**Linear Depth를 VS에서 계산한 이유** — 클립 공간 `z/w`는 근거리에 정밀도가 몰려 있어 시각화하면 대부분 흰색으로 뭉친다.
`nearZ · farZ / (farZ - z · (farZ - nearZ))`로 뷰 공간 거리를 복원한 뒤 `[near, far]` 구간으로 정규화해 보간되도록 VS에서 처리했다.
PS는 이 값을 WorldPos의 w 채널에 그대로 쓴다.

**Object Mask** — 배경 픽셀까지 조명을 태우지 않기 위해 Albedo 알파를 마스크로 썼다.
스태틱 메시 PS는 0.5, 빌보드 PS는 0.7을 기록하고, 클리어 값 0.0이 배경이 된다. 별도 스텐실 없이 RT 하나로 분기가 끝난다.

**결과** — Light Pass는 GBuffer SRV 4개만 읽으면 되고, 머티리얼 상수버퍼나 씬 오브젝트 목록을 다시 참조하지 않는다.

| 역할 | 파일 |
|---|---|
| GBuffer 텍스처 · RTV · SRV 생성 | [`GraphicDevice.cpp#L161-L215`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Engine/Source/Runtime/Windows/D3D11RHI/GraphicDevice.cpp#L161-L215) |
| 월드 노멀 · Linear Depth 계산 (VS) | [`MeshVertexShader.hlsl#L33-L51`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/MeshVertexShader.hlsl#L33-L51) |
| 4개 RT 출력 · Object Mask (PS) | [`MeshPixelShader.hlsl#L39-L88`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/MeshPixelShader.hlsl#L39-L88) |
| 빌보드 Object Mask 기록 | [`PixelTextureShader.hlsl#L39`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/PixelTextureShader.hlsl#L39) |

---

### 3. Light Pass와 ViewMode

GBuffer 4장을 SRV로 바인딩하고 FullScreen Quad를 한 번 그려 모든 픽셀의 조명을 계산한다.
출력은 Color와 Position 2장으로, 다음 PostProcess 패스가 Fog 합성에 쓴다.

**픽셀당 흐름**

1. Normal을 `(rgb - 0.5) * 2`로 디코딩, Albedo 알파에서 Object Mask, WorldPos 알파에서 Linear Depth를 읽는다
2. 마스크가 스태틱 메시(0.5)면 ViewMode에 따라 분기, 빌보드(0.7)나 배경이면 Albedo를 그대로 출력한다
3. Lit 모드는 Directional Light(Ambient + Lambert Diffuse)와 Point Light 루프 결과를 더한다

**Point Light** — 상수버퍼에 최대 300개 광원을 배열로 올리고 픽셀 셰이더에서 순회한다.
거리 감쇠는 `1 / (1 + falloff · (d / r)²)`로, 반경과 감쇠 계수를 분리해 UI에서 따로 조정할 수 있게 했다.
CPU 쪽은 매 프레임 광원 컴포넌트 목록을 수집해 배열 상수버퍼 하나로 업로드한다.

**ViewMode** — 같은 셰이더 안에서 `switch`로 처리한다. GBuffer에 이미 필요한 값이 다 들어 있어 별도 패스나 셰이더 교체 없이 출력만 바꾸면 된다.

| 모드 | 출력 |
|---|---|
| Lit | Directional + Point Light 조명 결과 |
| Unlit / Wireframe / BaseColor | Albedo |
| Normal | 디코딩한 월드 노멀 |
| Depth | Linear Depth (WorldPos.w) |
| WorldPosition | 정규화한 월드 좌표 |

| 역할 | 파일 |
|---|---|
| SRV 바인딩 · 광원 배열 상수버퍼 · Quad Draw (CPU) | [`Renderer.cpp#L786-L864`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Engine/Source/Runtime/Renderer/Renderer.cpp#L786-L864) |
| Directional Light 계산 | [`LightingPassPixelShader.hlsl#L75-L113`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/LightingPassPixelShader.hlsl#L75-L113) |
| Point Light 루프 · 거리 감쇠 | [`LightingPassPixelShader.hlsl#L115-L154`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/LightingPassPixelShader.hlsl#L115-L154) |
| Object Mask 분기 · ViewMode switch | [`LightingPassPixelShader.hlsl#L157-L221`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/LightingPassPixelShader.hlsl#L157-L221) |

---

## 그 외 담당

- **그리드 라인 거리 페이드** — 카메라와의 XY 평면 거리에 `1 - exp(-density · d)` 지수 감쇠를 적용해 원경 그리드가 자연스럽게 사라지도록 처리 ([`ShaderLine.hlsl`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/ShaderLine.hlsl))
- **기즈모 전용 픽셀 셰이더** — 기즈모가 GBuffer를 거치지 않고 Overlay 패스에서 머티리얼 색으로 바로 출력되도록 분리. 기즈모 위에 그리드 라인이 겹쳐 그려지던 문제와 색상 버그를 같이 정리 ([`GizmoPixelShader.hlsl`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/GizmoPixelShader.hlsl))
- **텍스처 누락 폴백** — 텍스처와 Diffuse 색이 모두 없을 때 마젠타를 출력해 에셋 누락을 화면에서 바로 확인할 수 있게 함

## 팀 담당 범위

| 파트 | 담당 |
|---|---|
| 멀티 패스 구조 · GBuffer · Light Pass · ViewMode · 기즈모/라인 셰이더 | 🟢 본인 |
| PostProcess 패스 연결 · Light Pass 출력 버퍼 생성 · Fog ShowFlag | ⚪ 팀원 |
| Height Fog 셰이더 · Spot Light 감쇠 · 멀티 뷰포트 UV 대응 | ⚪ 팀원 |
| 씬 직렬화/저장·로드 · UClass 자동 등록 매크로 | ⚪ 팀원 |

매주 팀이 바뀌며 이전 주차 코드베이스를 인수하는 방식으로 진행됐다. 이 레포는 W04(PIE) 코드베이스 위에 렌더 파이프라인을 재구성한 W05 결과물이다.
본인 커밋만 보려면 [`?author=gur0907`](https://github.com/tang-ka/W05-TARZAN/commits/main?author=gur0907).

## 실행

Visual Studio 2022, Windows SDK(D3D11) 필요. `TARZAN.sln` 열고 x64 / Debug 빌드 후 실행.
