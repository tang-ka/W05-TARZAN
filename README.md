# TARZAN - Deferred Rendering

> Direct3D 11 자체 엔진에 Deferred Rendering 기반 Multi-Pass 구조를 도입한 4인 팀 프로젝트

<!-- TODO: ViewMode 전환(Lit → Normal → Depth → WorldPosition) 대표 GIF 1개 -->

<table>
  <tr><td><b>기간</b></td><td>2025.04.04 ~ 2025.04.10 (1주)</td></tr>
  <tr><td><b>인원</b></td><td>4인</td></tr>
  <tr><td><b>엔진</b></td><td>자체 엔진 (Direct3D 11 / C++ / HLSL)</td></tr>
  <tr><td><b>담당</b></td><td>렌더 파이프라인 멀티 패스 분리 · GBuffer 설계 · Light Pass · ViewMode</td></tr>
</table>

매주 팀을 바꿔 이전 주차 코드베이스를 인수하는 방식으로 진행됐다. W04(PIE) 코드베이스 위에 렌더 파이프라인을 재구성한 W05 결과물이다. 설계 의도와 판단 근거는 포트폴리오에 있고, 여기서는 해당 코드의 위치를 안내한다.

---

## 코드 가이드

### 1. 멀티 패스 렌더링 구조

`FRenderer::RenderPass()`를 GBuffer → Light → PostProcess → Overlay 4개 패스로 분리. 패스 간 연결은 RTV → SRV 전환으로만 이루어지고, 조명 계산은 FullScreen Quad 1회로 고정된다.

| 역할 | 파일 |
|---|---|
| 패스 순서 · 멀티 뷰포트 루프 | [`Renderer.cpp#L56-L100`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Engine/Source/Runtime/Renderer/Renderer.cpp#L56-L100) |
| GBuffer · Light · Overlay 패스 본체 | [`Renderer.cpp#L773-L916`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Engine/Source/Runtime/Renderer/Renderer.cpp#L773-L916) |
| FullScreen Quad 정점 셰이더 | [`FullScreenVertexShader.hlsl`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/FullScreenVertexShader.hlsl) |

### 2. GBuffer 레이아웃 설계

`R16G16B16A16_FLOAT` 4장(Normal · Albedo · Ambient · WorldPos)을 MRT로 묶고, 빈 알파 채널에 Object Mask(Albedo.a)와 VS에서 선형화한 Depth(WorldPos.a)를 실었다.

| 역할 | 파일 |
|---|---|
| GBuffer 텍스처 · RTV · SRV 생성 | [`GraphicDevice.cpp#L161-L215`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Engine/Source/Runtime/Windows/D3D11RHI/GraphicDevice.cpp#L161-L215) |
| 월드 노멀 · Linear Depth 계산 (VS) | [`MeshVertexShader.hlsl#L33-L51`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/MeshVertexShader.hlsl#L33-L51) |
| 4개 RT 출력 · Object Mask (PS) | [`MeshPixelShader.hlsl#L39-L88`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/MeshPixelShader.hlsl#L39-L88) |
| 빌보드 Object Mask 기록 | [`PixelTextureShader.hlsl#L39`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/PixelTextureShader.hlsl#L39) |

### 3. Light Pass와 ViewMode

GBuffer 4장을 SRV로 읽어 Object Mask로 조명 대상을 분기하고, Directional(Ambient + Lambert) + Point Light 배열(최대 300, 거리 감쇠)을 계산한다. ViewMode는 같은 셰이더 안에서 `switch`로 출력만 바꾼다.

| 역할 | 파일 |
|---|---|
| SRV 바인딩 · 광원 배열 상수버퍼 · Quad Draw (CPU) | [`Renderer.cpp#L786-L864`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Engine/Source/Runtime/Renderer/Renderer.cpp#L786-L864) |
| Directional Light | [`LightingPassPixelShader.hlsl#L75-L113`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/LightingPassPixelShader.hlsl#L75-L113) |
| Point Light 루프 · 거리 감쇠 | [`LightingPassPixelShader.hlsl#L115-L154`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/LightingPassPixelShader.hlsl#L115-L154) |
| Object Mask 분기 · ViewMode switch | [`LightingPassPixelShader.hlsl#L157-L221`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/LightingPassPixelShader.hlsl#L157-L221) |

---

## 그 외 담당

- **그리드 라인 거리 페이드** — 카메라 거리에 지수 감쇠 적용 ([`ShaderLine.hlsl`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/ShaderLine.hlsl))
- **기즈모 전용 픽셀 셰이더** — GBuffer를 거치지 않고 Overlay 패스에서 직접 출력 ([`GizmoPixelShader.hlsl`](https://github.com/tang-ka/W05-TARZAN/blob/45ea8d6a1a9a7ea6b113b703462182c070a83d87/TARZAN/Shaders/GizmoPixelShader.hlsl))
- 텍스처·Diffuse 모두 없을 때 마젠타 폴백

## 팀 담당 범위

| 파트 | 담당 |
|---|---|
| 멀티 패스 구조 · GBuffer · Light Pass · ViewMode · 기즈모/라인 셰이더 | 🟢 본인 |
| PostProcess 패스 연결 · Light Pass 출력 버퍼 생성 · Fog ShowFlag | ⚪ 팀원 |
| Height Fog 셰이더 · Spot Light 감쇠 · 멀티 뷰포트 UV 대응 | ⚪ 팀원 |
| 씬 직렬화/저장·로드 · UClass 자동 등록 매크로 | ⚪ 팀원 |

## 실행

Visual Studio 2022, Windows SDK(D3D11) 필요. `TARZAN.sln` 열고 x64 / Debug 빌드 후 실행.
