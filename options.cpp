//**********************************************************************
//  Copyright (c) 2009-2025  Daniel D Miller
//  options handler for DerBar application
//  
//  Written by:   Daniel D. Miller
//**********************************************************************
#include <windows.h>
#include <time.h>
#include <tchar.h>

#include "resource.h"
#include "common.h"
#include "derbar.h"
#include "winmsgs.h"
#include "tooltips.h"

bool show_winmsgs = false ;
bool keep_on_top = false ;
bool use_logon_time_for_uptime = false ;
bool show_seconds_for_uptime = true ;

static HWND hwndEditFgnd ;
static HWND hwndEditBgnd ;

extern void update_keep_on_top(void);

//****************************************************************************
//  Options dialog tooltips
//****************************************************************************
static tooltip_data_t const options_tooltips[] = {
{ IDS_CLR_FGND,      _T("Select foreground color of data fields")},
{ IDC_EDIT_FGND,     _T("Select foreground color of data fields")},
{ IDC_CLR_FGND,      _T("View color selector for foreground color" )},
{ IDS_CLR_BGND,      _T("Select background color of data fields")},
{ IDC_EDIT_BGND,     _T("Select background color of data fields")},
{ IDC_CLR_BGND,      _T("View color selector for background color" )},
{ IDM_ONTOP,         _T("Keep main dialog on top of other applications" )},
{ IDM_LOGIN_UPTIME,  _T("Show login label vs uptime, as appropriate" )},
{ IDM_LOGIN_SECONDS, _T("Show/hide seconds in uptime/login field" )},
{ IDM_WINMSGS,       _T("Show WinAPI debug messages in DebugView" )},
{ IDS_MIN_FREEMEM,   _T("Display minimum observed free memory (debug)" )},
{ IDC_MIN_FREEMEM,   _T("Display minimum observed free memory (debug)" )},
{ IDC_CLR_MIN_FMEM,  _T("Reset low-memory indicators (debug)" )},
{ IDOK,              _T("Close this dialog and accept changes" )},

//  This is how to enter multi-line tooltips:
// { IDS_CP_SERNUM,     _T("The SEND CMD button will send COMMAND to the device with")
//                      _T("this Serial Number.  If Serial Number is 0, COMMAND is sent ")
//                      _T("to the broadcast address on the current port.") },

{ 0, NULL }} ;

//  low-memory indicators
extern u64 min_freemem ;
extern bool isMemoryLow ;

//*******************************************************************
//  show minimum free memory
//*******************************************************************
static void show_min_free_memory(HWND hwnd)
{
   TCHAR msgstr[81] ;
   u64 min_free_mb = min_freemem / (1024 * 1024);
   convert_to_commas(min_free_mb, msgstr);
   SetWindowText(GetDlgItem(hwnd, IDC_MIN_FREEMEM), msgstr);
}
      
//*******************************************************************
static void reset_low_memory_indicators(HWND hwnd)
{
   isMemoryLow = false ;
   min_freemem = 0 ;
   show_min_free_memory(hwnd);
}

//******************************************************************
static INT_PTR CALLBACK OptionsProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   uint tempEditLength ;
   TCHAR msgstr[81] ;

   //***************************************************
   //  debug: log all windows messages 
   //***************************************************
   if (show_winmsgs) {
      switch (msg) {
      //  list messages to be ignored
      case WM_NCHITTEST:
      case WM_SETCURSOR:
      case WM_MOUSEMOVE:
      case WM_NCMOUSEMOVE:
         break;
      default:
         syslog(_T("Opt [%s]\n"), lookup_winmsg_name(msg)) ;
         break;
      }
   }

   //********************************************************************
   //  Windows message handler for this dialog
   //********************************************************************
   switch(msg) {
   case WM_INITDIALOG:
// #ifdef  __x86_64__      
#ifdef _WIN64
      SetClassLongPtr(hwnd, GCLP_HICON,   (LONG_PTR)LoadIcon(g_hinst, (LPCTSTR)IDI_MAINICON));
      SetClassLongPtr(hwnd, GCLP_HICONSM, (LONG_PTR)LoadIcon(g_hinst, (LPCTSTR)IDI_MAINICON));
#else
      SetClassLong(hwnd, GCL_HICON,   (LONG)LoadIcon(g_hinst, (LPCTSTR)IDI_MAINICON));
      SetClassLong(hwnd, GCL_HICONSM, (LONG)LoadIcon(g_hinst, (LPCTSTR)IDI_MAINICON));
#endif      

      //  label the dialog
      hwndEditFgnd = GetDlgItem(hwnd, IDC_EDIT_FGND) ;  // EDITTEXT   
      hwndEditBgnd = GetDlgItem(hwnd, IDC_EDIT_BGND) ;  // EDITTEXT   

      wsprintf(msgstr, L" 0x%06X", fgnd_edit) ;
      SetWindowText(hwndEditFgnd, msgstr);
      wsprintf(msgstr, L" 0x%06X", bgnd_edit) ;
      SetWindowText(hwndEditBgnd, msgstr);

      PostMessage(GetDlgItem(hwnd, IDM_WINMSGS),       BM_SETCHECK, show_winmsgs, 0) ;
      PostMessage(GetDlgItem(hwnd, IDM_ONTOP),         BM_SETCHECK, keep_on_top, 0) ;
      PostMessage(GetDlgItem(hwnd, IDM_LOGIN_UPTIME),  BM_SETCHECK, use_logon_time_for_uptime, 0) ;
      PostMessage(GetDlgItem(hwnd, IDM_LOGIN_SECONDS), BM_SETCHECK, show_seconds_for_uptime, 0) ;

      show_min_free_memory(hwnd) ;

      create_and_add_tooltips(hwnd, 150, 100, 10000, options_tooltips);
      return TRUE ;

   //********************************************************************
   //  menu/control messages
   //********************************************************************
   case WM_COMMAND:
      if (HIWORD (wParam) == BN_CLICKED) {
         TCHAR *tptr ;
         COLORREF temp_attr ;
         bool changed ;

         switch(LOWORD(wParam)) {
            
         case IDM_WINMSGS:
            show_winmsgs = (show_winmsgs) ? false : true ;
            return TRUE;

         case IDM_ONTOP:
            keep_on_top = (keep_on_top) ? false : true ;
            update_keep_on_top() ;
            return TRUE;

         case IDM_LOGIN_UPTIME:
            use_logon_time_for_uptime = (use_logon_time_for_uptime) ? false : true ;
            return TRUE;

         case IDM_LOGIN_SECONDS:
            show_seconds_for_uptime = (show_seconds_for_uptime) ? false : true ;
            return TRUE;

         case IDC_CLR_MIN_FMEM:
            reset_low_memory_indicators(hwnd) ;
            return TRUE;

         case IDC_CLR_FGND:
            temp_attr = select_color(fgnd_edit) ;
            //  if user cancels color entry, stick with 
            //  existing color selection.
            if (temp_attr != 0) {
               wsprintf(msgstr, L" 0x%06X", temp_attr) ;
               SetWindowText(hwndEditFgnd, msgstr);
            }
            return TRUE;

         case IDC_CLR_BGND:
            temp_attr = select_color(bgnd_edit) ;
            //  if user cancels color entry, stick with 
            //  existing color selection.
            if (temp_attr != 0) {
               wsprintf(msgstr, L" 0x%06X", temp_attr) ;
               SetWindowText(hwndEditBgnd, msgstr);
            }
            return TRUE;
            
         case IDOK: //  take the new settings
            changed = false ;
            tempEditLength = GetWindowTextLength (hwndEditFgnd);
            GetWindowText (hwndEditFgnd, msgstr, tempEditLength + 1);
            msgstr[tempEditLength] = 0;
            tptr = strip_leading_spaces(msgstr) ;
            temp_attr = (uint) _tcstoul(tptr, 0, 0) ;
            if (temp_attr != fgnd_edit) {
               fgnd_edit = temp_attr ;
               changed = true ;
               wsprintf(msgstr, L" 0x%06X", fgnd_edit) ;
               SetWindowText(hwndEditFgnd, msgstr);
            }

            tempEditLength = GetWindowTextLength (hwndEditBgnd);
            GetWindowText (hwndEditBgnd, msgstr, tempEditLength + 1);
            msgstr[tempEditLength] = 0;
            tptr = strip_leading_spaces(msgstr) ;
            temp_attr = (uint) _tcstoul(tptr, 0, 0) ;
            if (temp_attr != fgnd_edit) {
               bgnd_edit = temp_attr ;
               changed = true ;
               hbEdit = CreateSolidBrush(bgnd_edit) ;
               wsprintf(msgstr, L" 0x%06X", bgnd_edit) ;
               SetWindowText(hwndEditBgnd, msgstr);
            }

            // sel = SendMessageA(hwndIpIface, CB_GETCURSEL, 0, 0);
            // if (ip_iface_idx != sel) {
            //    ip_iface_idx = sel ;
            //    changed = TRUE ;
            // }

            update_uptime_label();
            if (changed) 
               save_cfg_file() ;

            DestroyWindow(hwnd);
            return TRUE ;

         case IDCANCEL: //  discard all changes
            // send_commands(this_port) ;
            DestroyWindow(hwnd);
            return TRUE ;

         default:
            return FALSE;
         }
      }
      return FALSE;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      return TRUE ;

   case WM_DESTROY:
      return TRUE ;

   default:
      return FALSE;
   }

   return FALSE;  //lint !e527
}  //lint !e715

//****************************************************************************
void open_options_dialog(HWND hwnd)
{
   // DialogBox(g_hinst, MAKEINTRESOURCE(IDD_OPTIONS), hwnd, OptionsProc) ;
   DialogBox(g_hinst, MAKEINTRESOURCE(IDD_OPTIONS), 0, OptionsProc) ;
}  //lint !e715  hwnd

