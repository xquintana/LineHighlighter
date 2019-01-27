#include "resource.h"
#include "HighlightFilter.h"
#include <sstream>
#include <fstream>
#include <Shlobj.h>
#include <shlwapi.h>

#define CONFIG_FOLDER	L"\\Notepad++\\plugins\\Config"
#define INI_FILE		L"\\LineHighlighter.ini"


// Default highlight colors
unsigned long default_colors[MAX_FILTERS] = {
	RGB(233,183,255),
	RGB(175,238,238),
	RGB(204,255,153),
	RGB(255,222,173),
	RGB(253,167,165),
	RGB(0,255,255),
	RGB(0,235,13),
	RGB(255,255,0),
	RGB(255,165,0),
	RGB(255,0,0)
};


//////////////////////////////////////////////////////////////////////////////
// CHighlightFilter

CHighlightFilter::CHighlightFilter(string sText, unsigned long color)
{	
	m_sText = sText;	
	m_color = color;	
	m_hBrush = NULL;
	m_nMarker = 0;
	MakeParts();		
	Dump();
}

CHighlightFilter::~CHighlightFilter()
{	
	if (m_hBrush)
		DeleteObject(m_hBrush);
}

CHighlightFilter& CHighlightFilter::operator=(const CHighlightFilter& src)
{
	m_sText = src.m_sText;	
	m_color = src.m_color;	
	m_hBrush = NULL;
	m_parts = src.m_parts;
	m_nMarker = src.m_nMarker;
	return *this;
}

void CHighlightFilter::MakeParts()
{	
	istringstream f(m_sText);
	string str;
	m_parts.clear();
	while (std::getline(f, str, ';'))
	{
		if (str.length() > 0)
			m_parts.push_back(str);
	}
}

bool CHighlightFilter::IsEmpty()
{
	return (m_parts.size() <= 0);	
}

bool CHighlightFilter::CheckLine(char *line)
{	
	for(size_t i = 0; i < m_parts.size(); i++)
		if (strstr(line, m_parts[i].c_str()))
			return true;	
	return false;
}

void CHighlightFilter::Dump()
{	
	Trace(L"CHighlightItem::Dump-> text = '%s' - color = 0x%06x - %d parts", StringToWString(m_sText), m_color, m_parts.size());	
	for(size_t i=0; i< m_parts.size(); i++)
		Trace(L"CHighlightItem::Dump->    part %i = '%ws'", i, StringToWString(m_parts[i]));
}

void CHighlightFilter::SetText(string sText) 
{ 
	m_sText = sText; 
	MakeParts();
}

const wchar_t *CHighlightFilter::GetText()
{ 
	return StringToWString(m_sText);
}

void CHighlightFilter::SetColor(unsigned long color)
{
	m_color = color;	
	if (m_hBrush)
		DeleteObject(m_hBrush);
	m_hBrush = CreateSolidBrush(m_color);	
}

//////////////////////////////////////////////////////////////////////////////
// CHighlightManager

CFilterManager::CFilterManager()
{	
	GetConfigFilePath();	
}

bool CFilterManager::GetConfigFilePath()
{	
	SHGetSpecialFolderPath(NULL, m_sConfigFile, CSIDL_APPDATA, FALSE); // %APPDATA%\plugins\Config
	wcscat_s(m_sConfigFile, MAX_PATH, CONFIG_FOLDER);
	
	if (!DirectoryExists(m_sConfigFile))
	{	
		ShowError(L"The configuration folder does not exist:\r\n%s", m_sConfigFile);
		return false;		
	}
	wcscat_s(m_sConfigFile, MAX_PATH, INI_FILE);
	return true;
}

void CFilterManager::LoadDefault()
{
	m_filters.clear();
	while (m_filters.size() < MAX_FILTERS)
		m_filters.push_back(CHighlightFilter("", default_colors[m_filters.size()]));
}

bool CFilterManager::LoadConfigFile()
{
	m_filters.clear();	
	std::ifstream input(m_sConfigFile);	
	if (input)
	{
		std::string line;		
		while (std::getline(input, line))
		{
			if (!line.compare("[LineHighlighter]"))
			{				
				// Read text
				std::getline(input, line);
				string text = line;
				// Read color
				std::getline(input, line);
				string color = line;
				// Add filter to the list				
				CHighlightFilter filter(text, strtol(color.c_str(), NULL, 10));
				m_filters.push_back(filter);
				if (m_filters.size() >= MAX_FILTERS)
					return true;
			}
			else
			{				
				ShowError(L"Error reading config file:\r\n%s", m_sConfigFile);
				LoadDefault();
				return false;
			}
		}		
	}	
	else
	{	
		LoadDefault();
		return false;
	}	
	return true;
}

bool CFilterManager::SaveConfigFile()
{	
	char str[1024];
	std::ofstream out(m_sConfigFile);
	if (out.is_open())
	{
		for(size_t i=0; i< m_filters.size(); i++)	
		{
			if (i >= MAX_FILTERS)
				return true;
			sprintf_s(str, 1024, "[LineHighlighter]\n%ws\n%d\n", m_filters[i].GetText(), m_filters[i].GetColor());			
			out << str;
		}
		return true;
	}
	return false;
}

void CFilterManager::SetFilter(int index, char* sText, unsigned long color)
{
	if ((size_t)index < m_filters.size())
	{
		m_filters[index].SetText(sText);
		m_filters[index].SetColor(color);		
	}
}

void CFilterManager::Dump()
{
	Trace(L"CFilterManager::Dump-> %d items", m_filters.size());
	for(size_t i=0; i< m_filters.size(); i++)
		m_filters[i].Dump();
	Trace(L"CFilterManager::Dump-> exit");
}

bool CFilterManager::UpdateDocStatus(wstring sDoc)
{
	auto element = std::find(m_enabledDocs.begin(), m_enabledDocs.end(), sDoc);
	if (element == m_enabledDocs.end())
	{
		m_enabledDocs.push_back(sDoc);
		return true;
	}
	m_enabledDocs.erase(element);
	return false;
}

