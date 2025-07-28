/*-----------------------------------------------------------------------------

    This is a part of the Microsoft Source Code Samples. 
    Copyright (C) 1995 Microsoft Corporation.
    All rights reserved. 
    This source code is only intended as a supplement to 
    Microsoft Development Tools and/or WinHelp documentation.
    See these sources for detailed information regarding the 
    Microsoft samples programs.

    MODULE:   About.c

    PURPOSE:  Implement the About dialog box for the program.

    FUNCTIONS:
        CmdAbout     - Creates the About dialog in response to menu selection
        AboutDlgProc - Processes messages for the About dialog
        InitAboutDlg - Initialzes about dialog controls

-----------------------------------------------------------------------------*/

#include <windows.h>
#include <tchar.h>   //  required for version.h

#include "resource.h"
#include "version.h"
#include "hyperlinks.h"

//  window.mgr.cpp
extern HINSTANCE g_hinst ;
// extern OSVERSIONINFO gOSV;

/*-----------------------------------------------------------------------------

FUNCTION: AboutDlgProc(HWND, UINT, WPARAM, LPARAM)

PURPOSE: Dialog procedure for the "About Box"

PARAMETERS:
    hdlg     - dialog window handle
    uMessage - window message
    wparam   - message parameter (depends on message value)
    lparam   - message prarmeter (depends on message value)

HISTORY:    Date:      Author:     Comment:
            10/27/95   AllenD      Wrote it

-----------------------------------------------------------------------------*/
//static BOOL CALLBACK AboutDlgProc(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
static INT_PTR CALLBACK AboutDlgProc(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
#define  BUF_LEN  255
   wchar_t buf[BUF_LEN+1];
   
   switch(uMessage) {
   case WM_INITDIALOG:
      SetWindowText(GetDlgItem(hdlg, IDC_VERNUM), VerNum) ;
      ConvertStaticToHyperlink(hdlg, IDC_WEBLINK);
      break;

   case WM_COMMAND:
      switch (LOWORD(wparam)) {
      case IDC_WEBLINK:
         // ShellExecute(hdlg, _T("open"), _T("http://derelllicht.42web.io/DerBar.html"), _T(""), _T(""), SW_SHOW);
         GetDlgItemText(hdlg, IDC_WEBLINK, buf, BUF_LEN);
         ShellExecute(hdlg, L"open", buf, L"", L"", SW_SHOW);
         return TRUE;
         
      case IDOK:
      case IDCANCEL:
         EndDialog(hdlg, TRUE);
         return TRUE;
      }  //lint !e744  switch with no default
      break;
   }  //lint !e744  switch with no default

   return FALSE;
}  //lint !e715

/*-----------------------------------------------------------------------------
FUNCTION: CmdAbout( HWND )
PARAMETERS:
    hwnd - Owner of the window
PURPOSE: Creates the modal About dialog
-----------------------------------------------------------------------------*/
BOOL CmdAbout(HWND hwnd)
{
    // syslog("WM_COMMAND: IDM_ABOUT\n");
    // DialogBox(g_hinst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);
    DialogBox(g_hinst, MAKEINTRESOURCE(IDD_ABOUT), 0, AboutDlgProc);
    return 0;
}  //lint !e715  hwnd
