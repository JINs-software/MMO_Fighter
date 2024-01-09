#include "ExceptionHandler.h"

void ERROR_EXCEPTION_WINDOW(const WCHAR* wlocation, const WCHAR* wcomment) {
	std::wstring wstr = wlocation;
	wstr += L"\n";
	wstr += wcomment;
	MessageBox(NULL, wcomment, wstr.c_str(), MB_OK | MB_ICONWARNING);
}
