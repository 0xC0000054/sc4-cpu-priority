# sc4-cpu-priority

A DLL Plugin for SimCity 4 that extends the `-CPUPriority` command line argument.  

SimCity 4 only supports `-CPUPriority:Low`, and ignores any other value.    
This plugin adds support for the following additional CPUPriority values:

| Value | Notes |
--------|--------
| High ||
| AboveNormal ||
| Normal | The default value for a process, unless the parent process has a different value. |
| BelowNormal ||
| Idle | Identical to the game's `-CPUPriority:Low` option. |
 
The plugin can be downloaded from the Releases tab: https://github.com/0xC0000054/sc4-cpu-priority/releases

## System Requirements

* Windows 10 or later

The plugin may work on Windows 7 or later with the [Microsoft Visual C++ 2022 x86 Redistribute](https://aka.ms/vs/17/release/vc_redist.x86.exe)
installed, but I do not have the ability to test that.

## Installation

1. Close SimCity 4.
2. Copy `SC4CPUPriority.dll` into the Plugins folder in the SimCity 4 installation directory.
3. Start SimCity 4.

## Troubleshooting

The plugin should write a `SC4CPUPriority.log` file in the same folder as the plugin.    
The log contains status information for the most recent run of the plugin.

# License

This project is licensed under the terms of the MIT License.    
See [LICENSE.txt](LICENSE.txt) for more information.

## 3rd party code

[gzcom-dll](https://github.com/nsgomez/gzcom-dll/tree/master) Located in the vendor folder, MIT License.    
[Windows Implementation Library](https://github.com/microsoft/wil) MIT License    

# Source Code

## Prerequisites

* Visual Studio 2022

## Building the plugin

* Open the solution in the `src` folder
* Update the post build events to copy the build output to you SimCity 4 application plugins folder.
* Build the solution

## Debugging the plugin

Visual Studio can be configured to launch SimCity 4 on the Debugging page of the project properties.
I configured the debugger to launch the game in a window with the following command line:    
`-intro:off -CPUCount:1 -CPUPriority:high -w -CustomResolution:enabled -r1920x1080x32`

You may need to adjust the resolution for your screen.
