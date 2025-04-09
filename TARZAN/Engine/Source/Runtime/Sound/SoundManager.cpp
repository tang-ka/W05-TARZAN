#include "SoundManager.h"
#include <cstring>

IXAudio2SourceVoice* pSourceVoice = nullptr;

HRESULT SoundManager::LoadWavFile(const wchar_t* filename, BYTE** audioData, size_t* dataSize, WAVEFORMATEX** wfx)
{
    if (!filename || !audioData || !dataSize || !wfx)
        return E_INVALIDARG;

    HANDLE hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, 0, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD fileSize = GetFileSize(hFile, nullptr);
    if (fileSize == INVALID_FILE_SIZE)
    {
        CloseHandle(hFile);
        return HRESULT_FROM_WIN32(GetLastError());
    }

    BYTE* fileBuffer = new BYTE[fileSize];
    DWORD bytesRead = 0;
    if (!ReadFile(hFile, fileBuffer, fileSize, &bytesRead, nullptr) || bytesRead != fileSize)
    {
        CloseHandle(hFile);
        delete[] fileBuffer;
        return HRESULT_FROM_WIN32(GetLastError());
    }
    CloseHandle(hFile);

    if (fileSize < sizeof(RIFFHeader))
    {
        delete[] fileBuffer;
        return E_FAIL;
    }
    RIFFHeader* riff = (RIFFHeader*)fileBuffer;
    if (std::strncmp(riff->chunkID, "RIFF", 4) != 0 ||
        std::strncmp(riff->format, "WAVE", 4) != 0)
    {
        delete[] fileBuffer;
        return E_FAIL;
    }

    BYTE* ptr = fileBuffer + sizeof(RIFFHeader);
    BYTE* endPtr = fileBuffer + fileSize;
    WAVEFORMATEX* localWfx = nullptr;
    BYTE* localAudioData = nullptr;
    DWORD localDataSize = 0;

    while (ptr + sizeof(ChunkHeader) <= endPtr)
    {
        ChunkHeader* ch = (ChunkHeader*)ptr;
        ptr += sizeof(ChunkHeader);

        if (std::strncmp(ch->id, "fmt ", 4) == 0)
        {
            if (ch->size < sizeof(WAVEFORMATEX) - sizeof(WORD))
            {
                delete[] fileBuffer;
                return E_FAIL;
            }
            localWfx = (WAVEFORMATEX*) new BYTE[ch->size + sizeof(WORD)];
            memcpy(localWfx, ptr, ch->size);
        }
        else if (std::strncmp(ch->id, "data", 4) == 0)
        {
            localDataSize = ch->size;
            localAudioData = new BYTE[localDataSize];
            memcpy(localAudioData, ptr, localDataSize);
        }

        ptr += ch->size;
        if (ch->size & 1)
            ptr++;
    }

    delete[] fileBuffer;

    if (!localWfx || !localAudioData)
    {
        if (localWfx) delete[]((BYTE*)localWfx);
        if (localAudioData) delete[] localAudioData;
        return E_FAIL;
    }

    *wfx = localWfx;
    *audioData = localAudioData;
    *dataSize = localDataSize;
    return S_OK;
}

HRESULT SoundManager::InitializeAudio()
{
    HRESULT hr = XAudio2Create(&pxAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) return hr;
    hr = pxAudio2->CreateMasteringVoice(&pMasterVoice);
    return hr;
}

HRESULT SoundManager::PlayBGM(const wchar_t* filename)
{
    BYTE* audioData = nullptr;
    size_t dataSize = 0;
    WAVEFORMATEX* wfx = nullptr;
    HRESULT hr = LoadWavFile(filename, &audioData, &dataSize, &wfx);
    if (FAILED(hr)) return hr;

    hr = pxAudio2->CreateSourceVoice(&pSourceVoice, wfx);
    if (FAILED(hr)) return hr;

    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = (UINT32)dataSize;
    buffer.pAudioData = audioData;
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

    hr = pSourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) return hr;

    pSourceVoice->SetVolume(.5f);
    hr = pSourceVoice->Start(0);
    return hr;
}

HRESULT SoundManager::PlaySoundEffect(const wchar_t* effectFilename, int volume)
{
    BYTE* audioData = nullptr;
    size_t dataSize = 0;
    WAVEFORMATEX* wfx = nullptr;
    HRESULT hr = LoadWavFile(effectFilename, &audioData, &dataSize, &wfx);
    if (FAILED(hr))
        return hr;

    IXAudio2SourceVoice* pEffectVoice = nullptr;
    hr = pxAudio2->CreateSourceVoice(&pEffectVoice, wfx);
    if (FAILED(hr))
        return hr;

    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = (UINT32)dataSize;
    buffer.pAudioData = audioData;
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.LoopCount = 0;

    hr = pEffectVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr))
        return hr;

    int volumeValue = volume > 0 ? volume : 8;
    pEffectVoice->SetVolume(volumeValue);
    hr = pEffectVoice->Start(0);
    // 메모리 해제나 DestroyVoice는 콜백 또는 타이머로 따로 관리 필요
    return hr;
}

HRESULT SoundManager::PauseBGM()
{
    if (pSourceVoice)
    {
        // source voice를 정지(일시정지 효과, 이후 Start로 재개 가능)
        return pSourceVoice->Stop(0);
    }
    return E_FAIL;
}

HRESULT SoundManager::ResumeBGM()
{
    if (pSourceVoice)
    {
        // source voice를 정지(일시정지 효과, 이후 Start로 재개 가능)
        return pSourceVoice->Stop(0);
    }
    return E_FAIL;
}

HRESULT SoundManager::StopBGM()
{
    HRESULT hr = E_FAIL;
    if (pSourceVoice)
    {
        // 재생 멈추고 버퍼 플러시 후 source voice 파괴
        hr = pSourceVoice->Stop(0);
        pSourceVoice->FlushSourceBuffers();
        pSourceVoice->DestroyVoice();
        pSourceVoice = nullptr;
    }
    return hr;
}

void SoundManager::ShutdownAudio()
{
    if (pSourceVoice)
    {
        pSourceVoice->Stop(0);
        pSourceVoice->DestroyVoice();
        pSourceVoice = nullptr;
    }
    if (pMasterVoice)
    {
        pMasterVoice->DestroyVoice();
        pMasterVoice = nullptr;
    }
    if (pxAudio2)
    {
        pxAudio2->Release();
        pxAudio2 = nullptr;
    }
}
