#pragma once

#include "Utils.h"

#define MAX_FILTERS 10 // Maximum number of simultaneous filters/colors available

///////////////////////////////////////////////////////////////////////////////////
// class CHighlightFilter
// A filter is a set of strings that a line must match in order to be highlighted
// in a specific color.
//
class CHighlightFilter
{
public:
	CHighlightFilter(string sText, unsigned long color);
	~CHighlightFilter();
	virtual CHighlightFilter& operator=(const CHighlightFilter& src);

	bool CheckLine(char *line); // Returns true if the line is to be highlighted
	bool IsEmpty(); // Returns true if the filter's string is empty

	// Setters and Getters
	void   SetText(string sText);
	const wchar_t *GetText();
	void   SetColor(unsigned long color);
	unsigned long  GetColor() { return m_color; }
	void   SetBrush(HBRUSH hBrush) { m_hBrush = hBrush; }
	HBRUSH GetBrush() { return m_hBrush; }
	void   SetMarker(int nMarker) { m_nMarker = nMarker; }
	int    GetMarker() { return m_nMarker; }

	void Dump(); // For debug purposes

private:
	void MakeParts(); // Splits the full string by semicolons

private:
	string m_sText; // Full string of the filter, as entered by the user
	vector<string> m_parts; // Strings that will be matched for each input line
	unsigned long  m_color; // Highlight color
	HBRUSH m_hBrush; // Brush with the highlight color
	int m_nMarker;	// Scintilla marker
};


///////////////////////////////////////////////////////////////////////////////////
// class CFilterManager
// Manages a list of highlight filters.
//
class CFilterManager
{
public:
	CFilterManager();

	bool LoadConfigFile(); // Loads the filter configuration from disk
	bool SaveConfigFile(); // Saves the filter configuration to disk
	void LoadDefault();    // Loads default values for the filters (in case config file could not be read)
	void SetFilter(int index, char* sText, unsigned long color); // Initializes a new filter
	int  GetNumFilters() { return (int)m_filters.size(); } // Returns the number of filters
	CHighlightFilter* GetFilter(int idx) { return &(m_filters[idx]); } // Returns a filter by index
	void SetDocStatus(wstring sDoc, bool bEnable); // Sets the highlight status of a document
	bool ToggleDocStatus(wstring sDoc); // Changes the highlight status of a document and returns whether
										// the highlight has to be enabled for this document

	void Dump(); // For debug purposes

private:
	bool GetConfigFilePath(); // Obtains the full path of the configuration file
private:
	vector<CHighlightFilter> m_filters;	// List of highlight filters
	vector<wstring> m_enabledDocs; // List of documents that are currently highlighted
	wchar_t m_sConfigFile[MAX_PATH]; // Full path of the configuration file (config folder)
};