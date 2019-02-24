#pragma once

#include <windows.h>
using namespace std;
#include <vector>

// Displays a message box with an error
void ShowError(LPCTSTR fmt, ...);

// Returns true if the input directory exists
BOOL DirectoryExists(LPCTSTR szPath);

// Used to send traces in debug mode
void Trace(LPCTSTR fmt, ...);

// String conversion functions
char*  ANSItoUTF8(const char *ansi);
char*  UTF16toUTF8(const WCHAR *utf16);
WCHAR* UTF8toUTF16(const char *utf8);