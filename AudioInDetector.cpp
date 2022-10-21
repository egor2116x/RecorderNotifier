#include "AudioInDetector.h"
#include "MainWindow.h"
#include "PublicDefinition.h"
#include "Utils.h"
#include <Windows.h>
#define COBJMACROS
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <Functiondiscoverykeys_devpkey.h>

void AudioInDetector::isActive(CComPtr<IAudioSessionManager2>& pSessionManager, ActiveDeviceInfo& info)
{
    HRESULT hr = S_OK;
    if (!pSessionManager)
    {
        return;
    }

    int cbSessionCount = 0;

    CComPtr<IAudioSessionEnumerator> pSessionList(nullptr);

    hr = pSessionManager->GetSessionEnumerator(&pSessionList);
    if (hr != S_OK)
    {
        return;
    }

    hr = pSessionList->GetCount(&cbSessionCount);
    if (hr != S_OK)
    {
        return;
    }

    for (int index = 0; index < cbSessionCount; index++)
    {
        CComPtr<IAudioSessionControl> pSessionControl(nullptr);
        CComPtr<IAudioSessionControl2> pSessionControl2(nullptr);

        hr = pSessionList->GetSession(index, &pSessionControl);
        if (hr != S_OK)
        {
            continue;
        }

        hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&pSessionControl2));
        if (hr != S_OK)
        {
            continue;
        }

        hr = pSessionControl2->IsSystemSoundsSession();
        if (S_OK == hr)
        {
            // Exclude system sound session
            continue;
        }

        DWORD processId = 0;
        hr = pSessionControl2->GetProcessId(&processId);
        if (S_OK != hr)
        {
            continue;
        }

        std::wstring processName;
        utils::getProcessNameById(processId, processName);

        AudioSessionState state;
        hr = pSessionControl->GetState(&state);
        if (hr != S_OK)
        {
            continue;
        }
        switch (state)
        {
            case AudioSessionStateInactive:
                break;
            case AudioSessionStateActive:
                info.processName.push_back(processName);
                break;
            case AudioSessionStateExpired:
                break;
        }
    }
}

void AudioInDetector::detect(std::list<ActiveDeviceInfo>& infoList)
{
    HRESULT hr = CoInitialize(0);
    if (hr != S_OK && hr != S_FALSE)
    {
        return;
    }

    CComPtr<IMMDeviceEnumerator> pEnumerator(nullptr);
    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        NULL, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        reinterpret_cast<void**>(&pEnumerator));
    if (hr != S_OK)
    {
        return;
    }

    CComPtr<IMMDeviceCollection> dCol(nullptr);
    hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &dCol);
    if (hr != S_OK)
    {
        return;
    }

    UINT dCount = 0;
    hr = dCol->GetCount(&dCount);
    if (hr != S_OK)
    {
        return;
    }

    for (UINT i = 0; i < dCount; i++)
    {
        ActiveDeviceInfo info;
        CComPtr<IMMDevice> pCaptureDevice(nullptr);
        CComPtr<IAudioSessionManager2> pSessionManager(nullptr);
        hr = dCol->Item(i, &pCaptureDevice);
        if (hr != S_OK)
        {
            continue;
        }

        getDeviceFrendlyName(pCaptureDevice, info);
        hr = pCaptureDevice->Activate(
            __uuidof(IAudioSessionManager2), CLSCTX_ALL,
            NULL, reinterpret_cast<void**>(&pSessionManager));
        if (hr != S_OK)
        {
            continue;
        }

        isActive(pSessionManager, info);
        if (!info.processName.empty())
        {
            infoList.push_back(info);
        }
    }
}

void AudioInDetector::getDeviceFrendlyName(CComPtr<IMMDevice>& pCaptureDevice, ActiveDeviceInfo& info)
{
    CComPtr<IPropertyStore> pProps(nullptr);
    HRESULT hr = pCaptureDevice->OpenPropertyStore(STGM_READ, &pProps);
    if (hr != S_OK)
    {
        return;
    }

    PROPVARIANT varName;
    PropVariantInit(&varName);
    hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
    if (hr != S_OK)
    {
        return;
    }

    info.deviceName = varName.pwszVal;
}


