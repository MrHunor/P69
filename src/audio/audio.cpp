#include <iostream>
#include <vector>
#include <fstream>
#include <string>

// Windows and COM headers
#include <windows.h>
#include <ksmedia.h>
#include <initguid.h> // MUST precede mmdeviceapi.h and audioclient.h in MinGW
#include <mmdeviceapi.h>
#include <audioclient.h>


#pragma pack(push, 1)
struct WavHeader {
    char chunkId[4] = {'R', 'I', 'F', 'F'};
    uint32_t chunkSize = 0; 
    char format[4] = {'W', 'A', 'V', 'E'};
    char subchunk1Id[4] = {'f', 'm', 't', ' '};
    uint32_t subchunk1Size = 16;
    uint16_t audioFormat = 1;
    uint16_t numChannels = 0;
    uint32_t sampleRate = 0;
    uint32_t byteRate = 0;
    uint16_t blockAlign = 0;
    uint16_t bitsPerSample = 0;
    char subchunk2Id[4] = {'d', 'a', 't', 'a'};
    uint32_t subchunk2Size = 0; 
};
#pragma pack(pop)

//ai generated
bool CaptureAudio(double durationSeconds, const std::string& outputFile) {
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) return false;

    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioClient* pAudioClient = nullptr;
    IAudioCaptureClient* pCaptureClient = nullptr;
    WAVEFORMATEX* pwfx = nullptr;

    bool success = false;

    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
    if (SUCCEEDED(hr)) hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (SUCCEEDED(hr)) hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
    if (SUCCEEDED(hr)) hr = pAudioClient->GetMixFormat(&pwfx);

    if (SUCCEEDED(hr)) {
        hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pwfx, NULL);
        if (SUCCEEDED(hr)) hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);
    }

    if (SUCCEEDED(hr)) {
        UINT64 totalFramesRequired = static_cast<UINT64>(durationSeconds * pwfx->nSamplesPerSec);
        UINT64 framesCaptured = 0;
        std::vector<BYTE> audioData;
        
        hr = pAudioClient->Start();
        
        while (framesCaptured < totalFramesRequired) {
            Sleep(10); 

            UINT32 packetLength = 0;
            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            
            while (packetLength != 0) {
                BYTE* pData;
                UINT32 numFramesAvailable;
                DWORD flags;

                hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
                if (FAILED(hr)) break;

                UINT32 framesToCopy = numFramesAvailable;
                if (framesCaptured + framesToCopy > totalFramesRequired) {
                    framesToCopy = static_cast<UINT32>(totalFramesRequired - framesCaptured);
                }

                size_t bytesToCopy = static_cast<size_t>(framesToCopy) * pwfx->nBlockAlign;
                
                if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
                    audioData.insert(audioData.end(), bytesToCopy, 0);
                } else {
                    audioData.insert(audioData.end(), pData, pData + bytesToCopy);
                }

                framesCaptured += framesToCopy;
                hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
                hr = pCaptureClient->GetNextPacketSize(&packetLength);
            }
        }
        
        pAudioClient->Stop();

        WavHeader header;
        if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
            WAVEFORMATEXTENSIBLE* pEx = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx);
            if (pEx->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
                header.audioFormat = 3; 
            }
        } else if (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
            header.audioFormat = 3;
        }

        header.numChannels = pwfx->nChannels;
        header.sampleRate = pwfx->nSamplesPerSec;
        header.bitsPerSample = pwfx->wBitsPerSample;
        header.blockAlign = pwfx->nBlockAlign;
        header.byteRate = header.sampleRate * header.blockAlign;
        
        header.subchunk2Size = static_cast<uint32_t>(audioData.size());
        header.chunkSize = 36 + header.subchunk2Size;

        std::ofstream file(outputFile, std::ios::binary);
        if (file.is_open()) {
            file.write(reinterpret_cast<const char*>(&header), sizeof(WavHeader));
            file.write(reinterpret_cast<const char*>(audioData.data()), audioData.size());
            file.close();
            success = true;
        }
    }

    if (pwfx) CoTaskMemFree(pwfx);
    if (pCaptureClient) pCaptureClient->Release();
    if (pAudioClient) pAudioClient->Release();
    if (pDevice) pDevice->Release();
    if (pEnumerator) pEnumerator->Release();
    CoUninitialize();

    return success;
}