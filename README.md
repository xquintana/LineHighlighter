# Line Highlighter

_Line Highlighter_ is a _Notepad++_ plugin that highlights lines using different background colors depending on the line content.

![Report](/screenshots/highlight.png)

For example, it can help read log files by standing out lines with specific keywords or values.

This plugin is intended to simulate the highlight feature of [DebugView](https://docs.microsoft.com/en-us/sysinternals/downloads/debugview)

## Usage

Under __Plugins__ > __Line Highlighter__ there are two commands:
* __Toggle Highlight__. Enables or disables the highlighter for the current document.
* __Highlight Filter__. Shows a dialog to configure the filters of the highlighter. There are up to 10 filters available.  

Each filter has two parameters:
* A string that a line must contain in order to be highlighted. Filters are case-sensitive. Different strings can be specified by separating them with a semicolon (';')
* The background color of the highlighter.

It is recommended to map a shortcut to the __Toggle Highlight__ command.  

Please note:
* When the highlighter is enabled, only the current document is processed.
* If the document is modified, the highlighter does not update automatically: it must be reapplied.
* The highlighter is disabled by default when _Notepad++_ is started.


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
* Changed the location of the output files of the project.