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
CFilterManager g_FilterMgr;   // Manages the filters and colors of the highlighters

// Function prototypes
void pluginInit(HANDLE); // Plugin data initialization
void pluginCleanUp(); // Clean up
void GetMainHandler(); // Obtains the current handle of Scintilla
void commandMenuInit(); // Initialization of the plugin commands
void commandMenuCleanUp(); // Clean up plugin commands allocations
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit); // Helper function to initialize the plugin commands
void GetCurrentDocumentFullPath(wchar_t *file); // Returns the full path of the current document
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
		g_curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
	}	
}

void commandMenuInit()
{   
	setCommand(0, TEXT("Toggle Highlight"),    OnToggleHighlight, NULL, false);
    setCommand(1, TEXT("Highlight Filter..."), ShowConfigDlg,  NULL, false);
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

void GetCurrentDocumentFullPath(wchar_t *file)
{
	SendMessage(nppData._nppHandle, NPPM_GETFULLCURRENTPATH, 0, (LPARAM)file);
}

void OnToggleHighlight()
{	
	wchar_t file[MAX_PATH];
	GetCurrentDocumentFullPath(file);
	// Toggle the status of the document (highlighted or not)
	bool bEnable = g_FilterMgr.UpdateDocStatus(file);
	EnableHighlight(bEnable);
}

void ShowConfigDlg()
{
	if (IDOK == DialogBox(appInstance, MAKEINTRESOURCE(IDD_LINEHIGHLIGHTER), g_curScintilla, DlgConfigProc))
	{
		if (g_FilterMgr.SaveConfigFile())
			g_FilterMgr.LoadConfigFile();

		wchar_t file[MAX_PATH];
		GetCurrentDocumentFullPath(file);		
		EnableHighlight(true);
	}
	else
		g_FilterMgr.LoadConfigFile();
}

void EnableHighlight(bool bEnable)
{		
	CHighlightFilter *pItem = NULL;
	int currpos = 0, startpos = 0, endpos = 0;	
	char *line = NULL;
	int nMaxLineLength = 0;
	int nLineIdx = 0;	
	int nMarker = 0;	

	// Get position range
	currpos = static_cast<int>(SendMessage(g_curScintilla, SCI_GETCURRENTPOS, 0, 0));
	startpos = static_cast<int>(SendMessage(g_curScintilla, SCI_WORDSTARTPOSITION, currpos, (LPARAM)true));
	endpos = static_cast<int>(SendMessage(g_curScintilla, SCI_WORDENDPOSITION, currpos, (LPARAM)true));	
	
	// Update markers
	for (int i = 0; i< g_FilterMgr.GetNumFilters(); i++)
	{
		pItem = g_FilterMgr.GetFilter(i);

		if (bEnable)
		{
			// Set Marker Colors
			nMarker = 10 + i;
			pItem->SetMarker(nMarker);
			SendMessage(g_curScintilla, SCI_MARKERDELETEALL, nMarker, 0);   //first, remove all markers
			SendMessage(g_curScintilla, SCI_MARKERDEFINE,  nMarker, SC_MARK_BACKGROUND);
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
			int lineLength = static_cast<int>(SendMessage(g_curScintilla, SCI_LINELENGTH, nLineIdx, 0));
			if (lineLength <= 0)
				break;
			if (lineLength > nMaxLineLength)
			{
				if (line) delete [] line;
				nMaxLineLength = lineLength;
				line = new char[nMaxLineLength];
			}

			ZeroMemory(line, sizeof(char)*nMaxLineLength);
			SendMessage(g_curScintilla, SCI_GETLINE, nLineIdx, (LPARAM)line);

			if (line == NULL)
			{
				Trace(L"Error: line is NULL");
				break;
			}
			if (strlen(line) <= 0)
			{
				Trace(L"Error: invalid line length (%d)", strlen(line));
				break;
			}

			// Find and highlight text
			for (int i = 0; i< g_FilterMgr.GetNumFilters(); i++)
			{
				pItem = g_FilterMgr.GetFilter(i);
				if (!pItem->IsEmpty() && pItem->CheckLine(line))
					SendMessage(g_curScintilla, SCI_MARKERADD, nLineIdx, pItem->GetMarker()); // line, marker number
			}			
			nLineIdx++;
		}
		if (line) delete [] line;
	}	
	::SendMessage(g_curScintilla, SCI_SETSAVEPOINT, 0, 0);
}
