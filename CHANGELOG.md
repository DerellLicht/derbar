# derbar Changelog

## [1.29] - 2026-07-05
- Tweak positioning of controls in main dialog, make them more centered

## [1.28] - 2026-06-18
- Add hyperlink to the Github source code page, in About dialog

## [1.27] - 2026-06-11
- Clean up unused ImageList code in images.cpp

## [1.26] - 2026-04-13
- Set About dialog hyperlink color (in der_libs) to Windows COLOR_HOTLIGHT color

## [1.25] - 2025-10-26
- Add option to set colors of main-dialog static controls

## [1.24] - 2025-05-12
- Convert to Unicode, for compatibility with current der_libs

## [1.23] - 2024-12-03
- Show minimum free memory - analyze low-memory condition status

## [1.22] - 2024-03-27
- fixed: Windows could end up with logon entries which were later than current time.
- This would result in invalid logon time being displayed.

## [1.21] - 2023-02-26
- If RX Bytes/second is arbitrarily large (>20000), drop fractional
- portion of value in display, so rates > 100MB/sec will be readable
- Add tooltips to main dialog

## [1.20] - 2022-11-20
- Add option to show/hide seconds on uptime/login field

## [1.19] - 2022-10-26
- Converted to 64-bit compiler

## [1.18] - 2022-03-30
- Add tooltips to Options dialog controls

## [1.17] - 2021-06-06
- Add support for substituting login time for Uptime
- Enable left-click on Uptime field to toggle uptime/login display

## [1.16] - 2021-01-04
- Add WM_DWMCOLORIZATIONCOLORCHANGED command for redraw of icon colors

## [1.15] - 2020-02-07
- Add VS_VERSION_INFO to resource file

## [1.14] - 2019-06-22
- Double-click on DerBar dialog, now resets desktop icon colors

## [1.13] - 2019-05-27
- Enable toggling of CAPS/NUM/SCRL keys from program interface

## [1.12] - 2018-10-17
- Try to provide RED background for free memory, if it gets too low

## [1.11] - 2017-06-15
- Try to add right-click on main dialog, to *also* present the action menu
- Fix startup operations so ip_iface tables are build before reading config file
- Store show_winmsgs in INI file

## [1.10] - 2017-04-17
- Research into refresh messages

## [1.09] - 
- Move systray functionality to separate file
- Integrate ClearIconTray functions to here

## [1.08] - 
- Modify call to PdhGetFormattedCounterValue(), to try to eliminate the
- undocumented 0x800007D6 (PDH_CALC_NEGATIVE_DENOMINATOR) Error.
- About dialog - convert home website link from button to hyperlink

## [1.07] - 
- make the memory bars wider, so that 16GB data will display!
- try to detect if dialog is off-screen, and move it back onto display

## [1.06] - 
- Add option to move main dialog back to a visible position
- (to deal with desktop size changes)
- Add option to make main window stay on top

## [1.05] - 
- Add status of CapsLock/NumLock/ScrollLock keys

## [1.04] - 
- Replace interface-selection combo box in Options,
- with management via the listview dialog.

## [1.03] - 
- Add listview to show info for all interfaces

## [1.02] - 
- Allow user to select network interface to read

## [1.01] - 
- Added settable field colors

## [1.00] - 
- Initial release
