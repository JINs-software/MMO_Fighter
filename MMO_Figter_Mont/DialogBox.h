#pragma once
#include <windows.h>

// 255.255.255.255
class MyDialogClass
{
public:
	static bool progressFlag;
	static bool loopBackMode;
	static TCHAR ipAddress[16];
	static TCHAR port[6];
	static unsigned short usPort;

	static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

