#pragma once
static const wchar_t g_AppName[] = L"RecorderNotifier";
static const wchar_t g_TitleAppMessage[] = L"Exit";
static const wchar_t g_CloseAppMessage[] = L"Close application";
static const wchar_t g_MessageWhenProcessFound[] = L"Apps:";

#define GWL_HINSTANCE -6

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }