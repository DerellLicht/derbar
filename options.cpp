//**********************************************************************
//  Copyright (c) 2009-2017  Daniel D Miller
//  derbar.exe - Another WinBar application
//  derbar.cpp: main interface functions
//  
//  DerBar, its source code and executables, are Copyrighted in their
//  unmodified form by Daniel D Miller, and are distributed as free
//  software, with only one restriction:
//  
//  Any modified version of the program cannot be distributed with
//  the name DerBar.
//  
//  Other than this, the source code, executables, help files, and any
//  other related files are provided with absolutely no restriction 
//  on use, distribution, modification, commercial adaptation, 
//  or any other conditions.
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

bool show_winmsgs = true ;
bool keep_on_top = false ;

static HWND hwndEditFgnd ;
static HWND hwndEditBgnd ;
// static HWND hwndIpIface ;

extern void update_keep_on_top(void);

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
      // hwndIpIface  = GetDlgItem(hwnd, IDC_CBOX_IFACE) ;
      // fill_eth_iface_combobox(hwndIpIface, ip_iface_idx) ;

      wsprintf(msgstr, " 0x%06X", fgnd_edit) ;
      SetWindowText(hwndEditFgnd, msgstr);
      wsprintf(msgstr, " 0x%06X", bgnd_edit) ;
      SetWindowText(hwndEditBgnd, msgstr);

      // wsprintf(msgstr, " %u", this_port->odu_poll_period) ;
      // SetWindowText(this_port->hwndOptEdit, msgstr);
      // wsprintf(msgstr, " 0x%X", dbg_flags) ;
      // SetWindowText(this_port->hwndDbgEdit, msgstr);
      // wsprintf(msgstr, " %u", get_comm_timeout(this_port)) ;
      // SetWindowText(this_port->hwndCpTmo, msgstr);
      PostMessage(GetDlgItem(hwnd, IDM_WINMSGS), BM_SETCHECK, show_winmsgs, 0) ;
      PostMessage(GetDlgItem(hwnd, IDM_ONTOP), BM_SETCHECK, keep_on_top, 0) ;

      // wsprintf(msgstr, "Monitor Options") ;
      // SetWindowText(hwnd, msgstr) ;
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
            // show_winmsgs ^= 1 ;  //  unthematic
            keep_on_top = (keep_on_top) ? false : true ;
            update_keep_on_top() ;
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

