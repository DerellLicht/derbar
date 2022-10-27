//****************************************************************************
//  Copyright (c) 2008-2022  Daniel D Miller
//  tooltips.cpp - tooltip functions/data
//
//  Written by:  Dan Miller 
//****************************************************************************
//  Usage:
//    HWND hToolTip = create_tooltips(hwnd, 150, 100, 10000) ;
//    add_program_tooltips(hwnd, hToolTip) ;
//****************************************************************************

#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0501
#include <windows.h>
#include <tchar.h>
#include <commctrl.h>

#include "iface_32_64.h"
#include "resource.h"
#include "common.h"

//  static tooltip-list struct
typedef struct tooltip_data_s {
   uint ControlID ;
   TCHAR *msg ;
} tooltip_data_t ;

//****************************************************************************
HWND create_tooltips(HWND hwnd, uint max_width, uint popup_msec, uint stayup_msec)
{
   HWND hToolTip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP,
         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, //lint !e569
         hwnd, NULL, GetModuleHandle(NULL), NULL);
   if (hToolTip == NULL) {
      syslog("ToolTip CreateWindowEx: %s\n", get_system_message()) ;
   } else {
      SendMessage(hToolTip, TTM_SETMAXTIPWIDTH, 0, max_width) ;
      SendMessage(hToolTip, TTM_SETDELAYTIME, TTDT_INITIAL, popup_msec) ;
      SendMessage(hToolTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, stayup_msec) ;
   }
   return hToolTip ;
}

//****************************************************************************
static void add_tooltip_target(HWND parent, HWND target, HWND hToolTip, TCHAR *msg)
{
   // static bool hex_dump_was_run = false ;
   TOOLINFO ti;
   ti.cbSize = sizeof(TOOLINFO);
   ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
   ti.hwnd = parent;
   ti.uId = (UINTx) target;
   ti.lpszText = msg ;
   LRESULT bresult = SendMessage(hToolTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
   if (bresult == 0) {
      syslog("TTM_ADDTOOL: %u: %s\n", target, get_system_message()) ;
   }
}  //lint !e550  ti

//****************************************************************************
//  CommPort dialog tooltips
//****************************************************************************
static tooltip_data_t const program_tooltips[] = {
{ IDS_CLR_FGND,      _T("Select foreground color of data fields")},
{ IDC_EDIT_FGND,     _T("Select foreground color of data fields")},
{ IDC_CLR_FGND,      _T("View color selector for foreground color" )},
{ IDS_CLR_BGND,      _T("Select background color of data fields")},
{ IDC_EDIT_BGND,     _T("Select background color of data fields")},
{ IDC_CLR_BGND,      _T("View color selector for background color" )},
{ IDM_ONTOP,         _T("Keep main dialog on top of other applications" )},
{ IDM_LOGIN_UPTIME,  _T("Show login label vs uptime, as appropriate" )},
{ IDM_WINMSGS,       _T("Show WinAPI debug messages in DebugView" )},
{ IDOK,              _T("Close this dialog and accept changes" )},

//  This is how to enter multi-line tooltips:
// { IDS_CP_SERNUM,     _T("The SEND CMD button will send COMMAND to the device with")
//                      _T("this Serial Number.  If Serial Number is 0, COMMAND is sent ")
//                      _T("to the broadcast address on the current port.") },

{ 0, NULL }} ;

void add_program_tooltips(HWND hwnd, HWND hwndToolTip)
{
   unsigned idx ;
   for (idx=0; program_tooltips[idx].ControlID != 0; idx++) {
      add_tooltip_target(hwnd, GetDlgItem(hwnd, program_tooltips[idx].ControlID),
         hwndToolTip, program_tooltips[idx].msg) ;
   }
}

