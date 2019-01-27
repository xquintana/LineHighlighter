#ifndef PLUGINDEFINITION_H
#define PLUGINDEFINITION_H

#include "PluginInterface.h"

// Plugin name
const TCHAR NPP_PLUGIN_NAME[] = TEXT("Line Highlighter");

// Number of plugin commands
const int nbFunc = 2;

// Called while plugin loading
void pluginInit(HANDLE hModule);

// Called while plugin unloading
void pluginCleanUp();

// Initialization of the plugin commands
void commandMenuInit();

// Clean up any plugin command allocations
void commandMenuCleanUp();

// Function which sets the commands
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk = NULL, bool check0nInit = false);


// Command functions
void OnToggleHighlight(); // Invokes the function that toggles the highlight state
void ShowConfigDlg();  // Shows the dialog to configure the highlight filters and colors

// Enables or disables highlighting in the current document
void EnableHighlight(bool bEnable);

#endif //PLUGINDEFINITION_H
