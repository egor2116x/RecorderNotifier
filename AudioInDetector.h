#pragma once
#include <audiopolicy.h>
#include <mmdeviceapi.h>
#include <list>
#include <string>
#include <atlbase.h>

struct ActiveDeviceInfo
{
    std::list<std::wstring> processName;
    std::wstring deviceName;
};

class AudioInDetector
{
public:
    static AudioInDetector& getInstance()
    {
        static AudioInDetector instance;
        return instance;
    }
    AudioInDetector(AudioInDetector const&) = delete;
    void operator=(AudioInDetector const&) = delete;
    static void detect(std::list<ActiveDeviceInfo>& infoList);

private:
    AudioInDetector(){}
    static void isActive(CComPtr<IAudioSessionManager2>& pSessionManager, ActiveDeviceInfo& info);
    static void getDeviceFrendlyName(CComPtr<IMMDevice>& pCaptureDevice, ActiveDeviceInfo& info);
};

