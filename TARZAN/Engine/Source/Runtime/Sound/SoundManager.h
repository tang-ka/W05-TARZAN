#pragma once

#pragma comment(lib, "xaudio2.lib")
#include <xaudio2.h>
#include <Windows.h>
#include <set>

class SoundManager
{
private:
    IXAudio2* pxAudio2 = nullptr;
    IXAudio2MasteringVoice* pMasterVoice = nullptr;
    

    struct RIFFHeader {
        char chunkID[4];   // "RIFF"
        DWORD chunkSize;
        char format[4];    // "WAVE"
    };

    struct ChunkHeader {
        char id[4];
        DWORD size;
    };

public:
    HRESULT LoadWavFile(const wchar_t* filename, BYTE** audioData, size_t* dataSize, WAVEFORMATEX** wfx);
    HRESULT InitializeAudio();
    HRESULT PlayBGM(const wchar_t* filename);
    HRESULT PlaySoundEffect(const wchar_t* effectFilename, int volume = -1);
    HRESULT PauseBGM();   
    HRESULT ResumeBGM();  
    HRESULT StopBGM();
    void ShutdownAudio();
};
