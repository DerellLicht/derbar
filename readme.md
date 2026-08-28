### DerBar - a simple system-status bar

![DerBar without title bar](<images/Derbar_V1.28_image.jpg>)

DerBar is a simple, slightly-configurable status bar for monitoring key system and network parameters.  It is modeled on older status-monitoring programs such as StatBar, WinBar, and InfoBar, but without the configuration options of those other programs; it is designed for compactness and compatibility, not for turning parts on and off.  The few configuration options that are present, are accessed via the icon in the system tray.  You can move DerBar anywhere you want, by grabbing it and dragging it around, and it will remember where you leave it next time it runs.  

DerBar is compatible with all other programs and will not conflict with DirectX games, as each of the afore-mentioned tools would sometimes do.  It works fine with WinXP and Vista+, but would likely not work on Win98 famiily due to UNICODE support.

<hr>

This project is licensed under Creative Commons CC0 1.0 Universal;  

See the file LICENSE.txt for detailed information about this license
<hr>

Download [DerBar](https://derelllicht.42web.io/files/DerBar.zip) utility here  
Download [DerBar source code](https://github.com/DerellLicht/derbar) here

DerBar is written in C++, using standard Windows library calls.  

<hr>

#### building the application
This application is built using the MinGW toolchain; 
I recommend the [TDM](http://tdm-gcc.tdragon.net/) distribution, 
to avoid certain issues with library accessibility. 
The makefile also requires certain Cygwin tools (rm, make, etc).

#### NOTE: this program requires my ```der_libs``` submodule
If you clone the repository without the --recursive flag, 
you can recover the submodule later, with this command:

```git submodule update --init --recursive```

<hr>

See [Changelog](CHANGELOG.md) for the full revision history.
