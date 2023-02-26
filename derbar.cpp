//**************************************************************************************
//  Copyright (c) 2009-2023  Daniel D Miller
//  derbar.exe - Another WinBar application
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
//  derbar.cpp: main interface functions
//  
//  Written by:   Daniel D. Miller
//**************************************************************************************

//lint -esym(767, _WIN32_WINNT)
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>   //  for sprintf, for %f support
#include <time.h>
#include <tchar.h>

#include "iface_32_64.h"
#include "resource.h"
#include "version.h"
#include "common.h"
#include "derbar.h"
#include "images.h"
#include "winmsgs.h"
#include "systray.h"
#include "tooltips.h"

#define  USE_TIMER_TEST

#define  USE_CPU_UTIL   1

//  system.cpp
extern void update_memory_readings(void);
extern void build_iface_tables(void);
extern void update_iface_counters(void);
extern void cpu_usage_setup(void);
extern double cpu_usage_report(void);
extern void cpu_usage_release(void);
extern DWORD SampleMsec ;

//  about.cpp
extern BOOL CmdAbout(HWND hwnd);

//  options.cpp
extern void open_options_dialog(HWND hwnd);

//  lv_ifaces.cpp
extern void spawn_ifaces_lview(HWND hwnd);

//  login_lsa.cpp
//lint -esym(752, get_max_logon_time)
time_t get_max_logon_time(void);
time_t get_logon_time(void);

//  dialog color settings    0Oo 1lIi|  
static HBRUSH hbLogo = 0 ;
HBRUSH hbEdit = 0 ;
COLORREF fgnd_edit = WIN_BGREEN ;
COLORREF bgnd_edit = WIN_GREY ;

//********************************************************
static const unsigned SZ1GB = (unsigned) (1024 * 1024 * 1024) ;
static const unsigned SZ1MB = (unsigned) (1024 * 1024) ;

//  system.cpp
extern u64 freemem ;
extern u64 totalmem ;
extern uint RxBytesPerSec ;
extern uint TxBytesPerSec ;

//***********************************************************************
static char szClassName[] = "DerBar" ;

HINSTANCE g_hinst = 0;

static UINT timerID = 0 ;
static bool isMemoryLow = false ;

//  definitions for dialog controls
//lint -esym(844, hwndDerBar)
static HWND hwndDerBar ;
static HWND hwndFreeMem ;
static HWND hwndTotalMem ;
static HWND hwndUptime ;
static HWND hwndSUptime ;
static HWND hwndRxBytes ;
static HWND hwndTxBytes ;
static HWND hwndCpuTime ;
static HWND hwndMainDialog = 0 ;

static HWND hwndKbdCaps ;
static HWND hwndKbdNum  ;
static HWND hwndKbdScrl ;

//****************************************************************************
//  Main dialog tooltips
//****************************************************************************
static tooltip_data_t const main_tooltips[] = {
{ IDS_MEMORY,   _T("Show Memory info" )},
{ IDS_FREEMEM,  _T("Show Free Memory" )},
{ IDC_FREEMEM,  _T("Show Free Memory" )},
{ IDS_TOTALMEM, _T("Show Total Memory" )},
{ IDC_TOTALMEM, _T("Show Total Memory" )},
{ IDS_UPTIME,   _T("Show Uptime/Login Time" )},
{ IDC_UPTIME,   _T("Show Uptime/Login Time" )},
{ IDS_RXBYTES,  _T("Show Network Receive rate (KBytes/second)" )},
{ IDC_RXBYTES,  _T("Show Network Receive rate (KBytes/second)" )},
{ IDS_TXBYTES,  _T("Show Network Transmit rate (KBytes/second)" )},
{ IDC_TXBYTES,  _T("Show Network Transmit rate (KBytes/second)" )},
{ IDS_CPUTIME,  _T("Show CPU Utilization" )},
{ IDC_CPUTIME,  _T("Show CPU Utilization" )},
{ IDC_KBD_CAPS, _T("Show/toggle CapsLock state" )},
{ IDC_KBD_NUM , _T("Show/toggle NumLock state" )},
{ IDC_KBD_SCRL, _T("Show/toggle ScrollLock state" )},


//  This is how to enter multi-line tooltips:
// { IDS_CP_SERNUM,     _T("The SEND CMD button will send COMMAND to the device with")
//                      _T("this Serial Number.  If Serial Number is 0, COMMAND is sent ")
//                      _T("to the broadcast address on the current port.") },

{ 0, NULL }} ;

//*******************************************************************
static uint screen_width  = 0 ;
static uint screen_height = 0 ;

static void get_monitor_dimens(HWND hwnd)
{
   HMONITOR currentMonitor;      // Handle to monitor where fullscreen should go
   MONITORINFO mi;               // Info of that monitor
   currentMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
   mi.cbSize = sizeof(MONITORINFO);
   if (GetMonitorInfo(currentMonitor, &mi) != FALSE) {
      screen_width  = mi.rcMonitor.right  - mi.rcMonitor.left ;
      screen_height = mi.rcMonitor.bottom - mi.rcMonitor.top ;
   }
}

//*******************************************************************
//  if current dialog position is offscreen, move it safely onscreen.
//  The actual move will be done later by set_window_position()
//  
//  players: x_pos, y_pos, hwndMainDialog 
//*******************************************************************
static void verify_screen_position(void)
{
   if (x_pos > screen_width  ||  y_pos > screen_height) {
      x_pos = 0 ;
      y_pos = screen_height - 100 ;
   }
}

//*******************************************************************
static void save_window_position(HWND hwnd)
{
   RECT rect ;
   GetWindowRect(hwnd, &rect) ;
   x_pos = rect.left ;
   y_pos = rect.top ;
   save_cfg_file() ;
}

//  use 20 for WS_EX_APPWINDOW
//  use 16 for WS_EX_TOOLWINDOW
// #define  TBAR_DY  16

//******************************************************************
void update_uptime_label(void)
{
   if (use_logon_time_for_uptime) {
      SetWindowText(hwndSUptime, "Login") ;
   }
   else {
      SetWindowText(hwndSUptime, "Uptime") ;
   }
}

//*******************************************************************
static void relocate_main_dialog(HWND hwnd)
{
   SetWindowPos(hwnd, NULL, 0, 100, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME);
   x_pos = 0 ;
   y_pos = 100 ;
   save_cfg_file() ;
}

//*******************************************************************
static void toggle_title_bar(HWND hwnd)
{
   DWORD CurrentStyles = GetWindowLong( hwnd, GWL_STYLE );
   CurrentStyles ^= WS_CAPTION ;
   SetWindowLong( hwnd, GWL_STYLE, CurrentStyles);
   bool is_caption_visible = (CurrentStyles & WS_CAPTION) ? true : false ;
   RECT rect ;

   //  SM_CYCAPTION gives the caption height for WS_EX_APPWINDOW.
   //  SM_CYSMCAPTION gives the caption height for WS_EX_TOOLWINDOW.
   //  not for WS_EX_TOOLWINDOW, which I'm using.
   DWORD ExStyles = GetWindowLong( hwnd, GWL_EXSTYLE );
   uint caption_cy ;
   if (ExStyles & WS_EX_APPWINDOW) {
      caption_cy = (uint) GetSystemMetrics(SM_CYCAPTION) ;
      // syslog("WS_EX_APPWINDOW is active\n") ;
   }
   else {
      caption_cy = (uint) GetSystemMetrics(SM_CYSMCAPTION) ;
      // syslog("WS_EX_TOOLWINDOW is active\n") ;
   }
   // syslog("Caption CY=%d\n", caption_cy) ;
   GetWindowRect(hwnd, &rect) ;
   uint dy = rect.bottom - rect.top ;
   uint y0 = rect.top ;
   uint dx = rect.right - rect.left ;
   if (is_caption_visible) {
      y0 -= caption_cy ;
      dy += caption_cy ;
      tbar_on = 1 ;
   } else {
      y0 += caption_cy ;
      dy -= caption_cy ;
      tbar_on = 0 ;
   }
   // SetWindowPos( hwnd, NULL, rect.left, y0, dx, dy, SWP_NOSIZE | SWP_NOZORDER | SWP_NOMOVE | SWP_DRAWFRAME );
   SetWindowPos( hwnd, NULL, rect.left, y0, dx, dy, SWP_NOZORDER | SWP_DRAWFRAME );
   x_pos = rect.left ;
   y_pos = y0 ;
   save_cfg_file() ;
}

//*******************************************************************
//  used *only* in WM_INITDIALOG
//*******************************************************************
static void set_window_position(HWND hwnd)
{
   RECT rect ;
   GetWindowRect(hwnd, &rect) ;
   uint dy = rect.bottom - rect.top ;
   uint dx = rect.right - rect.left ;
   uint caption_cy = (uint) GetSystemMetrics(SM_CYSMCAPTION) ;

   if (tbar_on) {
      SetWindowPos(hwnd, NULL, x_pos, y_pos, dx, dy, SWP_NOZORDER | SWP_DRAWFRAME );
   } else {
      //  well, this sequence is not very efficient, but it works,
      //  and it's only run once when program is started...
      SetWindowPos(hwnd, NULL, x_pos, y_pos, dx, dy, SWP_NOZORDER | SWP_DRAWFRAME );
      toggle_title_bar(hwnd);
      y_pos -= caption_cy ;
      SetWindowPos( hwnd, NULL, x_pos, y_pos, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME );
   }

   save_cfg_file() ;
}

//***********************************************************************
//  this is a debug function to verify the 1000-msec main-loop timer
//***********************************************************************
#ifdef  USE_TIMER_TEST

//  if timer debugging was actually active,
//  we would want to be monitoring these two variables somewhere
static uint min_second_ticks = 0 ;
static uint max_second_ticks = 0 ;

static void update_time_count(void)
{
//lint -esym(843, cpu_freq_hz)   // Variable could be declared as const
   static uint cpu_freq_hz = 0 ;
   static u64 prev_ticks = 0 ;
   
   LARGE_INTEGER li_value ;
   if (cpu_freq_hz == 0) {
      QueryPerformanceFrequency(&li_value) ;
      cpu_freq_hz = (uint) li_value.LowPart ;   //lint !e1013 !e40
      // cpu_freq_mhz = cpu_freq_hz / 1000 ;
      // if (dbg_flags & DBG_VERBOSE)
      //    syslog("CPU freq=%u hz\n", cpu_freq_hz) ;
   }
   QueryPerformanceCounter(&li_value) ;
   u64 curr_ticks = li_value.QuadPart ;

   // runtime_seconds++ ;
   // DWORD curr_ticks = GetTickCount() ;
   //  check for counter wrap
   // if (prev_ticks == 0  ||  curr_ticks < prev_ticks) {
   if (prev_ticks == 0) {
      //  for now, do nothing here
   } else {
      DWORD diff_ticks = (uint) (curr_ticks - prev_ticks) ;
      if (diff_ticks == 0) {
         // zero_ticks_count++ ;
      } else if (max_second_ticks < diff_ticks) {
         // if (dbg_flags & DBG_VERBOSE)
         //    syslog("new max ticks: %u (%u msec)\n", diff_ticks, diff_ticks/cpu_freq_mhz) ;
         max_second_ticks = diff_ticks ;
      } else if (min_second_ticks == 0  ||  min_second_ticks > diff_ticks) {
         // if (dbg_flags & DBG_VERBOSE)
         //    syslog("new min ticks: %u (%u msec)\n", diff_ticks, diff_ticks/cpu_freq_mhz) ;
         min_second_ticks = diff_ticks ;
      }
   }
   prev_ticks = curr_ticks ;
}
#endif

//*******************************************************************
static char uptime_str[81] = "0" ;

static void update_uptime(void)
{
   DWORD gtc ;
   if (use_logon_time_for_uptime) {
      gtc = (DWORD) get_logon_time();
   }
   else {
      gtc = GetTickCount() ;  //  get uptime in msec
      gtc /= 1000 ;  //  convert to seconds
   }
   DWORD secs = gtc % 60 ;
   gtc /= 60 ; //  convert to minutes
   DWORD mins = gtc % 60 ;
   gtc /= 60 ; //  convert to hours
   DWORD hours = gtc % 24 ;
   gtc /= 24 ; //  convert to days
   // int slen = wsprintf(uptime_str, "Uptime: ") ;
   int slen = 0 ;
   if (gtc > 0) {
      slen += wsprintf(&uptime_str[slen], "%ud %uh %um", gtc, hours, mins) ;
   }
   else if (hours > 0) {
      slen += wsprintf(&uptime_str[slen], "%uh %um", hours, mins) ;
   }
   else {
      slen += wsprintf(&uptime_str[slen], "%um", mins) ;
   }
      
   if (show_seconds_for_uptime) {
      slen += wsprintf(&uptime_str[slen], " %us", secs) ;
   }
}

//*******************************************************************
static unsigned kbd_state = 0 ;
#define  KEY_ACTIVE     1  

static void update_keyboard_state(void)
{
   kbd_state = 0 ;
   SHORT kbd_status = GetKeyState(VK_CAPITAL) ;
   if (kbd_status & KEY_ACTIVE)  kbd_state |= 4 ;

   kbd_status = GetKeyState(VK_NUMLOCK) ;
   if (kbd_status & KEY_ACTIVE)  kbd_state |= 2 ;

   kbd_status = GetKeyState(VK_SCROLL) ;
   if (kbd_status & KEY_ACTIVE)  kbd_state |= 1 ;

   // SetWindowText(hwndKbdState, kbd_state_str[kbd_state]) ;
   SetWindowText(hwndKbdCaps, (kbd_state & 4) ? "C" : "c") ;
   SetWindowText(hwndKbdNum , (kbd_state & 2) ? "N" : "n") ;
   SetWindowText(hwndKbdScrl, (kbd_state & 1) ? "S" : "s") ;
}

//*******************************************************************
static double CpuTime = 0.0 ;

static void read_system_data(void)
{
   static unsigned sub_count = 0 ;
   if (++sub_count == 4) {
      sub_count = 0 ;
      update_uptime() ;
      update_memory_readings() ;
      update_iface_counters() ;
#ifdef   USE_CPU_UTIL
      CpuTime = cpu_usage_report() ;
#endif
   }
   update_keyboard_state() ;
}

//*******************************************************************
static void update_data_fields(void)
{
   char msgstr[81] ;
   //  update editable fields

   isMemoryLow = (freemem < (totalmem / 10)) ? true : false ;
   
   if (freemem < SZ1GB) 
      sprintf(msgstr, "%u MB", (unsigned) (freemem / SZ1MB)) ;
   else
      sprintf(msgstr, "%.2f GB", (double) freemem / SZ1GB) ;
   SetWindowText(hwndFreeMem,  msgstr) ;
   
   if (totalmem < 1000000000U) 
      sprintf(msgstr, "%u MB", (unsigned) (totalmem / SZ1MB)) ;
   else
      sprintf(msgstr, "%.2f GB", (double) totalmem / SZ1GB) ;
   SetWindowText(hwndTotalMem, msgstr) ;

   SetWindowText(hwndUptime, uptime_str) ;

   double rxBps = (double) RxBytesPerSec / SampleMsec ;
   if (rxBps < 20000.0) {
      sprintf(msgstr, "%.1f", rxBps) ;
   } 
   else {
      sprintf(msgstr, "%.0f", rxBps) ;
   }
   SetWindowText(hwndRxBytes, msgstr) ;
   sprintf(msgstr, "%.2f", (double) TxBytesPerSec / SampleMsec) ;
   SetWindowText(hwndTxBytes, msgstr) ;

   sprintf(msgstr, "%u", (unsigned) (CpuTime + 0.5)) ;
   SetWindowText(hwndCpuTime, msgstr) ;
}

//*******************************************************************
void update_keep_on_top(void)
{
   if (keep_on_top) {
      SetWindowPos (hwndMainDialog, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
   } else {
      SetWindowPos (hwndMainDialog, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );               
   }
}

//*************************************************************************************
//  info on toggling keyboard states was obtained from:
//  https://support.microsoft.com/en-us/help/127190/
//     howto-toggle-the-num-lock-caps-lock-and-scroll-lock-keys
//  
//  Later note: just be aware that this function is somewhat misnamed,
//              now that it is also responsible for clicks in the Uptime/Logon field
//*************************************************************************************
#define PROP_KBD_PROC    TEXT("_DerBarKeys_Original_Proc_")

static LRESULT CALLBACK KbdFlagsProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   BYTE keyState[256];
   //  debug function: see what messages are being received
   if (show_winmsgs) {
      switch (message) {
      //  list messages to be ignored
      case WM_TIMER:
      case WM_SETTEXT:
      case WM_NCHITTEST:
      case WM_CTLCOLORSTATIC:
      case WM_SETCURSOR:
      case WM_NCMOUSEMOVE:
      case WM_MOUSEMOVE:
         break;
      default:
         syslog("DerBarSC [%X]: [%s]\n", hwnd, lookup_winmsg_name(message)) ;
         break;
      }
   }
   // kbd hwnd: C0013023E N0013023C S001906D6
   WNDPROC pfnOrigProc = (WNDPROC) GetProp(hwnd, PROP_KBD_PROC);   //lint !e611
   
   switch (message)
   {
   case WM_LBUTTONUP:
      GetKeyboardState((PBYTE)&keyState[0]);
      // syslog("kbd hwnd: hwnd:%08X C:%08X N:%08X S:%08X\n", hwnd, hwndKbdCaps, hwndKbdNum, hwndKbdScrl);
      if (hwnd == hwndKbdCaps) {
         // Simulate a key press
         keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );

         // Simulate a key release
         keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      } else
      if (hwnd == hwndKbdNum) {
         // Simulate a key press
         keybd_event( VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );

         // Simulate a key release
         keybd_event( VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      } else
      if (hwnd == hwndKbdScrl) {
         // Simulate a key press
         keybd_event( VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );

         // Simulate a key release
         keybd_event( VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      } else
      if (hwnd == hwndUptime) {
         use_logon_time_for_uptime = (use_logon_time_for_uptime) ? false : true ;
         update_uptime_label();
         save_cfg_file() ;
      }
      break;

   default:
      break;   
   }  //lint !e744

   return CallWindowProc(pfnOrigProc, hwnd, message, wParam, lParam);
}

//*******************************************************************
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   // int result ;
   // static bool show_winmsgs = true ;
   // static bool lbutton_active = false ;
   char msgstr[81] ;
   static bool first_pass_done = false ;
   static bool moving_via_title_bar = false ;
   HDC hdc ;

   //  debug function: see what messages are being received
   if (show_winmsgs) {
      switch (message) {
      //  list messages to be ignored
      case WM_CTLCOLORSTATIC:
      case WM_NCHITTEST:
      case WM_SETCURSOR:
      case WM_TIMER:
      case WM_NCMOUSEMOVE:
      case WM_MOUSEWHEEL:
      case WM_ENTERIDLE:
         break;
      default:
         // syslog("MON: [%s]\n", get_winmsg_name(result));
         syslog("DerBar: [%s] %08X\n", lookup_winmsg_name(message), message) ;
         break;
      }
   }

   switch (message) {
   // case WM_CREATE:
   case WM_INITDIALOG:
      wsprintf(msgstr, "DerBar %s", VerNum) ;
      SetWindowText(hwnd, msgstr) ;
      hwndMainDialog = hwnd ;
      get_monitor_dimens(hwnd);

      //  read configuration *before* creating edit fields
      verify_screen_position() ;
      read_system_data() ;
      update_keep_on_top() ;

      //  edit fields
      hwndDerBar   = GetDlgItem(hwnd, IDC_DERBAR) ;
      hwndFreeMem  = GetDlgItem(hwnd, IDC_FREEMEM) ;
      hwndTotalMem = GetDlgItem(hwnd, IDC_TOTALMEM) ;
      hwndSUptime  = GetDlgItem(hwnd, IDS_UPTIME) ;
      hwndUptime   = GetDlgItem(hwnd, IDC_UPTIME) ;
      hwndRxBytes  = GetDlgItem(hwnd, IDC_RXBYTES) ;
      hwndTxBytes  = GetDlgItem(hwnd, IDC_TXBYTES) ;
      hwndCpuTime  = GetDlgItem(hwnd, IDC_CPUTIME) ;
      hwndKbdCaps  = GetDlgItem(hwnd, IDC_KBD_CAPS) ;
      hwndKbdNum   = GetDlgItem(hwnd, IDC_KBD_NUM ) ;
      hwndKbdScrl  = GetDlgItem(hwnd, IDC_KBD_SCRL) ;


      //  Subclass the existing key-state controls, 
      //  to allow toggling their states
      {
      WNDPROC pfnOrigProcCaps = (WNDPROC) GetWindowLongIF(hwndKbdCaps, GWx_WNDPROC);
      SetProp(hwndKbdCaps, PROP_KBD_PROC, (HANDLE) pfnOrigProcCaps); //lint !e611
      SetWindowLongIF(hwndKbdCaps, GWx_WNDPROC, (LONG_PTR) (WNDPROC) KbdFlagsProc);

      WNDPROC pfnOrigProcNum = (WNDPROC) GetWindowLongIF(hwndKbdNum, GWx_WNDPROC);
      SetProp(hwndKbdNum, PROP_KBD_PROC, (HANDLE) pfnOrigProcNum); //lint !e611
      SetWindowLongIF(hwndKbdNum, GWx_WNDPROC, (LONG_PTR) (WNDPROC) KbdFlagsProc);

      WNDPROC pfnOrigProcScrl = (WNDPROC) GetWindowLongIF(hwndKbdScrl, GWx_WNDPROC);
      SetProp(hwndKbdScrl, PROP_KBD_PROC, (HANDLE) pfnOrigProcScrl); //lint !e611
      SetWindowLongIF(hwndKbdScrl, GWx_WNDPROC, (LONG_PTR) (WNDPROC) KbdFlagsProc);

      //  try this for uptime field as well
      //  NOTE: this requires SS_NOTIFY set in control definition (derbar.rc)
      WNDPROC pfnOrigProcUptime = (WNDPROC) GetWindowLongIF(hwndUptime, GWx_WNDPROC);
      SetProp(hwndUptime, PROP_KBD_PROC, (HANDLE) pfnOrigProcUptime); //lint !e611
      SetWindowLongIF(hwndUptime, GWx_WNDPROC, (LONG_PTR) (WNDPROC) KbdFlagsProc);
      }

      update_data_fields() ;
      //**********************************************************
      //  do other config tasks *after* creating fields,
      //  so we can display status messages.
      //**********************************************************
      SendMessage(hwnd, WM_SETICON, ICON_BIG,   (LPARAM) LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_MAINICON)));
      SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_MAINICON)));

      load_tray_menu();

      set_window_position(hwnd) ;

      // put the icon into a system tray
      attach_tray_icon(hwnd, szClassName);

      update_uptime_label();
      {
      HWND hToolTip = create_tooltips(hwnd, 150, 100, 10000) ;
      add_tooltips(hwnd, hToolTip, main_tooltips);
      // add_main_tooltips(hwnd, hToolTip) ;
      }
      //  start timer for program update
      //  Note that Windows timers do not actually count 1000 msec per second,
      //  they count 1024 msec per second; thus, they are off by about 1.5%
      // timerID = SetTimer(hwnd, IDT_TIMER, 1000, (TIMERPROC) NULL) ;
      // timerID = SetTimer(hwnd, IDT_TIMER, 977, (TIMERPROC) NULL) ;
      //  Later note: I eventually switched to 1/4-second timing,
      //  to improve responsiveness in data updates.
      timerID = SetTimer(hwnd, IDT_TIMER, 244, (TIMERPROC) NULL) ;
      return TRUE;

   case WM_USER:
      respond_to_tray_clicks(hwnd, lParam);
      break;

   case WM_NCRBUTTONUP:
      respond_to_tray_clicks(hwnd, WM_RBUTTONUP);
      break;

   case WM_SYSCOMMAND:
      // syslog("SYSCOMMAND>: wParam=%04X\n", wParam) ;
      switch (wParam) {
      // case SC_MINIMIZE:
      //    ShowWindow (hwnd, SW_HIDE);
      //    break;
      
      //  detect close from System menu
      // case SC_CLOSE:
      //    DestroyWindow (hwnd);
      //    break;
      
      //  In case you're wondering, NO, I have no idea why I put
      //  the "OR 2" option in here.  Presumably I was seeing these messages
      //  pop up in the winmsgs display, and couldn't find documentation
      //  on precisely what SC_MOVE is supposed to do... (??)
      //  
      //  Later note: Actually, SC_MOVE does not occur on its own,
      //  it is *always* accompanies by 0x0002
      //  This makes it difficult to specifically detect left-click...
      case (SC_MOVE | 2):  //  mouse move
      // case SC_MOVE:
         // syslog("we be moving!!\n") ;
         moving_via_title_bar = true ;
         break;
      
      //  any WM_SYSCOMMAND that we don't handle ourselves,
      //  needs to be passed on to the system.
      //  Otherwise, the window isn't movable!!
      default:
         // syslog("SYSCOMMAND: wParam=%04X\n", wParam) ;
         // return DefWindowProc (hwnd, message, wParam, lParam);
         break;
      }
      break;
   
   case WM_EXITSIZEMOVE:
      if (moving_via_title_bar) {
         save_window_position(hwnd) ;
         moving_via_title_bar = false ;
      }
      return TRUE;
      
   //  technique to drag dialog by its client area.
   //  Once the title bar is enabled, though, the popup menu will no longer work!!
   case WM_NCHITTEST:
      if (!tbar_on  &&  DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam) == HTCLIENT) {
         // SetWindowLong(hwnd, DWL_MSGRESULT, HTCAPTION);
         SetWindowLong(hwnd, 0, HTCAPTION);  //  change for 64-bit build
         return TRUE;
      } 
      break;


#define  WM_DWMCOLORIZATIONCOLORCHANGED   0x320
   //  DerBar: [WM_NCLBUTTONDBLCLK]
   case WM_DWMCOLORIZATIONCOLORCHANGED:
   case WM_NCLBUTTONDBLCLK:
      reset_icon_colors(false);
      break;

   case WM_COMMAND:
      {
      DWORD cmd = HIWORD (wParam) ;
      DWORD target = LOWORD(wParam) ;
      if (show_winmsgs) {
         syslog("WM_COMMAND: cmd: %08X, target: %08X\n", cmd, target);
      }
      // If a button is clicked...
      if (cmd == BN_CLICKED) {
         switch (target) {
         //  Open Network Ifaces listview
         case IDM_IFACES:
            // show_winmsgs ^= 1 ;  //  unthematic
            // show_winmsgs = (show_winmsgs) ? FALSE : TRUE ;
            spawn_ifaces_lview(hwnd) ;
            return TRUE;

         case IDM_TOGGLE_TBAR:
            toggle_title_bar(hwnd) ;
            return TRUE;

         case IDM_RELOCATE:
            relocate_main_dialog(hwnd) ;
            return TRUE;

         case IDM_SET_NOW:
            reset_icon_colors(false);
            return TRUE;

         case IDM_SET_COLOR:   
            reset_icon_colors(true);
            return TRUE;
            
         case IDM_ABOUT:
            CmdAbout(hwnd);
            return TRUE;

         case IDM_OPTIONS:
            open_options_dialog(hwnd);
            return TRUE;

         case IDM_APP_EXIT:
            DestroyWindow(hwnd);
            return TRUE;
         }  //lint !e744
      } 
      }
      break;

   case WM_CTLCOLORSTATIC:
      hdc = (HDC) wParam;
      if ((HWND) lParam == hwndDerBar) {
         SetTextColor(hdc, WIN_BCYAN);
         SetBkColor(hdc, WIN_BLUE);
         if (hbLogo == 0) 
            hbLogo = CreateSolidBrush(WIN_BLUE) ;
         return (LRESULT) hbLogo ;   // hilight colour
      } 
      if ((HWND) lParam == hwndFreeMem  ||
          (HWND) lParam == hwndTotalMem ||
          (HWND) lParam == hwndUptime   ||
          (HWND) lParam == hwndRxBytes  ||
          (HWND) lParam == hwndTxBytes  ||
          (HWND) lParam == hwndCpuTime) {
            
         if (isMemoryLow   &&  (HWND) lParam == hwndFreeMem) {
            SetTextColor(hdc, WIN_WHITE);
            SetBkColor(hdc, WIN_RED);
            hbEdit = CreateSolidBrush(WIN_RED) ;
         } else {
            SetTextColor(hdc, fgnd_edit);
            SetBkColor(hdc, bgnd_edit);
            if (hbEdit == 0) 
               hbEdit = CreateSolidBrush(bgnd_edit) ;
         }
         return (LRESULT) hbEdit ;   // hilight colour
      }
      if ((HWND) lParam == hwndKbdCaps) {
         if (kbd_state & 4) {
            SetTextColor(hdc, WIN_BCYAN);
            SetBkColor(hdc, bgnd_edit);
            if (hbEdit == 0) 
               hbEdit = CreateSolidBrush(bgnd_edit) ;
            return (LRESULT) hbEdit ;   // hilight colour
         }
      } 
      if ((HWND) lParam == hwndKbdNum) {
         if (kbd_state & 2) {
            SetTextColor(hdc, WIN_BCYAN);
            SetBkColor(hdc, bgnd_edit);
            if (hbEdit == 0) 
               hbEdit = CreateSolidBrush(bgnd_edit) ;
            return (LRESULT) hbEdit ;   // hilight colour
         }
      } 
      if ((HWND) lParam == hwndKbdScrl) {
         if (kbd_state & 1) {
            SetTextColor(hdc, WIN_BCYAN);
            SetBkColor(hdc, bgnd_edit);
            if (hbEdit == 0) 
               hbEdit = CreateSolidBrush(bgnd_edit) ;
            return (LRESULT) hbEdit ;   // hilight colour
         }
      } 
      break;

   case WM_TIMER:
      switch (wParam) {
      case IDT_TIMER:
#ifdef  USE_TIMER_TEST
         update_time_count() ;
#endif
         read_system_data() ;
         update_data_fields() ;
         if (!first_pass_done) {
            first_pass_done = true ;
            reset_icon_colors(false);
         }

         return TRUE;
      }  //lint !e744
      break;

   //  We cannot trigger on WM_ERASEBKGND or WM_CTLCOLORDLG,
   //  because we want to hide the main window.
   //  Later: now that I've attached ClearIcon to DerBar,
   //  which is always displayed, the above restrictions do not matter.
   //  Thus, we trigger on WM_SYSCOLORCHANGE instead.
   // 00000002 11:37:16.650   [6380] SWMsg: [WM_SYSCOLORCHANGE]
   // 00000003 11:37:16.650   [6380] SWMsg: [WM_PAINT]   
   // 00000004 11:37:16.650   [6380] SWMsg: [WM_NCPAINT] 
   // 00000005 11:37:16.655   [6380] SWMsg: [WM_ERASEBKGND] 
   // 00000006 11:37:16.655   [6380] SWMsg: [WM_CTLCOLORDLG]   
   // case WM_SYSCOLORCHANGE:
   // 00000826 13:10:04.124   [5108] SWMsg: [WM_DISPLAYCHANGE] 
   // 00000827 13:10:04.125   [4104] 2017-04-07 13:10:10.824 (  87756.863) |    
   //    DEBUG: [UXDriver.ApiX.MessageTranslator] 325@Nvidia::UXDriver::ApiX::MessageTranslator::Translate : 
   //           message(0x7e) wparam(0x20) lParam(0x6400a00): 9, translated(1).   
   // case WM_DISPLAYCHANGE:
   //  Messages rcvd when exiting from Witcher 3
   // 00000042 08:28:48.089   [10444] DBGHELP:  
   // 00000043 08:28:48.108   [5336] DerBar: [WM_PAINT]  
   // 00000044 08:28:48.108   [5336] DerBar: [WM_NCPAINT]   
   // 00000045 08:28:48.108   [5336] DerBar: [WM_ERASEBKGND]   
   // 00000046 08:28:48.108   [5336] DerBar: [WM_CTLCOLORDLG]  
   // Since I'm not receiving WM_SYSCOLORCHANGE in this case,
   // I'll try switching to one of the others instead.
   // 
   // 12/09/17 - From Divinity: Original Sin 2, I receive:
   // 00000071 13:54:33.658   [3692] DerBar: [WM_DISPLAYCHANGE]   
   // 00000072 13:54:34.227   [3692] DerBar: [WM_SYNCPAINT ]   
   // 00000073 13:54:34.229   [3692] DerBar: [WM_NCPAINT]   
   // 00000074 13:54:34.231   [3692] DerBar: [WM_ERASEBKGND]   
   // 00000075 13:54:34.239   [3692] DerBar: [WM_SYNCPAINT ]   
   // 00000076 13:54:34.246   [3692] DerBar: [WM_CTLCOLORDLG]  
   // 00000077 13:54:34.246   [3692] DerBar: [WM_PAINT]  
   //  but trapping WM_PAINT did not work either
   case WM_ERASEBKGND:
      reset_icon_colors(false);
      break;

   //********************************************************************
   //  application shutdown handlers
   //********************************************************************
   case WM_CLOSE:
      DestroyWindow(hwnd);
      return TRUE;

   case WM_DESTROY:
      if (timerID != 0) {
         KillTimer(hwnd, timerID) ;
         timerID = 0 ;
      }
#ifdef   USE_CPU_UTIL
      cpu_usage_release();
#endif
      // remove the icon from a system tray and free .dll handle
      release_systray_resource();
      release_led_images();

      PostQuitMessage(0);
      return TRUE;

   // default:
   //    return DefWindowProc (hwnd, message, wParam, lParam);
   }  //lint !e744
   return FALSE;
}  //lint !e715

//***********************************************************************
static BOOL WeAreAlone(LPSTR szName)
{
   HANDLE hMutex = CreateMutex (NULL, TRUE, szName);
   if (GetLastError() == ERROR_ALREADY_EXISTS)
   {
      CloseHandle(hMutex);
      return FALSE;
   }
   return TRUE;
}

//*********************************************************************
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpszArgument, int nFunsterStil)
{
   if (!WeAreAlone (szClassName)) {
      MessageBox(NULL, "DerBar is already running!!", "collision", MB_OK | MB_ICONEXCLAMATION) ;
      return 0;
   }

   g_hinst = hInstance;
   load_exec_filename() ;  //  get our executable name
   
   //  build one-time network tables
   build_iface_tables() ;
   update_iface_counters() ;
   
   // do this *after* building iface tables
   read_config_file() ;    //  read current screen position, etc

#ifdef   USE_CPU_UTIL
   cpu_usage_setup();
#endif
   load_led_images() ;     //  load our image list

   //  create the main application
   HWND hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC) WndProc);
   if (hwnd == NULL) {
      // Notified your about the failure
      syslog("CreateDialog (main): %s [%u]\n", get_system_message(), GetLastError()) ;
      // Set the return value
      return FALSE;
   }
   ShowWindow (hwnd, SW_SHOW) ;
   UpdateWindow(hwnd);

   MSG msg ;
   while (GetMessage (&msg, NULL, 0, 0)) {
      if (!IsDialogMessage(hwnd, &msg)) {
         TranslateMessage (&msg) ;
         DispatchMessage (&msg) ;
      }
   }
   return (int) msg.wParam ;
}  //lint !e715

