# Line Highlighter

_Line Highlighter_ is a _Notepad++_ plugin that highlights lines with different background colors depending on line content.

![Report](/screenshots/highlight.png)

For example, it can be useful to read log files by standing out lines with specific keywords or data.

## Usage

Under __Plugins__ > __Line Highlighter__ there are two commands:
* __Toggle Highlight__. Enables or disables the highlighter for the current document.
* __Highlight Filter__. Shows a dialog to configure the filters of the highlighter.

Each filter has two parameters:
* The string or set of strings that a line must contain in order to be highlighted. Different strings must be separated with a semicolon (';')
* The background color of the line.

There are up to 10 filters available.  

It is recommended to map a shortcut to the __Toggle Highlight__ command.  

Please note:
* When the highlighter is enabled, only the current document is processed.
* If the document is modified, the highlighter does not update automatically: it must be reapplied.
* The highlighter is disabled by default when _Notepad++_ is started.

This plugin works like the highlight feature of [DebugView](https://docs.microsoft.com/en-us/sysinternals/downloads/debugview)

## Installation

__Step 1:__ Download the ZIP file according to your _Notepad++_ platform:  
* [32-bit version](/download/LineHighlighter_32bits.zip).
* [64-bit version](/download/LineHighlighter_64bits.zip).

__Step 2:__ Unzip and copy the DLL into the _plugins_ folder of _Notepad++_.  

__Step 3:__ Restart _Notepad++_.  

## License

Licensed under the GNU General Public License v2.0 - see the [LICENSE](LICENSE) file for details.  

This project is based on the [plugin template](https://github.com/npp-plugins/plugintemplate) by Don HO, with the following changes:

* Added a function that modifies the background of a whole line if it contains certain substrings (_EnableHighlight_) and a command handler that invokes that function (_OnToggleHighlight_)
* Added a data structure in order to hold and manage the substrings used to highlight the lines (_CHighlightFilter/CFilterManager_)
* Added a dialog to edit the highlight filters (_HighlightDlg_)
* Added a command handler that shows this dialog (_ShowConfigDlg_)
* Changed the location of the output files