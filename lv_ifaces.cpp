//**********************************************************************
//  Copyright (c) 2009  Daniel D Miller
//  derbar.exe - Another WinBar application
//  lv_ifaces.cpp - network-interfaces listview
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
//  
//  Last Update:  04/10/09 09:50
//**********************************************************************
#include <windows.h>
#include <tchar.h>
// #define  _WIN32_IE   0x0500
#include <commctrl.h>
#include <iphlpapi.h>

#include "resource.h"
#include "common.h"
#include "derbar.h"
#include "images.h"
#include "ip_iface.h"

//  constants requiring specific values of _WIN32_IE 
#ifndef _WIN64
#define LVM_SETEXTENDEDLISTVIEWSTYLE   (LVM_FIRST+54)
#define LVS_EX_GRIDLINES               1
#define LVS_EX_SUBITEMIMAGES           2
#define LVM_SUBITEMHITTEST (LVM_FIRST+57)

//lint -esym(751, NMITEMACTIVATE)
//lint -esym(754, tagNMITEMACTIVATE::hdr, tagNMITEMACTIVATE::iItem, tagNMITEMACTIVATE::iSubItem)
//lint -esym(754, tagNMITEMACTIVATE::uNewState, tagNMITEMACTIVATE::uOldState)
//lint -esym(754, tagNMITEMACTIVATE::uChanged, tagNMITEMACTIVATE::lParam, tagNMITEMACTIVATE::uKeyFlags)

typedef struct tagNMITEMACTIVATE {
   NMHDR hdr;
   int iItem;
   int iSubItem;
   UINT uNewState;
   UINT uOldState;
   UINT uChanged;
   POINT ptAction;
   LPARAM lParam;
   UINT uKeyFlags;
} NMITEMACTIVATE, *LPNMITEMACTIVATE;
#endif

//  images.cpp
extern HIMAGELIST get_image_list(void) ;

//  window.mgr.cpp
static unsigned cxClient = 0 ;
// static unsigned cyClient = 0 ;

//****************************************************************
static bool lv_running = false ;
//lint -esym(844, lv_thread)
static HANDLE lv_thread = NULL ;
static DWORD dwRxThread ;
static HWND hwndLVtop = NULL ;

//****************************************************************
// Here we put the info on the column headers
//****************************************************************
// typedef struct _LVCOLUMN {
//     UINT mask;
//     int fmt;
//     int cx;    //  width of column in pixels
//     LPTSTR pszText;
//     int cchTextMax;
//     int iSubItem;
// #if (_WIN32_IE >= 0x0300)
//     int iImage;
//     int iOrder;
// #endif
// } LVCOLUMN, FAR *LPLVCOLUMN;

typedef struct lv_cols_s {
   TCHAR *txt ;
   uint cx ;
} lv_cols_t ;

static lv_cols_t const lv_cols[] = {
{ (TCHAR *) _T("Sel"),                40 },
{ (TCHAR *) _T("IP Address"),        100 },
{ (TCHAR *) _T("Network Interface"),   0 },
{ 0, 50 }} ;

//  This creates the columns, as well as defining header labels
static void lv_assign_column_headers(void)
{
   HWND hList = hwndLVtop ;

   // cyClient = (myRect.bottom - myRect.top) ;
   //  allow images on SubItems
   SendMessage(hwndLVtop, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_SUBITEMIMAGES, LVS_EX_SUBITEMIMAGES);  
   SendMessage(hwndLVtop, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_GRIDLINES, LVS_EX_GRIDLINES);  

   // SendMessageA (hwndLVtop, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);   // Set style
   // SendMessageA (hwndLVtop, LVM_SETTEXTBKCOLOR, 0, (LPARAM) CLR_NONE);
   HIMAGELIST hImageList = get_image_list() ;
   (void) ListView_SetImageList(hwndLVtop, hImageList, LVSIL_SMALL);  //lint !e522
   // SendMessage(hwndLVtop, LVM_SETIMAGELIST, (WPARAM) hImageList, LVSIL_SMALL);  //lint !e522

   // columns_created = true ;
   LVCOLUMN LvCol;                 // Make Column struct for ListView
   ZeroMemory(&LvCol, sizeof (LvCol)); // Reset Column
   // LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;   // Type of mask
   //  I will try to center elements within the columns, hoping that my
   //  LEDs will be centered.  Of course, it didn't work that way...
   //  Text is centered, images are not...
   LvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;   // Type of mask
   LvCol.fmt = LVCFMT_CENTER ;   //  only centers text
   unsigned dx = 0 ;

   // Inserting columns as much as we want
   // syslog("cxClient=%u\n", cxClient) ;
   unsigned idx ;
   for (idx=0; lv_cols[idx].txt != 0; idx++) {
      LvCol.pszText = lv_cols[idx].txt ;
      if (lv_cols[idx].cx == 0) {
         // LvCol.cx = cxClient - dx - 10;
         LvCol.cx = cxClient - dx - 5 ;
         // syslog("idx=%u, end of line, cxClient=%u\n", idx, cxClient) ;
      }
      else {
         LvCol.cx = lv_cols[idx].cx ;
         // syslog("idx=%u, cx=%u\n", idx, LvCol.cx) ;
      }
      SendMessage(hList, LVM_INSERTCOLUMN, idx, (LPARAM) &LvCol); // Insert/Show the column
      dx += (uint) LvCol.cx ;
   }
}

//***********************************************************************
static ip_iface_entry_p find_listview_row(uint lview_row)
{
   ip_iface_entry_p iftemp = NULL ;
   while (LOOP_FOREVER) {
      iftemp = get_next_ip_iface(iftemp);
      if (iftemp == NULL)
         break;
      if (iftemp->lview_row == lview_row)
         break;
   }
   return iftemp;
}

//***********************************************************************
static void update_active_led(ip_iface_entry_p iftemp, uint led_idx)
{
   LVITEM LvItem; //  ListView Item struct
   ZeroMemory(&LvItem, sizeof (LvItem));  // Reset Item Struct
   LvItem.iItem = iftemp->lview_row ;
   LvItem.iSubItem = 0 ;
   LvItem.mask = LVIF_IMAGE;   // Text Style
   LvItem.iImage = led_idx ;
   SendMessage(hwndLVtop, LVM_SETITEM, 0, (LPARAM) &LvItem);  // Send to the Listview
}

//******************************************************************
static void toggle_active_led(uint iSelect)
{
   ip_iface_entry_p iftemp = find_listview_row(iSelect) ;
   if (iftemp == NULL)
      return ;
   
   if (iftemp->if_active) {
      iftemp->if_active = false ;
      update_active_led(iftemp, LED_IDX_OFF) ;
   } else {
      iftemp->if_active = true ;
      update_active_led(iftemp, LED_IDX_GREEN) ;
   }
}

//******************************************************************
static void lv_add_data(void)
{
   TCHAR msgstr[81] ;
   LVITEM LvItem; //  ListView Item struct
   ZeroMemory(&LvItem, sizeof (LvItem));  // Reset Item Struct
   LvItem.cchTextMax = 80;   // Max size of text
   uint curr_rows = 0 ;
   ip_iface_entry_p iftemp = NULL ;
   while (LOOP_FOREVER) {
      iftemp = get_next_ip_iface(iftemp);
      if (iftemp == NULL)
         break;

      // iftemp->lview_row = curr_rows++ ;
      // syslog("index %u: [%s] %s\n", iftemp->dwIndex, iftemp->ipaddr_str, iftemp->iface_name) ;

      // { _T("Sel"),                40 },
      LvItem.mask = LVIF_TEXT | LVIF_IMAGE;   // Text Style
      LvItem.iItem = iftemp->lview_row ;   // choose item  (0-based counter)
      // this_object->lview_idx = curr_rows ;
      LvItem.iSubItem = 0; // Put in first Column

      LvItem.iImage = (iftemp->if_active)
                    ? LED_IDX_GREEN
                    : LED_IDX_OFF ;

      wsprintf(msgstr, " %u", curr_rows);
      LvItem.pszText = msgstr ;
      SendMessage(hwndLVtop, LVM_INSERTITEM, 0, (LPARAM) &LvItem);  // Send to the Listview

      //  reset structure elements to text-only
      LvItem.iImage = -1 ;
      LvItem.mask = LVIF_TEXT ;   // Text Style

      // { _T("IP Address"),         70 },
      LvItem.iSubItem++ ;
      LvItem.pszText = iftemp->ipaddr_str ;
      SendMessage (hwndLVtop, LVM_SETITEM, 0, (LPARAM) &LvItem);  // Send to the Listview

      // { _T("Network Interface"), 200 },
      LvItem.iSubItem++ ;
      LvItem.pszText = iftemp->iface_name ;
      SendMessage(hwndLVtop, LVM_SETITEM, 0, (LPARAM) &LvItem);  // Send to the Listview
   }
}

//******************************************************************
// determine which row receives the click
//******************************************************************
static int find_selected_row(NMHDR* pNMHDR) 
{
   // retrieve message info.
   LPNMITEMACTIVATE pItemAct = (LPNMITEMACTIVATE) (char *) pNMHDR;   //lint !e826

   LVHITTESTINFO  hitTest;
   ZeroMemory(&hitTest, sizeof(LVHITTESTINFO));
   hitTest.pt = pItemAct->ptAction;
   SendMessage(hwndLVtop, LVM_SUBITEMHITTEST, 0, (LPARAM) &hitTest);
   return hitTest.iItem;
}

//******************************************************************
static INT_PTR CALLBACK IfaceListProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   //***************************************************
   //  debug: log all windows messages
   //***************************************************
   // if (dbg_flags & DBG_WINMSGS) {
   //    switch (msg) {
   //    //  list messages to be ignored
   //    case WM_NCHITTEST:
   //    case WM_SETCURSOR:
   //    case WM_MOUSEMOVE:
   //    case WM_NCMOUSEMOVE:
   //    case WM_COMMAND:
   //    case WM_TIMER:
   //       break;
   //    default:
   //       syslog("Poll [%s]\n", lookup_winmsg_name(msg)) ;
   //       break;
   //    }
   // }

   //********************************************************************
   //  Windows message handler for this dialog
   //********************************************************************
   switch(msg) {
   case WM_INITDIALOG:
      hwndLVtop = GetDlgItem(hwnd, IDC_LV_IFACES) ;
      RECT myRect ;
      // GetWindowRect(hwndLVtop, &myRect) ;
      // GetClientRect(hwndLVtop, &myRect) ;
      GetClientRect(hwnd, &myRect) ;
      cxClient = (myRect.right - myRect.left) ;

      lv_assign_column_headers() ;

      lv_add_data() ;
      return TRUE ;
      
   case WM_NOTIFY:
      switch (LOWORD (wParam)) {
      case IDC_LV_IFACES:
         unsigned ncode = ((LPNMHDR) lParam)->code ;
         switch (ncode) {
         //**************************************************
         case NM_CLICK:
            int iSelect = find_selected_row((LPNMHDR) lParam) ;
            // syslog("LVM_GETNEXTITEM: %d\n", iSelect) ;
            toggle_active_led(iSelect) ;
            save_cfg_file() ;
            break;
         }  //lint !e744  switch ncode
      }  //lint !e744
      break;

   case WM_CLOSE:
      // syslog("Iface: WM_CLOSE\n") ;
      DestroyWindow(hwnd);
      return TRUE ;

   case WM_DESTROY:
      // syslog("Iface: WM_DESTROY\n") ;
      PostQuitMessage(0) ;
      return FALSE;

   default:
      return FALSE;
   }

   return FALSE;  //lint !e527
}

//**********************************************************************
static DWORD WINAPI IfaceThread(LPVOID iValue)
{
   // syslog("spawning iface thread\n") ;
   // cport_p this_port = (cport_p) iValue ;
   //  CreateDialogParamA note:
   //  Use NULL for the parent hwnd, if the dialog is being created in
   //  a separate thread.  Otherwise, the two message queues will 
   //  interact with each other.  In particular, anything which stops the
   //  child message queue will stop the parent as well!!
   HWND hwnd = CreateDialog(g_hinst, MAKEINTRESOURCE(IDD_LV_IFACES), 
      NULL, IfaceListProc) ;
   if (hwnd == NULL) {
      syslog("CreateDialog: %s\n", get_system_message()) ;
      return 0;
   }
   MSG Msg;
   // while(GetMessage(&Msg, hwnd,0,0)) {
   while(GetMessage(&Msg, NULL,0,0)) {
      if(!IsDialogMessage(hwnd, &Msg)) {
          TranslateMessage(&Msg);
          DispatchMessage(&Msg);
      }
   }
   // syslog("exit iface thread\n") ;
   lv_running = false ;
   return 0;
}  //lint !e715

//**********************************************************************
void spawn_ifaces_lview(HWND hwnd)
{
   if (lv_running) 
      return ;
   lv_running = true ;
   lv_thread = CreateThread(NULL, 0, IfaceThread, (VOID *) 0, 0, &dwRxThread);
   if(lv_thread == NULL) {
      syslog("CreateThread: %s\n", get_system_message()) ;
   }
}  //lint !e715

