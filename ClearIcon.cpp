//*****************************************************************************************
//  Copyright (c) 2007-2017  Daniel D Miller
//  ClearIcon.cpp - a utility to change the desktop icon color.
//  
//  This program, its source code and executables, are Copyrighted in their
//  unmodified form by Daniel D Miller, and are distributed as free
//  software, with only one restriction:
//  
//  Any modified version of the program cannot be distributed with
//  the original name.
//  
//  Other than this, the source code, executables, help files, and any
//  other related files are provided with absolutely no restriction 
//  on use, distribution, modification, commercial adaptation, 
//  or any other conditions.
//  
//  Written by:   Daniel D. Miller
//*****************************************************************************************
//  
//  References:
//  The original techinique for setting the desktop color was taken
//  from the TransDesk utility, by Wei Ke (see his original notes below)
//  
//  The ChooseColor hook procedure, which allows ClearIcon to position the 
//  color dialog as desired, was extracted from:
//  "Programming the Windows 95 User Interface", Nancy Cluts, 1995, Chapter 06, cmndlg32.
//  
//  The technique of setting the dialog position by trapping WM_INITDIALOG
//  in the hook procedure, was taken from comments on the web.
//  
//*****************************************************************************************
//  Bugs:
//  This color-setting technique will not work if "drop shadows" are enabled
//  on the desktop.  
//  Unfortunately, the technique for turning off drop shadows, varies from OS to OS...
//*****************************************************************************************
//  Wei Ke's original notes:
//  
//  TransDesk.cpp
//  original author: Wei Ke [kw@iglyph.com]
//  file created: 5/16/98 3:46:29 PM
//  file last modified: 5/20/98 6:12:21 PM
//
//  Toggles desktop icon text background between transparent and Windows' default.
//  
//  THIS CODE, PROGRAM AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY 
//  OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
//  PARTICULAR PURPOSE.
//
//*****************************************************************************************
//  version    changes
//  =======    ======================================
//    1.00     Initial release
//*****************************************************************************************

//lint -esym(767, _WIN32_WINNT)
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>

#include "common.h"
#include "derbar.h"

//***********************************************************************
// alt_fg_attr=0x41c345
unsigned ci_attr = 0x41c345 ;

/****************************************************************************
 * This hook procedure, which allows ClearIcon to position the color dialog
 * as desired, was extracted from Nancy Cluts, Chapter 06, cmndlg32.
 * The technique of setting the dialog position by trapping WM_INITDIALOG
 * in the hook procedure, was taken from comments on the web.
****************************************************************************/
static BOOL APIENTRY ChooseColorHookProc(
        HWND hDlg,              /* window handle of the dialog box */
        UINT message,           /* type of message                 */
        UINT wParam,            /* message-specific information    */
        LONG lParam)
{
   switch (message) {
   case WM_INITDIALOG:
      {
      DWORD UFLAGS = SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW;
      SetWindowPos(hDlg, HWND_TOP, 400, 300, 0, 0, UFLAGS);
      }   
      break;
        
      // case WM_COMMAND:
      //     if (LOWORD(wParam) == IDOK)
      //     {
      //         // if (MessageBox( hDlg, "Are you sure you want to change the color?",
      //         //     "Information", MB_YESNO ) == IDYES )
      //         //     break;
      //         return (TRUE);
      // 
      //     }
      //  else if (LOWORD(wParam) == IDCANCEL)
      //     return TRUE;
      //     break;
   default:
      break;
   }
   return (FALSE);
}  //lint !e715


//****************************************************************
unsigned select_color(COLORREF orig_attr)
{
   static CHOOSECOLOR cc ;
   static COLORREF    crCustColors[16] ;

   ZeroMemory(&cc, sizeof(cc));
   cc.lStructSize    = sizeof (CHOOSECOLOR) ;
   // cc.rgbResult      = RGB (0x80, 0x80, 0x80) ;
   cc.rgbResult      = orig_attr ;
   cc.lpCustColors   = crCustColors ;
   cc.Flags          = CC_RGBINIT | CC_FULLOPEN ;
   cc.Flags          = CC_ENABLEHOOK;
   cc.lpfnHook       = (LPCCHOOKPROC)ChooseColorHookProc;
   cc.lpTemplateName = (LPTSTR)NULL;

   return (ChooseColor(&cc) == TRUE) ? cc.rgbResult : 0 ;
}

//***************************************************************************
//  This is Wei Ke's method for setting the background to transparent.  
//***************************************************************************
void reset_icon_colors(bool my_select_color)
{
   HWND hwnd = FindWindow(L"Progman", L"Program Manager");
   if ( hwnd == NULL ) {
      syslog(L"FindWindow failed\n") ;
      return;
   }
   hwnd = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", L"");
   if ( hwnd == NULL ) {
      syslog(L"FindWindowEx SHELLDLL_DefView failed\n") ;
      return;
   }
   hwnd = FindWindowEx(hwnd, NULL, L"SysListView32", NULL);
   if ( hwnd == NULL ) {
      //  Under WinXP:
      //  FindWindowEx SysListView32: 
      //    Cannot create a file when that file already exists.
      syslog(L"FindWindowEx SysListView32: %s\n", get_system_message()) ;
      return;
   }
      
   //  if color dialog was requested, run it
   if (my_select_color) {
      uint temp_attr = select_color(ci_attr) ;
      //  if user cancels color entry, 
      //  stick with existing color selection.
      if (temp_attr != 0) {
         ci_attr = temp_attr ;
         save_cfg_file();
      }
   }

   //  finally, set screen colors
   //  the preceding (void) declaration, is merely to avoid warnings about
   //  not using the return values from the macro functions.
   (void) ListView_SetTextBkColor(hwnd, CLR_NONE);  //lint !e522
   (void) ListView_SetTextColor(hwnd, ci_attr) ; //lint !e522
   InvalidateRect(hwnd, NULL, TRUE);
   UpdateWindow(hwnd);
}

