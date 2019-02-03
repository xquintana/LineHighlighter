#include "Utils.h"


static wstring wsAux;

void ShowError(LPCTSTR fmt, ...)
{
	va_list args;
	wchar_t buffer[2048];
	va_start(args, fmt);
	vswprintf_s(buffer, 2048, fmt, args);
	va_end(args);
	::MessageBox(NULL, buffer, L"Line Highlight Plugin Error", MB_OK | MB_ICONERROR);
}

void Trace(LPCTSTR fmt, ...)
{
#ifdef _DEBUG
	va_list args;
	wchar_t buffer[2048];
	va_start(args, fmt);
	vswprintf_s(buffer, 2048, fmt, args);
	va_end(args);
	OutputDebugString(buffer);
#else
	UNREFERENCED_PARAMETER(fmt);
#endif
}

const wchar_t *StringToWString(const string &sInput)
{
	std::wstring wsTmp(sInput.begin(), sInput.end());
	wsAux = wsTmp;
	return wsAux.c_str();
}

string WStringToString(const wchar_t *sInput)
{
	wstring ws(sInput);
	string str(ws.begin(), ws.end());
	return str;
}

BOOL DirectoryExists(const LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
