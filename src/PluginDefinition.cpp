#include "PluginDefinition.h"
#include <stdio.h>
#include <windows.h>
#include "HighlightFilter.h"
#include "HighlightDlg.h"

// Global variables
extern HINSTANCE appInstance; // Application instance
FuncItem funcItem[nbFunc];    // The plugin data that Notepad++ needs
NppData nppData;              // Data of Notepad++
HWND g_curScintilla = NULL;   // Handle to the current window of Scintilla
FilterManager g_FilterMgr;   // Manages the filters and colors of the highlighters

// Function prototypes
void pluginInit(HANDLE); // Plugin data initialization
void pluginCleanUp(); // Clean up
void GetMainHandler(); // Obtains the current handle of Scintilla
void commandMenuInit(); // Initialization of the plugin commands
void commandMenuCleanUp(); // Clean up plugin commands allocations
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit); // Helper function to initialize the plugin commands
void GetCurrentDocumentFullPath(WCHAR *wsFile); // Returns the full path of the current document
void OnToggleHighlight(); // Command handler that updates the highlight status
void ShowConfigDlg(); // Command handler that shows the dialog to configure the filter strings and colors
void EnableHighlight(bool bEnable); // Enables of disables the highlight of the current document



void pluginInit(HANDLE)
{
	g_FilterMgr.LoadConfigFile();
}

void pluginCleanUp()
{
}

void GetMainHandler()
{
	if (g_curScintilla == NULL)
	{
		int which = -1;
		::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
		if (which == -1)
			return;
		g_curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;
	}
}

void commandMenuInit()
{
	setCommand(0, TEXT("Toggle Highlight"), OnToggleHighlight, NULL, false);
	setCommand(1, TEXT("Highlight Filter..."), ShowConfigDlg, NULL, false);
	GetMainHandler();
}

void commandMenuCleanUp()
{
}

bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit)
{
	if (index >= nbFunc)
		return false;
	if (!pFunc)
		return false;
	lstrcpy(funcItem[index]._itemName, cmdName);
	funcItem[index]._pFunc = pFunc;
	funcItem[index]._init2Check = check0nInit;
	funcItem[index]._pShKey = sk;
	return true;
}

void GetCurrentDocumentFullPath(WCHAR* wsFile)
{
	SendMessage(nppData._nppHandle, NPPM_GETFULLCURRENTPATH, 0, (LPARAM)wsFile);
}

void OnToggleHighlight()
{
	WCHAR wsFile[MAX_PATH];
	GetCurrentDocumentFullPath(wsFile);
	// Toggle the status of the document (highlighted or not)
	bool bEnable = g_FilterMgr.ToggleDocStatus(wsFile);
	EnableHighlight(bEnable);
}

void ShowConfigDlg()
{
	if (IDOK == DialogBox(appInstance, MAKEINTRESOURCE(IDD_LINEHIGHLIGHTER), g_curScintilla, DlgConfigProc))
	{
		if (g_FilterMgr.SaveConfigFile())
			g_FilterMgr.LoadConfigFile();

		WCHAR wsFile[MAX_PATH];
		GetCurrentDocumentFullPath(wsFile);
		g_FilterMgr.SetDocStatus(wsFile, true);
		EnableHighlight(true);
	}
	else
		g_FilterMgr.LoadConfigFile();
}

void EnableHighlight(bool bEnable)
{
	HighlightFilter* pItem = NULL;
	int nCurPos = 0, nStartPos = 0, nEndPos = 0, nCodePage = 0;
	char* sInputLine = NULL;
	int nMaxLineLength = 0;
	int nLineIdx = 0;
	int nMarker = 0;

	// Get code page
	nCodePage = static_cast<int>(SendMessage(g_curScintilla, SCI_GETCODEPAGE, 0, 0));

	// Get position range
	nCurPos = static_cast<int>(SendMessage(g_curScintilla, SCI_GETCURRENTPOS, 0, 0));
	nStartPos = static_cast<int>(SendMessage(g_curScintilla, SCI_WORDSTARTPOSITION, nCurPos, (LPARAM)true));
	nEndPos = static_cast<int>(SendMessage(g_curScintilla, SCI_WORDENDPOSITION, nCurPos, (LPARAM)true));

	// Update markers
	for (int i = 0; i < g_FilterMgr.GetNumFilters(); i++)
	{
		pItem = g_FilterMgr.GetFilter(i);

		if (bEnable)
		{
			// Set Marker Colors
			nMarker = 10 + i;
			pItem->SetMarker(nMarker);
			SendMessage(g_curScintilla, SCI_MARKERDELETEALL, nMarker, 0);   //first, remove all markers
			SendMessage(g_curScintilla, SCI_MARKERDEFINE, nMarker, SC_MARK_BACKGROUND);
			SendMessage(g_curScintilla, SCI_MARKERSETBACK, nMarker, pItem->GetColor());
		}
		else
		{
			nMarker = pItem->GetMarker();
			SendMessage(g_curScintilla, SCI_MARKERDELETEALL, nMarker, 0);   //first, remove all markers
		}
	}

	// Highlight the lines that match any of the filters
	if (bEnable)
	{
		// Process all lines
		while (nLineIdx >= 0)
		{
			// Get next line
			int nLineLength = static_cast<int>(SendMessage(g_curScintilla, SCI_LINELENGTH, nLineIdx, 0));
			if (nLineLength <= 0)
				break;
			if (nLineLength > nMaxLineLength)
			{
				if (sInputLine) delete[] sInputLine;
				nMaxLineLength = nLineLength;
				sInputLine = new char[nMaxLineLength];
			}

			ZeroMemory(sInputLine, sizeof(char)*nMaxLineLength);
			SendMessage(g_curScintilla, SCI_GETLINE, nLineIdx, (LPARAM)sInputLine);

			if (sInputLine == NULL)
			{
				Trace(L"Error: line is NULL");
				break;
			}
			if (strlen(sInputLine) <= 0)
			{
				Trace(L"Error: invalid line length (%d)", strlen(sInputLine));
				break;
			}

			// Find and highlight text
			for (int i = 0; i < g_FilterMgr.GetNumFilters(); i++)
			{
				pItem = g_FilterMgr.GetFilter(i);
				if (!pItem->IsEmpty() && pItem->CheckLine(nCodePage, sInputLine))
					SendMessage(g_curScintilla, SCI_MARKERADD, nLineIdx, pItem->GetMarker()); // line, marker number
			}
			nLineIdx++;
		}
		if (sInputLine) delete[] sInputLine;
	}
	::SendMessage(g_curScintilla, SCI_SETSAVEPOINT, 0, 0);
}
