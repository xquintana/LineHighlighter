#include "Utils.h"

#define STR_MAX_LEN 65536

static WCHAR wsAux[STR_MAX_LEN];
static char   sAux[STR_MAX_LEN];

void ShowError(LPCTSTR fmt, ...)
{
	va_list args;
	WCHAR wsBuffer[2048];
	va_start(args, fmt);
	vswprintf_s(wsBuffer, 2048, fmt, args);
	va_end(args);
	::MessageBox(NULL, wsBuffer, L"Line Highlight Plugin Error", MB_OK | MB_ICONERROR);
}

void Trace(LPCTSTR fmt, ...)
{
#ifdef _DEBUG
	va_list args;
	WCHAR wsBuffer[2048];
	va_start(args, fmt);
	vswprintf_s(wsBuffer, 2048, fmt, args);
	va_end(args);
	OutputDebugString(wsBuffer);
#else
	UNREFERENCED_PARAMETER(fmt);
#endif
}

BOOL DirectoryExists(const LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

char* ANSItoUTF8(const char *ansi)
{
	if (!MultiByteToWideChar(CP_ACP, 0, ansi, -1, wsAux, sizeof(wsAux) / sizeof(WCHAR))) // ANSI to UTF-16
		return NULL;
	if (!WideCharToMultiByte(CP_UTF8, 0, wsAux, -1, sAux, sizeof(sAux), 0, 0))	         // UTF-16 to UTF-8
		return NULL;
	return sAux;
}

char* UTF16toUTF8(const WCHAR *utf16)
{
	if (!WideCharToMultiByte(CP_UTF8, 0, utf16, -1, sAux, sizeof(sAux), 0, 0))
		return NULL;
	return sAux;
}

WCHAR* UTF8toUTF16(const char *utf8)
{
	if (!MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wsAux, sizeof(wsAux) / sizeof(WCHAR)))
		return NULL;
	return wsAux;
}
