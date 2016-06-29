//**********************************************************************
//  Copyright (c) 2009  Daniel D Miller
//  derbar.exe - Another WinBar application
//  common_funcs.cpp: commonly-used functions
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
#include <stdio.h>   //  vsprintf
#include <math.h>    //  fabs()
#include <limits.h>
#ifdef _lint
#include <stdlib.h>
#endif
#define  _WIN32_IE   0x0500
#include <commctrl.h>

#include "wcommon.h"

//lint -esym(714, hex_dump, load_exec_filename, lookup_winmsg_code, get_winmsg_name)
//lint -esym(759, hex_dump, load_exec_filename, get_system_message, lookup_winmsg_code, get_winmsg_name)
//lint -esym(765, hex_dump, load_exec_filename, get_system_message, lookup_winmsg_code, get_winmsg_name)

//lint -esym(714, get_hex8, get_hex16, get_base_filename, derive_filename_from_exec)
//lint -esym(759, get_hex8, get_hex16, get_base_filename, derive_filename_from_exec)
//lint -esym(765, get_hex8, get_hex16, get_base_filename, derive_filename_from_exec)

//lint -esym(714, lookup_winmsg_name, add_tooltip_target, next_field, build_font, select_text_file)
//lint -esym(759, lookup_winmsg_name, add_tooltip_target, next_field, build_font, select_text_file)
//lint -esym(765, lookup_winmsg_name, add_tooltip_target, next_field, build_font, select_text_file)

//lint -esym(714, EzCreateFont, strip_newlines, show_error, strip_leading_spaces, bzero)
//lint -esym(759, EzCreateFont, strip_newlines, show_error, strip_leading_spaces, bzero)
//lint -esym(765, EzCreateFont, strip_newlines, show_error, strip_leading_spaces, bzero)

//lint -esym(526, __builtin_va_start)
//lint -esym(628, __builtin_va_start)

static char exec_fname[PATH_MAX+1] = "" ;

//lint -esym(759, derive_filename_from_exec)
//lint -esym(765, derive_filename_from_exec)

static TCHAR const szPalFilter[] = 
   TEXT ("Text Files (*.TXT)\0*.txt\0")  \
   TEXT ("All Files (*.*)\0*.*\0\0") ;

//******************************************************************
bool select_text_file(HWND hDlgWnd, char *command_filename)
{
   OPENFILENAME ofn;       // common dialog box structure
   char szFile[MAX_PATH];       // buffer for file name
   char oldFile[MAX_PATH];       // buffer for file name
   char dirFile[MAX_PATH];       // buffer for file name

   // Initialize OPENFILENAME
   ZeroMemory(&ofn, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hDlgWnd;
   ofn.lpstrFile = szFile;
   //
   // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
   // use the contents of szFile to initialize itself.
   //
   ofn.lpstrFile[0] = '\0';
   lstrcpy(dirFile, command_filename) ;
   char *strptr = strrchr(dirFile, '\\') ;
   if (strptr != 0) {
      strptr++ ;  //  leave the backslash in place
      *strptr = 0 ;  //  strip off filename
      // OutputDebugString(dirFile) ;
   }
   ofn.lpstrInitialDir = dirFile ;
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFilter = szPalFilter ;
   ofn.nFilterIndex = 1;
   ofn.lpstrTitle = "select command file" ;
   ofn.lpstrFileTitle = NULL ;
   ofn.lpstrDefExt = TEXT ("txt") ;
   // ofn.nMaxFileTitle = 0;
   // ofn.lpstrInitialDir = NULL;
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

   // Display the Open dialog box.
   if (GetOpenFileName(&ofn) == TRUE) {
      lstrcpyn(oldFile, command_filename, sizeof(oldFile)) ;
      lstrcpyn(command_filename, ofn.lpstrFile, MAX_PATH) ;

      SetFocus(hDlgWnd) ;
      return true;
   }
   return false;
}

//****************************************************************
unsigned select_color(unsigned curr_attr)
{
   static CHOOSECOLOR cc ;
   static COLORREF    crCustColors[16] ;

   ZeroMemory(&cc, sizeof(cc));
   cc.lStructSize    = sizeof (CHOOSECOLOR) ;
   //  cc.rgbResult      = RGB (0x80, 0x80, 0x80) ;
   cc.rgbResult      = curr_attr ;
   cc.lpCustColors   = crCustColors ;
   cc.Flags          = CC_RGBINIT | CC_FULLOPEN ;

   if (ChooseColor(&cc) == TRUE) {
      return cc.rgbResult ;
   } else {
      return 0 ;
   }
}

//*********************************************************************
void bzero(void *s, size_t n)
{
   memset(s, 0, n) ;
}

//****************************************************************************
void add_tooltip_target(HWND parent, HWND target, HWND hToolTip, char *msg)
{
   TOOLINFO ti;
   // Set the tooltip
   ti.cbSize = sizeof(TOOLINFO);
   ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
   ti.hwnd = parent;
   ti.uId = (UINT) target;
   //ti.lpszText = clocks_data[i].idstring;
   ti.lpszText = msg;
   SendMessage(hToolTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

//*************************************************************************
HFONT build_font(char *fname, unsigned fheight, unsigned fbold, unsigned fitalic, unsigned funderline, unsigned fstrikeout)
{
   HFONT hfont = CreateFont(
         //  46, 28, 215, 0,  //  height, width, escapement, orientation
         fheight, 0, 0, 0,
         (fbold == 0) ? FW_NORMAL : FW_BOLD,
         fitalic,
         funderline,
         fstrikeout,
         DEFAULT_CHARSET, 0, 0, 0, DEFAULT_PITCH, //  other stuff
         fname);
   return hfont;
}

//*************************************************************************
HFONT EzCreateFont(HDC hdc, char * szFaceName, int iDeciPtHeight,
       int iDeciPtWidth, unsigned iAttributes, int textangle, BOOL fLogRes)
{
   FLOAT      cxDpi, cyDpi ;
   HFONT      hFont ;
   LOGFONT    lf ;
   POINT      pt ;
   TEXTMETRIC tm ;

   SaveDC (hdc) ;

   SetGraphicsMode (hdc, GM_ADVANCED) ;
   ModifyWorldTransform (hdc, NULL, MWT_IDENTITY) ;
   SetViewportOrgEx (hdc, 0, 0, NULL) ;
   SetWindowOrgEx   (hdc, 0, 0, NULL) ;

   if (fLogRes) {
      cxDpi = (FLOAT) GetDeviceCaps (hdc, LOGPIXELSX) ;
      cyDpi = (FLOAT) GetDeviceCaps (hdc, LOGPIXELSY) ;
   }
   else {
      cxDpi = (FLOAT) (25.4 * GetDeviceCaps (hdc, HORZRES) /
                              GetDeviceCaps (hdc, HORZSIZE)) ;

      cyDpi = (FLOAT) (25.4 * GetDeviceCaps (hdc, VERTRES) /
                              GetDeviceCaps (hdc, VERTSIZE)) ;
   }

   pt.x = (long) (iDeciPtWidth  * cxDpi / 72) ;
   pt.y = (long) (iDeciPtHeight * cyDpi / 72) ;

   DPtoLP (hdc, &pt, 1) ;

   lf.lfHeight         = - (int) (fabs ((double) pt.y) / 10.0 + 0.5) ;
   lf.lfWidth          = 0 ;
   lf.lfEscapement     = textangle ;
   lf.lfOrientation    = textangle ;
   lf.lfWeight         = iAttributes & EZ_ATTR_BOLD      ? 700 : 0 ;
   lf.lfItalic         = iAttributes & EZ_ATTR_ITALIC    ?   1 : 0 ;
   lf.lfUnderline      = iAttributes & EZ_ATTR_UNDERLINE ?   1 : 0 ;
   lf.lfStrikeOut      = iAttributes & EZ_ATTR_STRIKEOUT ?   1 : 0 ;
   lf.lfCharSet        = 0 ;
   lf.lfOutPrecision   = 0 ;
   lf.lfClipPrecision  = 0 ;
   lf.lfQuality        = 0 ;
   lf.lfPitchAndFamily = 0 ;
   lstrcpy (lf.lfFaceName, szFaceName) ;
   hFont = CreateFontIndirect (&lf) ;

   if (iDeciPtWidth != 0) {
      hFont = (HFONT) SelectObject (hdc, hFont) ;

      GetTextMetrics (hdc, &tm) ;

      DeleteObject (SelectObject (hdc, hFont)) ;

      lf.lfWidth = (int) (tm.tmAveCharWidth *
                          fabs ((double) pt.x) / fabs ((double) pt.y) + 0.5) ;

      hFont = CreateFontIndirect (&lf) ;
   }
   RestoreDC (hdc, -1) ;
   return hFont ;
}

//*************************************************************
LRESULT derive_filename_from_exec(char *drvbfr, char *new_ext)
{
   if (exec_fname[0] == 0) {
      syslog("cannot find name of executable\n") ;
      return ERROR_FILE_NOT_FOUND ;
   }
   lstrcpyn(drvbfr, exec_fname, PATH_MAX) ;
   //  this should never fail; failure would imply
   //  an executable with no .exe extension!
   char *sptr = strrchr(drvbfr, '.') ;
   if (sptr == 0) {
      syslog("%s: no valid extension\n", drvbfr) ;
      return ERROR_BAD_FORMAT;
   }
   //  if no period in new_ext, skip the one in drvbfr
   if (*new_ext != '.')
      sptr++ ;

   lstrcpy(sptr, new_ext) ;
   // syslog("derived [%s]\n", drvbfr) ;
   return 0;
}

//*************************************************************
LRESULT get_base_filename(char *drvbfr)
{
   if (exec_fname[0] == 0) {
      syslog("cannot find name of executable\n") ;
      return ERROR_FILE_NOT_FOUND ;
   }
   lstrcpyn(drvbfr, exec_fname, PATH_MAX) ;
   //  this should never fail; failure would imply
   //  an executable with no .exe extension!
   char *sptr = strrchr(drvbfr, '.') ;
   if (sptr == 0) {
      syslog("%s: no valid extension\n", drvbfr) ;
      return ERROR_BAD_FORMAT;
   }
   *sptr = 0 ; //  strip extension
   return 0;
}

//*************************************************************
DWORD load_exec_filename(void)
{
   //  get fully-qualified name of executable program
   DWORD result = GetModuleFileName(NULL, exec_fname, PATH_MAX) ;
   if (result == 0) {
      exec_fname[0] = 0 ;
      syslog("GetModuleFileName: %s\n", get_system_message()) ;
   }
   // else {
   //    syslog("exe: %s\n", exec_fname) ;
   // }
   return result ;
}

//*************************************************************
//  each subsequent call to this function overwrites
//  the previous report.
//*************************************************************
char *get_system_message(void)
{
   static char msg[261] ;
   // int slen ;

   LPVOID lpMsgBuf;
   FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &lpMsgBuf,
      0, 0);
   // Process any inserts in lpMsgBuf.
   // ...
   // Display the string.
   lstrcpyn(msg, (char *) lpMsgBuf, 260) ;

   // Free the buffer.
   LocalFree( lpMsgBuf );

   //  trim the newline off the message before copying it...
   strip_newlines(msg) ;
   // slen = strlen(msg) ;
   // if (msg[slen-1] == 10  ||  msg[slen-1] == 13) {
   //    msg[slen-1] = 0 ;
   // }

   return msg;
}

//*************************************************************
char *get_system_message(DWORD errcode)
{
   static char msg[261] ;
   // int slen ;
   int result = (int) errcode ;
   if (result < 0) {
      wsprintf(msg, "Win32: unknown error code %d", result) ;
      return msg;
   }

   LPVOID lpMsgBuf;
   FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      errcode,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &lpMsgBuf,
      0, 0);
   // Process any inserts in lpMsgBuf.
   // ...
   // Display the string.
   lstrcpyn(msg, (char *) lpMsgBuf, 260) ;

   // Free the buffer.
   LocalFree( lpMsgBuf );

   //  trim the newline off the message before copying it...
   strip_newlines(msg) ;

   return msg;
}

//********************************************************************
int syslog(const char *fmt, ...)
{
   char consoleBuffer[3000] ;
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   // if (common_logging_enabled)
   //    fprintf(cmlogfd, "%s", consoleBuffer) ;
   OutputDebugString(consoleBuffer) ;
   va_end(al);
   return 1;
}

//**********************************************************************
char *show_error(int error_code)
{
   static char * const message0 = (char *) "no response from ODU" ;
   DWORD ecode = (error_code < 0) ? -error_code : error_code ;
   if (ecode == 0)
      return message0 ;
   else
      return get_system_message(ecode) ;
}

//**********************************************************************
char *next_field(char *q)
{
   while (*q != ' '  &&  *q != 0)
      q++ ; //  walk past non-spaces
   while (*q == ' ')
      q++ ; //  walk past all spaces
   return q;
}

//**********************************************************************
void strip_newlines(char *rstr)
{
   int slen = (int) strlen(rstr) ;
   while (1) {
      if (slen == 0)
         break;
      if (*(rstr+slen-1) == '\n'  ||  *(rstr+slen-1) == '\r') {
         slen-- ;
         *(rstr+slen) = 0 ;
      } else {
         break;
      }
   }
}

//**********************************************************************
char *strip_leading_spaces(char *str)
{
   if (str == 0)
      return 0;
   char *tptr = str ;
   while (1) {
      if (*tptr == 0)
         return tptr;
      if (*tptr != ' '  &&  *tptr != HTAB)
         return tptr;
      tptr++ ;
   }
}

//**********************************************************************
//lint -esym(714, strip_trailing_spaces)
//lint -esym(759, strip_trailing_spaces)
//lint -esym(765, strip_trailing_spaces)
void strip_trailing_spaces(char *rstr)
{
   unsigned slen = strlen(rstr) ;
   while (LOOP_FOREVER) {
      if (slen == 0)
         break;
      slen-- ; //  point to last character
      if (*(rstr+slen) != ' ') 
         break;
      *(rstr+slen) = 0 ;
   }
}

//**********************************************************************
u8 get_hex8(char *ptr)
{
   char hex[3] ;
   hex[0] = *(ptr) ;
   hex[1] = *(ptr+1) ;
   hex[2] = 0 ;
   return (u8) strtoul(hex, 0, 16);
}

//**********************************************************************
u16 get_hex16(char *ptr)
{
   char hex[5] ;
   hex[0] = *(ptr) ;
   hex[1] = *(ptr+1) ;
   hex[2] = *(ptr+2) ;
   hex[3] = *(ptr+3) ;
   hex[4] = 0 ;
   return (u16) strtoul(hex, 0, 16);
}

//**********************************************************************
//  Modify this to build entire string and print once.
//  This command has several forms:
//
//  - the basic form has too many arguments!!
//    bfr - data buffer to display
//    bytes - number of bytes (of bfr) to display
//    addr  - base address to display at beginning of line.
//            This helps with repeated calls to this function.
//    mode  - 0=output to printf, 1=output to syslog
//
//  - The other two forms take only buffer and length args,
//    and implicitly print to either printf or syslog.
//**********************************************************************
//lint -esym(843, high_chars)
static int high_chars = 0 ; //  print using high-ascii chars, not used for now

int hex_dump(u8 *bfr, int bytes, unsigned addr)
{
   int j, len ;
   char tail[40] ;
   char pstr[81] ;

   tail[0] = 0 ;
   int idx = 0 ;
   int plen = 0 ;
   while (1) {
      int leftovers = bytes - idx ;
      if (leftovers > 16)
          leftovers = 16 ;

      plen = wsprintf(pstr, "%05X:  ", addr+idx) ;  //lint !e737
      len = 0 ;
      for (j=0; j<leftovers; j++) {
         u8 chr = bfr[idx+j] ;
         plen += wsprintf(&pstr[plen], "%02X ", chr) ;
         if (chr < 32) {
            len += wsprintf(tail+len, ".") ;
         } else if (chr < 127  ||  high_chars) {
            len += wsprintf(tail+len, "%c", chr) ;
         } else {
            len += wsprintf(tail+len, "?") ;
         }
      }
      //  last, print fill spaces
      for (; j<16; j++) {
         plen += wsprintf(&pstr[plen], "   ") ;
         len += wsprintf(tail+len, " ") ;
      }

      // printf(" | %s |\n", tail) ;
      lstrcat(pstr, " | ") ;
      lstrcat(pstr, tail) ;
      lstrcat(pstr, " |") ;
      // printf("%s\n", pstr) ;
      syslog("%s\n", pstr) ;

      idx += leftovers ;
      if (idx >= bytes)
         break;
   }
   return 0;
}

//**************************************************************************
int hex_dump(u8 *bfr, int bytes)
{
   return hex_dump(bfr, bytes, 0) ;
}

//**************************************************************************
typedef struct win_msgs_s {
   unsigned msg_num ;
   char msg_name[41] ;
} win_msgs_t ;

#define  WM_NOT_FOUND   100000

static win_msgs_t win_msgs[] = {
{ 0, "WM_NULL" },
{ 1, "WM_CREATE" },
{ 2, "WM_DESTROY" },
{ 3, "WM_MOVE" },
{ 5, "WM_SIZE" },
{ 6, "WM_ACTIVATE" },
{ 7, "WM_SETFOCUS" },
{ 8, "WM_KILLFOCUS" },
{ 10, "WM_ENABLE" },
{ 11, "WM_SETREDRAW" },
{ 12, "WM_SETTEXT" },
{ 13, "WM_GETTEXT" },
{ 14, "WM_GETTEXTLENGTH" },
{ 15, "WM_PAINT" },
{ 16, "WM_CLOSE" },
{ 17, "WM_QUERYENDSESSION" },
{ 18, "WM_QUIT" },
{ 19, "WM_QUERYOPEN" },
{ 20, "WM_ERASEBKGND" },
{ 21, "WM_SYSCOLORCHANGE" },
{ 22, "WM_ENDSESSION" },
{ 24, "WM_SHOWWINDOW" },
{ 26, "WM_SETTINGCHANGE" },
{ 26, "WM_WININICHANGE" },
{ 27, "WM_DEVMODECHANGE" },
{ 28, "WM_ACTIVATEAPP" },
{ 29, "WM_FONTCHANGE" },
{ 30, "WM_TIMECHANGE" },
{ 31, "WM_CANCELMODE" },
{ 32, "WM_SETCURSOR" },
{ 33, "WM_MOUSEACTIVATE" },
{ 34, "WM_CHILDACTIVATE" },
{ 35, "WM_QUEUESYNC" },
{ 36, "WM_GETMINMAXINFO" },
{ 38, "WM_PAINTICON" },
{ 39, "WM_ICONERASEBKGND" },
{ 40, "WM_NEXTDLGCTL" },
{ 42, "WM_SPOOLERSTATUS" },
{ 43, "WM_DRAWITEM" },
{ 44, "WM_MEASUREITEM" },
{ 45, "WM_DELETEITEM" },
{ 46, "WM_VKEYTOITEM" },
{ 47, "WM_CHARTOITEM" },
{ 48, "WM_SETFONT" },
{ 49, "WM_GETFONT" },
{ 50, "WM_SETHOTKEY" },
{ 51, "WM_GETHOTKEY" },
{ 55, "WM_QUERYDRAGICON" },
{ 57, "WM_COMPAREITEM" },
{ 61, "WM_GETOBJECT" },
{ 65, "WM_COMPACTING" },
{ 68, "WM_COMMNOTIFY" },
{ 70, "WM_WINDOWPOSCHANGING" },
{ 71, "WM_WINDOWPOSCHANGED" },
{ 72, "WM_POWER" },
{ 74, "WM_COPYDATA" },
{ 75, "WM_CANCELJOURNAL" },
{ 78, "WM_NOTIFY" },
{ 80, "WM_INPUTLANGCHANGEREQUEST" },
{ 81, "WM_INPUTLANGCHANGE" },
{ 82, "WM_TCARD" },
{ 83, "WM_HELP" },
{ 84, "WM_USERCHANGED" },
{ 85, "WM_NOTIFYFORMAT" },
{ 123, "WM_CONTEXTMENU" },
{ 124, "WM_STYLECHANGING" },
{ 125, "WM_STYLECHANGED" },
{ 126, "WM_DISPLAYCHANGE" },
{ 127, "WM_GETICON" },
{ 128, "WM_SETICON" },
{ 129, "WM_NCCREATE" },
{ 130, "WM_NCDESTROY" },
{ 131, "WM_NCCALCSIZE" },
{ 132, "WM_NCHITTEST" },
{ 133, "WM_NCPAINT" },
{ 134, "WM_NCACTIVATE" },
{ 135, "WM_GETDLGCODE" },
{ 136, "WM_SYNCPAINT " },
{ 160, "WM_NCMOUSEMOVE" },
{ 161, "WM_NCLBUTTONDOWN" },
{ 162, "WM_NCLBUTTONUP" },
{ 163, "WM_NCLBUTTONDBLCLK" },
{ 164, "WM_NCRBUTTONDOWN" },
{ 165, "WM_NCRBUTTONUP" },
{ 166, "WM_NCRBUTTONDBLCLK" },
{ 167, "WM_NCMBUTTONDOWN" },
{ 168, "WM_NCMBUTTONUP" },
{ 169, "WM_NCMBUTTONDBLCLK" },
{ 171, "WM_NCXBUTTONDOWN" },
{ 172, "WM_NCXBUTTONUP" },
{ 173, "WM_NCXBUTTONDBLCLK" },
{ 255, "WM_INPUT" },
{ 256, "WM_KEYDOWN" },
{ 256, "WM_KEYFIRST" },
{ 257, "WM_KEYUP" },
{ 258, "WM_CHAR" },
{ 259, "WM_DEADCHAR" },
{ 260, "WM_SYSKEYDOWN" },
{ 261, "WM_SYSKEYUP" },
{ 262, "WM_SYSCHAR" },
{ 263, "WM_SYSDEADCHAR" },
{ 264, "WM_KEYLAST" },
{ 272, "WM_INITDIALOG" },
{ 273, "WM_COMMAND" },
{ 274, "WM_SYSCOMMAND" },
{ 275, "WM_TIMER" },
{ 276, "WM_HSCROLL" },
{ 277, "WM_VSCROLL" },
{ 278, "WM_INITMENU" },
{ 279, "WM_INITMENUPOPUP" },
{ 287, "WM_MENUSELECT" },
{ 288, "WM_MENUCHAR" },
{ 289, "WM_ENTERIDLE" },
{ 290, "WM_MENURBUTTONUP" },
{ 291, "WM_MENUDRAG" },
{ 292, "WM_MENUGETOBJECT" },
{ 293, "WM_UNINITMENUPOPUP" },
{ 294, "WM_MENUCOMMAND" },
{ 295, "WM_CHANGEUISTATE" },
{ 296, "WM_UPDATEUISTATE" },
{ 297, "WM_QUERYUISTATE" },
{ 306, "WM_CTLCOLORMSGBOX" },
{ 307, "WM_CTLCOLOREDIT" },
{ 308, "WM_CTLCOLORLISTBOX" },
{ 309, "WM_CTLCOLORBTN" },
{ 310, "WM_CTLCOLORDLG" },
{ 311, "WM_CTLCOLORSCROLLBAR" },
{ 312, "WM_CTLCOLORSTATIC" },
// { 512, "WM_MOUSEFIRST" },
{ 512, "WM_MOUSEMOVE" },
{ 513, "WM_LBUTTONDOWN" },
{ 514, "WM_LBUTTONUP" },
{ 515, "WM_LBUTTONDBLCLK" },
{ 516, "WM_RBUTTONDOWN" },
{ 517, "WM_RBUTTONUP" },
{ 518, "WM_RBUTTONDBLCLK" },
{ 519, "WM_MBUTTONDOWN" },
{ 520, "WM_MBUTTONUP" },
{ 521, "WM_MBUTTONDBLCLK" },
{ 522, "WM_MOUSEWHEEL" },
{ 523, "WM_XBUTTONDOWN" },
{ 524, "WM_XBUTTONUP" },
{ 525, "WM_MOUSELAST" },
{ 525, "WM_XBUTTONDBLCLK" },
{ 528, "WM_PARENTNOTIFY" },
{ 529, "WM_ENTERMENULOOP" },
{ 530, "WM_EXITMENULOOP" },
{ 531, "WM_NEXTMENU" },
{ 531, "WM_NEXTMENU" },
{ 532, "WM_SIZING" },
{ 533, "WM_CAPTURECHANGED" },
{ 534, "WM_MOVING" },
{ 536, "WM_POWERBROADCAST" },
{ 537, "WM_DEVICECHANGE" },
{ 544, "WM_MDICREATE" },
{ 545, "WM_MDIDESTROY" },
{ 546, "WM_MDIACTIVATE" },
{ 547, "WM_MDIRESTORE" },
{ 548, "WM_MDINEXT" },
{ 549, "WM_MDIMAXIMIZE" },
{ 550, "WM_MDITILE" },
{ 551, "WM_MDICASCADE" },
{ 552, "WM_MDIICONARRANGE" },
{ 553, "WM_MDIGETACTIVE" },
{ 560, "WM_MDISETMENU" },
{ 561, "WM_ENTERSIZEMOVE" },
{ 562, "WM_EXITSIZEMOVE" },
{ 563, "WM_DROPFILES" },
{ 564, "WM_MDIREFRESHMENU" },
{ 673, "WM_MOUSEHOVER  " },
{ 674, "WM_NCMOUSELEAVE" },
{ 675, "WM_MOUSELEAVE  " },
{ 0x0281, "WM_IME_SETCONTEXT" },
{ 0x0282, "WM_IME_NOTIFY" },
{ 0x0283, "WM_IME_CONTROL" },
{ 0x0284, "WM_IME_COMPOSITIONFULL" },
{ 0x0285, "WM_IME_SELECT" },
{ 0x0286, "WM_IME_CHAR" },
{ 0x0288, "WM_IME_REQUEST" },
{ 0x0290, "WM_IME_KEYDOWN" },
{ 0x0291, "WM_IME_KEYUP" },
{ 768, "WM_CUT" },
{ 769, "WM_COPY" },
{ 770, "WM_PASTE" },
{ 771, "WM_CLEAR" },
{ 772, "WM_UNDO" },
{ 773, "WM_RENDERFORMAT" },
{ 774, "WM_RENDERALLFORMATS" },
{ 775, "WM_DESTROYCLIPBOARD" },
{ 776, "WM_DRAWCLIPBOARD" },
{ 777, "WM_PAINTCLIPBOARD" },
{ 778, "WM_VSCROLLCLIPBOARD" },
{ 779, "WM_SIZECLIPBOARD" },
{ 780, "WM_ASKCBFORMATNAME" },
{ 781, "WM_CHANGECBCHAIN" },
{ 782, "WM_HSCROLLCLIPBOARD" },
{ 783, "WM_QUERYNEWPALETTE" },
{ 784, "WM_PALETTEISCHANGING" },
{ 785, "WM_PALETTECHANGED" },
{ 786, "WM_HOTKEY" },
{ 791, "WM_PRINT" },
{ 792, "WM_PRINTCLIENT" },
{ 793, "WM_APPCOMMAND" },
{ 794, "WM_THEMECHANGED" },
{ 856, "WM_HANDHELDFIRST" },
{ 863, "WM_HANDHELDLAST" },
{ 864, "WM_AFXFIRST" },
{ 895, "WM_AFXLAST" },
{ 896, "WM_PENWINFIRST" },
{ 911, "WM_PENWINLAST" },
{ 1024, "DM_GETDEFID" },
{ 1025, "DM_SETDEFID" },
{ 1026, "DM_REPOSITION" },

//  AnaComm-specific messages
{ 1124, "WM_POLL_ADD_ITEM" },
// { 1126, "WM_TX_SEND_RECV_MSG" },
{ 1127, "WM_DO_COMM_TASK" },
{ 1128, "WM_COMM_TASK_DONE" },

//  PSM mesages
{ 1137, "PSM_PRESSBUTTON" },         
{ 1138, "PSM_SETCURSELID" },         
{ 1139, "PSM_SETFINISHTEXTA" },      
{ 1140, "PSM_GETTABCONTROL" },       
{ 1141, "PSM_ISDIALOGMESSAGE" },     
{ 1142, "PSM_GETCURRENTPAGEHWND" },  
{ 1145, "PSM_SETFINISHTEXTW" },      
{ 32768, "WM_APP" },
{ WM_NOT_FOUND, "WM_NOT_FOUND" } };

static win_msgs_t win_neg_msgs[] = {
{ 0, "WM_NULL" },

{ 1,  "NM_OUTOFMEMORY" },
{ 2,  "NM_CLICK" },
{ 3,  "NM_DBLCLK" },          
{ 4,  "NM_RETURN" },          
{ 5,  "NM_RCLICK" },          
{ 6,  "NM_RDBLCLK" },         
{ 7,  "NM_SETFOCUS" },        
{ 8,  "NM_KILLFOCUS" },       
{ 12, "NM_CUSTOMDRAW" },      
{ 13, "NM_HOVER" },           
{ 14, "NM_NCHITTEST" },       
{ 15, "NM_KEYDOWN" },         
{ 16, "NM_RELEASEDCAPTURE" }, 
{ 17, "NM_SETCURSOR" },       
{ 18, "NM_CHAR" },            
{ 19, "NM_TOOLTIPSCREATED" }, 

{ 100, "LVN_ITEMCHANGING" },     
{ 101, "LVN_ITEMCHANGED" },      
{ 102, "LVN_INSERTITEM" },       
{ 103, "LVN_DELETEITEM" },       
{ 104, "LVN_DELETEALLITEMS" },   
{ 105, "LVN_BEGINLABELEDITA" },  
{ 106, "LVN_ENDLABELEDITA" },    
{ 108, "LVN_COLUMNCLICK" },      
{ 109, "LVN_BEGINDRAG" },        
{ 111, "LVN_BEGINRDRAG" },       
{ 113, "LVN_ODCACHEHINT" },      
{ 114, "LVN_ITEMACTIVATE" },     
{ 115, "LVN_ODSTATECHANGED" },   

{ 121, "LVN_HOTTRACK" },

{ 150, "LVN_GETDISPINFOA" },      
{ 175, "LVN_BEGINLABELEDITW" },   
{ 176, "LVN_ENDLABELEDITW" },     
{ 177, "LVN_GETDISPINFOW" },      
{ 151, "LVN_SETDISPINFOA" },      
{ 178, "LVN_SETDISPINFOW" },      
{ 152, "LVN_ODFINDITEMA" },       
{ 179, "LVN_ODFINDITEMW" },       
{ 155, "LVN_KEYDOWN" },           
{ 156, "LVN_MARQUEEBEGIN" },      
{ 157, "LVN_GETINFOTIPA" },       
{ 158, "LVN_GETINFOTIPW" },       

{ 200, "PSN_SETACTIVE" },              
{ 201, "PSN_KILLACTIVE" },             
{ 202, "PSN_APPLY" },                  
{ 203, "PSN_RESET" },                  
{ 205, "PSN_HELP" },                   
{ 206, "PSN_WIZBACK" },                
{ 207, "PSN_WIZNEXT" },                
{ 208, "PSN_WIZFINISH" },              
{ 209, "PSN_QUERYCANCEL" },            
{ 210, "PSN_GETOBJECT" },              
{ 212, "PSN_TRANSLATEACCELERATOR" },   
{ 213, "PSN_QUERYINITIALFOCUS" },      

{ WM_NOT_FOUND, "WM_NOT_FOUND" } };

//*********************************************************************
#define NEG_IND   ((unsigned) 0x80000)

int lookup_winmsg_code(int msg_code)
{
   unsigned j ;

   if (msg_code >= 0) {
      for (j=0; win_msgs[j].msg_num != WM_NOT_FOUND; j++) {
         if (win_msgs[j].msg_num == (unsigned) msg_code)
            return (int) j;
      }
   } else {
      msg_code = -msg_code ;
      for (j=0; win_neg_msgs[j].msg_num != WM_NOT_FOUND; j++) {
         if (win_neg_msgs[j].msg_num == (unsigned) msg_code)
            return (int) (NEG_IND + j);
      }
   }
   return -1;
}

//*********************************************************************
char *get_winmsg_name(unsigned idx)
{
   if (idx & NEG_IND) {
      idx &= ~NEG_IND ;
      return win_neg_msgs[idx].msg_name ;
   } else {
      return win_msgs[idx].msg_name ;
   }
}

//*********************************************************************
char *lookup_winmsg_name(int msg_code)
{
   unsigned j ;
   if (msg_code >= 0) {
      for (j=0; win_msgs[j].msg_num != WM_NOT_FOUND; j++) {
         if (win_msgs[j].msg_num == (unsigned) msg_code)
            return win_msgs[j].msg_name ;
      }
   } else {
      msg_code = -msg_code ;
      for (j=0; win_neg_msgs[j].msg_num != WM_NOT_FOUND; j++) {
         if (win_neg_msgs[j].msg_num == (unsigned) msg_code)
            return win_neg_msgs[j].msg_name ;
      }
   }
   static char msgstr[81] ;
   wsprintf(msgstr, "Msg not found [%d]", msg_code) ;
   return msgstr;
}

