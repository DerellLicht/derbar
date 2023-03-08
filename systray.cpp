//**************************************************************************************
//  Copyright (c) 2017  Daniel D Miller
//  Handlers for system-tray access
//  
//  Collected by:   Daniel D. Miller
//**************************************************************************************
#include <windows.h>
// #define  SET_POPUP_MENU_BGND  1

#ifdef   SET_POPUP_MENU_BGND
//  from winuser.h, requires WINVER   0x0500
#define MIM_MAXHEIGHT   1
#define MIM_BACKGROUND  2
#define MIM_HELPID      4
#define MIM_MENUDATA    8
#define MIM_STYLE       16
#define MIM_APPLYTOSUBMENUS 0x80000000L
#endif

#include "resource.h"
#include "common.h"
#include "systray.h"

extern HINSTANCE g_hinst ;

static NOTIFYICONDATA NotifyIconData;

static HMENU hMenu = NULL ;
//***************************************************************
void load_tray_menu(void)
{
   HMENU hTopMenu = LoadMenu (g_hinst, MAKEINTRESOURCE(IDM_POPMENU)) ;
   if (hTopMenu == NULL) {
      syslog("LoadMenu: %s\n", get_system_message()) ;
   } 
   
   hMenu = GetSubMenu(hTopMenu, 0) ;
   if (hMenu == NULL) {
      syslog("GetSubMenu: %s\n", get_system_message()) ;
   } 
   
#ifdef   SET_POPUP_MENU_BGND
   // https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-menuinfo
   // change the menu backcolor:  doesn't work with MinGW32
   MENUINFO mnuInfo = {0};
   mnuInfo.cbSize   = sizeof(mnuInfo);
   mnuInfo.fMask    = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
   mnuInfo.hbrBack  = CreateSolidBrush(RGB(128,128,128));
   if(SetMenuInfo(hTopMenu,&mnuInfo)==FALSE) {
      syslog("Popup Menu background not changed");
   }   
#endif   
}

//***************************************************************
// put the icon into a system tray
//  Call from WM_INITDIALOG
//***************************************************************
//lint -esym(18, _NOTIFYICONDATAA::szTip) // c:\mingw\include\shellapi.h  166  Error 18: Symbol '_NOTIFYICONDATAA::szTip' redeclared (size) 
//lint -esym(18, _NOTIFYICONDATAW::szTip) // c:\mingw\include\shellapi.h  166  Error 18: Symbol '_NOTIFYICONDATAA::szTip' redeclared (size) 
void attach_tray_icon(HWND hwnd, char const * const szClassName)
{
   NotifyIconData.cbSize = sizeof (NOTIFYICONDATA);
   NotifyIconData.hWnd = hwnd;
   NotifyIconData.uID = IDI_MAINICON ;
   NotifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
   NotifyIconData.uCallbackMessage = WM_USER; // tray events will generate WM_USER event
   NotifyIconData.hIcon = (HICON) LoadIcon (g_hinst, MAKEINTRESOURCE (IDI_MAINICON));
   lstrcpy (NotifyIconData.szTip, szClassName); // max 64 characters
   //  getting string from STRINGTABLE in .rc file
   // LoadString(hInstance, IDS_APPTOOLTIP,nidApp.szTip,MAX_LOADSTRING);
   Shell_NotifyIcon (NIM_ADD, &NotifyIconData);
}


//**************************************************************************************
//  Handle messages generated by system tray application,
//     typicaly WM_USER or equivalent
//  The type of tray event which generated the message can be found in lParam.
//  
//  However, this code caused a situation where clicking outside the 
//  title bar did *not* make the menu go away.
//  Microsoft's solutions to this problem are documented in
//  http://support.microsoft.com/kb/q135788/
//  and are implemented below.
//**************************************************************************************
BOOL respond_to_tray_clicks(HWND hwnd, LPARAM lParam)
{
   POINT MouseCoordinates;
   switch (lParam)   {
   case WM_LBUTTONUP:
   case WM_RBUTTONUP:
      // display a tray menu
      SetForegroundWindow(hwnd); //  MSoft fix1 for stuck title bar
      GetCursorPos(&MouseCoordinates);
      TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN,
         MouseCoordinates.x, MouseCoordinates.y, 0, hwnd, NULL);
      PostMessage(hwnd, WM_NULL, 0, 0); //  MSoft fix2 for stuck title bar
      return TRUE;

   default:
      break;
   }
   return FALSE;
}

// remove the icon from a system tray and free the .dll handle
void release_systray_resource(void)
{
   Shell_NotifyIcon (NIM_DELETE, &NotifyIconData);
}

