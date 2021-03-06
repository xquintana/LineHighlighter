#include "HighlightFilter.h"
#include "HighlightDlg.h"
#include "Commctrl.h"

#define MAX_STRING_LEN	1024

// Global variables
extern FilterManager g_FilterMgr; // Highlight filter manager
extern HWND g_curScintilla; // Handle to the current window of Scintilla
HighlightFilter* g_pSelectedFilter = NULL; // Pointer to the current filter
int g_nLastSelection = 0; // Last selected index in the filter combo

// Function prototypes
BOOL CenterWindow(HWND hwndWindow);
BOOL InitControls(HWND hWndDlg);
void UpdateControls(HWND hWndDlg, HWND hWndCombo);
BOOL OpenColorPicker(HWND hParent, HighlightFilter* pFilter, COLORREF& rgbResult);

// Callbacks
LRESULT CALLBACK ComboProc(HWND hWndCombo, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubClass, DWORD_PTR);
UINT_PTR CALLBACK CCHookProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgConfigProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam);


// Centers the dialog relative to its parent
BOOL CenterWindow(HWND hwndWindow)
{
	HWND hwndParent;
	RECT rectWindow, rectParent;

	if ((hwndParent = GetParent(hwndWindow)) != NULL)
	{
		GetWindowRect(hwndWindow, &rectWindow);
		GetWindowRect(hwndParent, &rectParent);

		int nWidth = rectWindow.right - rectWindow.left;
		int nHeight = rectWindow.bottom - rectWindow.top;

		int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
		int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;
		MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);
		return TRUE;
	}

	return FALSE;
}

// Initializes the combo and the editbox
// Each entry of the combo is assigned a pointer to a highlight filter
BOOL InitControls(HWND hWndDlg)
{
	WCHAR wsIndex[MAX_PATH];
	HWND hWndCombo = GetDlgItem(hWndDlg, IDC_COMBO_HIGHLIGHT_ID);
	if (hWndCombo == NULL)
		return FALSE;

	for (int i = 0; i < g_FilterMgr.GetNumFilters(); i++)
	{
		HighlightFilter* pFilter = g_FilterMgr.GetFilter(i);
		if (i == 0) g_pSelectedFilter = pFilter;
		swprintf(wsIndex, MAX_PATH, L"%d", i + 1);
		SendMessage(hWndCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(wsIndex));
		SendMessage(hWndCombo, CB_SETITEMDATA, i, reinterpret_cast<LPARAM>(pFilter));

	}
	SendMessage(hWndCombo, CB_SETCURSEL, (WPARAM)g_nLastSelection, (LPARAM)0);
	UpdateControls(hWndDlg, hWndCombo);
	::SetFocus(hWndCombo);
	return TRUE;
}

// Gets the selected item of the combo and updates the editbox with the item's name
void UpdateControls(HWND hWndDlg, HWND hWndCombo)
{
	LRESULT lResult = 0;
	HWND hEdit = GetDlgItem(hWndDlg, IDC_EDIT_TEXT);

	// Get the selected index of the combo
	int index = (int)SendMessage(hWndCombo, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	if (index >= 0)
	{
		// Get the associated item's pointer
		lResult = SendMessage(hWndCombo, CB_GETITEMDATA, index, 0);

		if (lResult == CB_ERR)
			ShowError(L"Cannot get item from ComboBox");
		else
		{
			g_pSelectedFilter = reinterpret_cast<HighlightFilter*>(lResult);
			::InvalidateRect(hEdit, NULL, FALSE);
			// Update the text of the editbox with the item's text
			SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)UTF8toUTF16(g_pSelectedFilter->GetText()));
			g_nLastSelection = index;
		}
	}

}

LRESULT CALLBACK ComboProc(HWND hWndCombo, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubClass, DWORD_PTR)
{
	if (msg == WM_CTLCOLORLISTBOX)
	{
		COMBOBOXINFO ci = { sizeof(COMBOBOXINFO) };
		GetComboBoxInfo(hWndCombo, &ci);
		if (HWND(lParam) == ci.hwndList) // handle to the drop-down list.
			UpdateControls(GetParent(hWndCombo), hWndCombo);
	}

	if (msg == WM_NCDESTROY)
		RemoveWindowSubclass(hWndCombo, ComboProc, uIdSubClass);

	return DefSubclassProc(hWndCombo, msg, wParam, lParam);
}

// Callback needed for the color picker
UINT_PTR CALLBACK CCHookProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	wParam = lParam = 0;
	if (message == WM_INITDIALOG)
	{
		CenterWindow(hdlg);
		return TRUE;
	}
	return FALSE;
}

// Opens a dialog that allows to select a background color
BOOL OpenColorPicker(HWND hParent, HighlightFilter* pFilter, COLORREF& rgbResult)
{
	COLORREF col_array[16];
	for (int i = 0; i < 16; i++)
		col_array[i] = RGB(255, 255, 255);

	CHOOSECOLOR ccColour;
	ZeroMemory(&ccColour, sizeof(CHOOSECOLOR));
	ccColour.lStructSize = sizeof(CHOOSECOLOR);
	ccColour.hwndOwner = hParent;
	ccColour.hInstance = NULL;
	ccColour.Flags = CC_FULLOPEN | CC_ENABLEHOOK;
	ccColour.lpCustColors = col_array;
	ccColour.lpfnHook = CCHookProc;

	if (pFilter)
	{
		ccColour.Flags |= CC_RGBINIT;
		ccColour.rgbResult = pFilter->GetColor();
	}

	if (ChooseColor(&ccColour) == TRUE)
	{
		rgbResult = ccColour.rgbResult;
		Trace(L"OpenColorPicker-> r=%d-g=%d-b=%d", GetRValue(rgbResult), GetGValue(rgbResult), GetBValue(rgbResult));
		return TRUE;
	}
	return FALSE;
}

// Applies the content of the edit box to the current filter
void UpdateFilterText(HWND hWndDlg)
{
	if (g_pSelectedFilter)
	{
		WCHAR wsEditText[MAX_STRING_LEN];
		HWND hEdit = GetDlgItem(hWndDlg, IDC_EDIT_TEXT);
		GetWindowText(hEdit, wsEditText, MAX_STRING_LEN);
		g_pSelectedFilter->SetText(UTF16toUTF8(wsEditText));
	}
}

INT_PTR CALLBACK DlgConfigProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowSubclass(GetDlgItem(hWndDlg, IDC_COMBO_HIGHLIGHT_ID), ComboProc, 0, 0);
		CenterWindow(hWndDlg);
		if (!InitControls(hWndDlg))
			ShowError(L"Cannot initialize dialog");
		break;

	case WM_CTLCOLOREDIT:
		if (!g_pSelectedFilter)
			return FALSE;
		if (g_pSelectedFilter->GetBrush() == NULL)
			g_pSelectedFilter->SetBrush(CreateSolidBrush(g_pSelectedFilter->GetColor()));
		SetBkColor((HDC)wParam, g_pSelectedFilter->GetColor());
		return (INT_PTR)(g_pSelectedFilter->GetBrush());

	case WM_COMMAND:

		if (wParam == IDOK) // User has hit the ENTER key.
		{
			UpdateFilterText(hWndDlg);

			// Check the current focus
			if (GetFocus() == GetDlgItem(hWndDlg, IDC_EDIT_TEXT))
			{
				// The focus is on the edit control, so do not close the dialog.
				// Move the focus to the next control in the dialog.
				PostMessage(hWndDlg, WM_NEXTDLGCTL, 0, 0L);
				return TRUE;
			}
			else
			{	// The focus is on the default button, so close the dialog.
				EndDialog(hWndDlg, TRUE);
				return FALSE;
			}
		}

		if (HIWORD(wParam) == CBN_SELCHANGE || HIWORD(wParam) == CBN_DROPDOWN)
		{
			UpdateFilterText(hWndDlg);
			UpdateControls(hWndDlg, (HWND)lParam);
			return TRUE;
		}

		if (((HWND)lParam) && (HIWORD(wParam) == BN_CLICKED))
		{
			COLORREF rgbResult;
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON_COLORS:
			{
				if (OpenColorPicker(hWndDlg, g_pSelectedFilter, rgbResult))
				{
					if (g_pSelectedFilter)
					{
						UpdateFilterText(hWndDlg);
						g_pSelectedFilter->SetColor(rgbResult);
						UpdateControls(hWndDlg, GetDlgItem(hWndDlg, IDC_COMBO_HIGHLIGHT_ID));
					}
				}
				return TRUE;
			}
			case IDOK:
			case IDCANCEL:
				EndDialog(hWndDlg, wParam);
			}
			return TRUE;
		}
	}
	return FALSE;
}
