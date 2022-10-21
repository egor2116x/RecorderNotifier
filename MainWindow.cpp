#include "MainWindow.h"
#include "PublicDefinition.h"
#include "resource.h"
#include "AudioInDetector.h"
#include "Utils.h"

void MainWindow::run(HINSTANCE hinstance)
{
	if (utils::findProcessByName(g_AppName))
	{
		// The application is already running
		return;
	}
	createWindow(hinstance);
	createIcon(hinstance);
	SetTimer(m_window, 0, 1000, nullptr);
	messageLoopHandling();
	Shell_NotifyIcon(NIM_DELETE, &m_icon);
}

void MainWindow::createWindow(HINSTANCE hinstance)
{
	m_main.cbSize = sizeof(WNDCLASSEX);
	m_main.hInstance = hinstance;
	m_main.lpszClassName = g_AppName;
	m_main.lpfnWndProc = MainWindow::WndProc;
	RegisterClassEx(&m_main);
	m_window = CreateWindowEx(0, g_AppName, nullptr, 0, 0, 0, 0, 0, nullptr, nullptr, hinstance, nullptr);
	m_hinstance = hinstance;
}

void MainWindow::createIcon(HINSTANCE hinstance)
{
	m_icon.cbSize = sizeof(NOTIFYICONDATA);
	m_icon.hWnd = m_window;
	m_icon.uVersion = NOTIFYICON_VERSION;
	m_icon.uCallbackMessage = WM_USER;
	m_icon.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_ICON2));
	m_icon.uFlags = NIF_TIP | NIF_ICON | NIF_MESSAGE;
	wcscpy_s(m_icon.szTip, sizeof(m_icon.szTip) / sizeof(m_icon.szTip[0]), g_AppName);
	Shell_NotifyIcon(NIM_ADD, &m_icon);
}

void MainWindow::messageLoopHandling()
{
	MSG message;
	while (GetMessage(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

LRESULT CALLBACK MainWindow::WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_TIMER:
		{
			std::list<ActiveDeviceInfo> list;
			AudioInDetector::detect(list);
			MainWindow::getInstance().createIcon(MainWindow::getInstance().m_hinstance);
			auto icon = MainWindow::getInstance().m_icon;
			
			if (!list.empty())
			{
				std::wstring appList(g_MessageWhenProcessFound);
				appList += L"\n";
				for (const auto& info : list)
				{
					if (!info.processName.empty())
					{
						for (const auto& appName : info.processName)
						{
							appList += appName + L'\n';
						}
					}
				}
				
				wcscpy_s(icon.szTip, sizeof(icon.szTip) / sizeof(icon.szTip[0]), appList.c_str());
				Shell_NotifyIcon(NIM_MODIFY, &icon);
			}
			else {
				Shell_NotifyIcon(NIM_DELETE, &icon);
			}
			break;
		}

		case WM_USER:
			if (lParam == WM_RBUTTONDOWN)
			{
				if (MessageBox(window, g_CloseAppMessage, g_TitleAppMessage, MB_YESNO) == IDYES)
				{
					DestroyWindow(window);
				}
			}

			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(window, message, wParam, lParam);
	}
	return 0;
}
