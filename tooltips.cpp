//****************************************************************************
//  Copyright (c) 2008-2023  Daniel D Miller
//  tooltips.cpp - tooltip functions/data
//
//  Written by:  Daniel D Miller 
//****************************************************************************
//  Usage:
//    HWND hToolTip = create_tooltips(hwnd, 150, 100, 10000) ;
//    add_tooltips(hwnd, hToolTip, name_of_tooltip_array) ;
//****************************************************************************

#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0501
#include <windows.h>
#include <commctrl.h>

#include "iface_32_64.h"
#include "common.h"
#include "tooltips.h"

//****************************************************************************
static HWND create_tooltips(HWND hwnd, uint max_width, uint popup_msec, uint stayup_msec)
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
static void add_tooltips(HWND hwnd, HWND hwndToolTip, tooltip_data_t const * const tooltip_array)
{
   unsigned idx ;
   
   for (idx=0; tooltip_array[idx].ControlID != 0; idx++) {
      add_tooltip_target(hwnd, GetDlgItem(hwnd, tooltip_array[idx].ControlID),
         hwndToolTip, tooltip_array[idx].msg) ;
   }
}

//****************************************************************************
void create_and_add_tooltips(HWND hwnd, uint max_width, uint popup_msec, uint stayup_msec,
                             tooltip_data_t const * const tooltip_array)
{
   HWND hToolTip = create_tooltips(hwnd, max_width, popup_msec, stayup_msec);
   add_tooltips(hwnd, hToolTip, tooltip_array);
}                             
