#include "Utils.h"
#include <Windows.h>
#include <psapi.h>

const size_t g_ProcNameSizeMax = 64;

bool utils::findProcessByName(std::wstring procName)
{
    DWORD aProcesses[1024], cbNeeded, numProcesses;
    int foundProcesses = 0;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return 1;
    }

    numProcesses = cbNeeded / sizeof(DWORD);

    for (int i = 0; i < numProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                                            PROCESS_VM_READ,
                                            FALSE, aProcesses[i]);

            if (nullptr != hProcess)
            {
                HMODULE hMod;
                DWORD cbNeeded;
                wchar_t receivedProcName[g_ProcNameSizeMax] = {0};

                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
                    &cbNeeded))
                {
                    const DWORD result = GetModuleBaseName(hProcess, hMod, receivedProcName, g_ProcNameSizeMax);
                    if (result)
                    {
                        if (std::wstring(receivedProcName).find(procName) != std::wstring::npos)
                        {
                            ++foundProcesses;
                        }
                    }

                }
                CloseHandle(hProcess);
            }
        }
    }

    return foundProcesses > 1;
}

bool utils::getProcessNameById(unsigned long processId, std::wstring& processName)
{
    HANDLE Handle = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE,
        processId
    );

    bool result = false;
    if (Handle)
    {
        wchar_t buffer[MAX_PATH];
        if (GetModuleFileNameEx(Handle, 0, buffer, MAX_PATH))
        {
            processName = buffer;
            const size_t pos = processName.rfind('\\', processName.length());
            if (pos != std::wstring::npos)
            {
                processName = processName.substr(pos + 1, processName.length() - pos);
            }
            result = true;
        }
        CloseHandle(Handle);
    }
    return result;
}
