#pragma once

#include "Utils.h"

#define MAX_FILTERS 10	// Maximum number of simultaneous filters available

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class HighlightFilter
// A filter is a set of strings that a line must match in order to be highlighted in a specific color.
// Strings are stored as UTF-8.
//
class HighlightFilter
{
public:
	HighlightFilter(std::string sText, unsigned long color);
	~HighlightFilter();
	virtual HighlightFilter& operator=(const HighlightFilter& src);

	bool CheckLine(int nCodePage, char* sLine); // Returns true if the line is to be highlighted
	bool IsEmpty(); // Returns true if the filter's string is empty

	// Setters and Getters
	void        SetText(std::string sText); // string is UTF-8
	const char* GetText();
	void		SetColor(unsigned long color);
	ULONG       GetColor() { return m_color; }
	void	    SetBrush(HBRUSH hBrush) { m_hBrush = hBrush; }
	HBRUSH		GetBrush() { return m_hBrush; }
	void		SetMarker(int nMarker) { m_nMarker = nMarker; }
	int			GetMarker() { return m_nMarker; }

	void Dump(); // For debug purposes

private:
	void MakeParts(); // Splits the full string by semicolons

private:
	std::string m_sText; // Full string of the filter, as entered by the user, encoded in UTF-8 format
	std::vector<std::string> m_parts; // Strings encoded in UTF-8 that will be matched to each input line
	unsigned long  m_color; // Highlight color
	HBRUSH m_hBrush; // Brush with the highlight color
	int m_nMarker;	// Scintilla marker
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class FilterManager
// Manages a list of highlight filters.
//
class FilterManager
{
public:
	FilterManager();

	bool LoadConfigFile(); // Loads the filter configuration from disk
	bool SaveConfigFile(); // Saves the filter configuration to disk
	void LoadDefault();    // Loads default values for the filters (in case config file could not be read)
	void SetFilter(int index, char* sText, unsigned long color); // Initializes a new filter
	int  GetNumFilters() { return (int)m_filters.size(); } // Returns the number of filters
	HighlightFilter* GetFilter(int idx) { return &(m_filters[idx]); } // Returns a filter by index
	void SetDocStatus(std::wstring sDoc, bool bEnable); // Sets the highlight status of a document
	bool ToggleDocStatus(std::wstring sDoc); // Changes the highlight status of a document and returns whether
										// the highlight has to be enabled for this document

	void Dump(); // For debug purposes

private:
	bool GetConfigFilePath(); // Obtains the full path of the configuration file
private:
	std::vector<HighlightFilter> m_filters;	// List of highlight filters
	std::vector<std::wstring> m_enabledDocs; // List of documents that are currently highlighted
	WCHAR m_wsConfigFile[MAX_PATH]; // Full path of the configuration file (config folder)
};
