//**************************************************************************************
//  version    changes
//   =======   ======================================
//    1.00     Initial release
//    1.01     Added settable field colors
//    1.02     Allow user to select network interface to read
//    1.03     Add listview to show info for all interfaces
//    1.04     Replace interface-selection combo box in Options,
//             with management via the listview dialog.
//    1.05     Add status of CapsLock/NumLock/ScrollLock keys
//    1.06     > Add option to move main dialog back to a visible position
//               (to deal with desktop size changes)
//             > Add option to make main window stay on top
//    1.07     > make the memory bars wider, so that 16GB data will display!
//             > try to detect if dialog is off-screen, and move it back onto display
//    1.08     > Modify call to PdhGetFormattedCounterValue(), to try to eliminate the
//               undocumented 0x800007D6 (PDH_CALC_NEGATIVE_DENOMINATOR) Error.
//             > About dialog - convert home website link from button to hyperlink
//    1.09     > Move systray functionality to separate file
//             > Integrate ClearIconTray functions to here
//    1.10     Research into refresh messages
//    1.11     > Try to add right-click on main dialog, to *also* present the action menu
//             > Fix startup operations so ip_iface tables are build before reading config file
//             > Store show_winmsgs in INI file
//    1.12     Try to provide RED background for free memory, if it gets too low
//    1.13     Enable toggling of CAPS/NUM/SCRL keys from program interface
//    1.14     Double-click on DerBar dialog, now resets desktop icon colors
//    1.15     Add VS_VERSION_INFO to resource file
//    1.16     Add WM_DWMCOLORIZATIONCOLORCHANGED command for redraw of icon colors
//    1.17     Add support for substituting login time for Uptime
//             Enable left-click on Uptime field to toggle uptime/login display
//    1.18     Add tooltips to Options dialog controls
//    1.19     Converted to 64-bit compiler
//    1.20     Add option to show/hide seconds on uptime/login field
//    1.21     > If RX Bytes/second is arbitrarily large (>20000), drop fractional
//               portion of value in display, so rates > 100MB/sec will be readable
//             > Add tooltips to main dialog
//    1.22     fixed: Windows could end up with logon entries which were later than current time.
//             This would result in invalid logon time being displayed.
//    1.23     Show minimum free memory - analyze low-memory condition status
//**************************************************************************************
#define VerNum    "V1.23"

