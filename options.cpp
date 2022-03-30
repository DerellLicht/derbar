//**********************************************************************
//  Copyright (c) 2009-2021  Daniel D Miller
//  derbar.exe - Another WinBar application
//  derbar.cpp: main interface functions
//  
//  Written by:   Daniel D. Miller
//**********************************************************************
#include <windows.h>
#include <time.h>
#ifdef _lint
#include <stdlib.h>
#endif

#include "resource.h"
#include "common.h"
#include "derbar.h"
#include "winmsgs.h"

bool show_winmsgs = false ;
bool keep_on_top = false ;
bool use_logon_time_for_uptime = false ;

static HWND hwndEditFgnd ;
static HWND hwndEditBgnd ;
// static HWND hwndIpIface ;

extern void update_keep_on_top(void);

//  tooltips.cpp
extern HWND create_tooltips(HWND hwnd, uint max_width, uint popup_msec, uint stayup_msec);
extern void add_program_tooltips(HWND hwnd, HWND hwndToolTip);

//  system.cpp
// extern void fill_eth_iface_combobox(HWND hwndIpIface, uint init_idx);

//******************************************************************
static BOOL CALLBACK OptionsProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   uint tempEditLength ;
   char msgstr[81] ;

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
         syslog("Opt [%s]\n", lookup_winmsg_name(msg)) ;
         break;
      }
   }

   //********************************************************************
   //  Windows message handler for this dialog
   //********************************************************************
   switch(msg) {
   case WM_INITDIALOG:
      SetClassLong(hwnd, GCL_HICON,   (LONG)LoadIcon(g_hinst, (LPCTSTR)IDI_MAINICON));
      SetClassLong(hwnd, GCL_HICONSM, (LONG)LoadIcon(g_hinst, (LPCTSTR)IDI_MAINICON));

      //  label the dialog
      hwndEditFgnd = GetDlgItem(hwnd, IDC_EDIT_FGND) ;  // EDITTEXT   
      hwndEditBgnd = GetDlgItem(hwnd, IDC_EDIT_BGND) ;  // EDITTEXT   

      wsprintf(msgstr, " 0x%06X", fgnd_edit) ;
      SetWindowText(hwndEditFgnd, msgstr);
      wsprintf(msgstr, " 0x%06X", bgnd_edit) ;
      SetWindowText(hwndEditBgnd, msgstr);

      PostMessage(GetDlgItem(hwnd, IDM_WINMSGS),      BM_SETCHECK, show_winmsgs, 0) ;
      PostMessage(GetDlgItem(hwnd, IDM_ONTOP),        BM_SETCHECK, keep_on_top, 0) ;
      PostMessage(GetDlgItem(hwnd, IDM_LOGIN_UPTIME), BM_SETCHECK, use_logon_time_for_uptime, 0) ;

      {
      HWND hToolTip = create_tooltips(hwnd, 150, 100, 10000) ;
      add_program_tooltips(hwnd, hToolTip) ;
      }
      return TRUE ;

   //********************************************************************
   //  menu/control messages
   //********************************************************************
   case WM_COMMAND:
      if (HIWORD (wParam) == BN_CLICKED) {
         char *tptr ;
         COLORREF temp_attr ;
         bool changed ;

         switch(LOWORD(wParam)) {
            
         case IDM_WINMSGS:
            // show_winmsgs ^= 1 ;  //  unthematic
            show_winmsgs = (show_winmsgs) ? false : true ;
            return TRUE;

         case IDM_ONTOP:
            keep_on_top = (keep_on_top) ? false : true ;
            update_keep_on_top() ;
            return TRUE;

         case IDM_LOGIN_UPTIME:
            use_logon_time_for_uptime = (use_logon_time_for_uptime) ? false : true ;
            return TRUE;

         case IDC_CLR_FGND:
            temp_attr = select_color(fgnd_edit) ;
            //  if user cancels color entry, stick with 
            //  existing color selection.
            if (temp_attr != 0) {
               wsprintf(msgstr, " 0x%06X", temp_attr) ;
               SetWindowText(hwndEditFgnd, msgstr);
            }
            return TRUE;

         case IDC_CLR_BGND:
            temp_attr = select_color(bgnd_edit) ;
            //  if user cancels color entry, stick with 
            //  existing color selection.
            if (temp_attr != 0) {
               wsprintf(msgstr, " 0x%06X", temp_attr) ;
               SetWindowText(hwndEditBgnd, msgstr);
            }
            return TRUE;
            
         case IDOK: //  take the new settings
            changed = false ;
            tempEditLength = GetWindowTextLength (hwndEditFgnd);
            GetWindowText (hwndEditFgnd, msgstr, tempEditLength + 1);
            msgstr[tempEditLength] = 0;
            tptr = strip_leading_spaces(msgstr) ;
            temp_attr = (uint) strtoul(tptr, 0, 0) ;
            if (temp_attr != fgnd_edit) {
               fgnd_edit = temp_attr ;
               changed = true ;
               wsprintf(msgstr, " 0x%06X", fgnd_edit) ;
               SetWindowText(hwndEditFgnd, msgstr);
            }

            tempEditLength = GetWindowTextLength (hwndEditBgnd);
            GetWindowText (hwndEditBgnd, msgstr, tempEditLength + 1);
            msgstr[tempEditLength] = 0;
            tptr = strip_leading_spaces(msgstr) ;
            temp_attr = (uint) strtoul(tptr, 0, 0) ;
            if (temp_attr != fgnd_edit) {
               bgnd_edit = temp_attr ;
               changed = true ;
               hbEdit = CreateSolidBrush(bgnd_edit) ;
               wsprintf(msgstr, " 0x%06X", bgnd_edit) ;
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

