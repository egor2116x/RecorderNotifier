#pragma once
#include <windows.h>

class MainWindow
{
public:
    static MainWindow& getInstance()
    {
        static MainWindow instance;   
        return instance;
    }
    MainWindow(MainWindow const&) = delete;
    void operator=(MainWindow const&) = delete;
    void run(HINSTANCE hinstance);
    NOTIFYICONDATA getIcon() const { return m_icon; }
    
private:
    MainWindow() {}
    void createWindow(HINSTANCE hinstance);
    void createIcon(HINSTANCE hinstance);
    void messageLoopHandling();
    static LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

private:
	NOTIFYICONDATA m_icon = { 0 };
    WNDCLASSEX m_main = { 0 };
    HWND m_window;
    HINSTANCE m_hinstance;
};

