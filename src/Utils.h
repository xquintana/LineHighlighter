#pragma once

#include <windows.h>
using namespace std;
#include <vector>

// Displays a message box with an error
void ShowError(LPCTSTR fmt, ...);

// Converts an ANSI string into a UNICODE
const wchar_t *StringToWString(const string &s);

// Converts a UNICODE string into ANSI
string WStringToString(const wchar_t *sInput);

// Returns true if the input directory exists
BOOL DirectoryExists(LPCTSTR szPath);

// Used to send traces in debug mode
void Trace(LPCTSTR fmt, ...);