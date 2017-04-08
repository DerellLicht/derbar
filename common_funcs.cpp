//****************************************************************************
//  Copyright (c) 2008-2017  Daniel D Miller
//  common_funcs.cpp - common functions for console applications.
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//  
//  Collected and Organized by:  Dan Miller
//****************************************************************************

#include <windows.h>
//#define __USE_MINGW_ANSI_STDIO
#include <stdio.h>   //  vsprintf
#include <tchar.h>
#include <time.h>
#include <math.h>    //  fabs()
#include <limits.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#ifdef _lint
#include <stdlib.h>
#endif

#include "common.h"

//lint -esym(526, __builtin_va_start)
//lint -esym(628, __builtin_va_start)

//lint -esym(714, TCR, TLF, TTAB)
//lint -esym(759, TCR, TLF, TTAB)
//lint -esym(765, TCR, TLF, TTAB)
const TCHAR  TCR   =  13 ;
const TCHAR  TLF   =  10 ;
const TCHAR  TTAB  =   9 ;

static char exec_fname[PATH_MAX+1] = "" ;

//******************************************************************
//lint -esym(714, show_bool_str)
//lint -esym(759, show_bool_str)
//lint -esym(765, show_bool_str)
char const * const show_bool_str(bool bool_flag)
{
   return (bool_flag) ? "true" : "false" ;
}

//**************************************************************************
//  we *have* to user our own rand/srand functions, so we'll get
//  the same result on every execution
//**************************************************************************
#define  RAND_MAX2   (0x7FFF)
static unsigned long holdrand = 0 ;

//lint -esym(714, rand2)
//lint -esym(759, rand2)
//lint -esym(765, rand2)
u16 rand2(void)
{
   return((u16) ((holdrand = holdrand * 214013UL + 2531011UL) >> 16) & 0x7FFFUL); 
}

//lint -esym(714, srand2)
//lint -esym(759, srand2)
//lint -esym(765, srand2)
void srand2(unsigned long seed)
{
   holdrand = seed;
}

//**************************************************************************
//  generate random number between 0 and n-1
//**************************************************************************
// In Numerical Recipes in C: The Art of Scientific Computing (William H. 
// Press, Brian P. Flannery, Saul A. Teukolsky, William T. Vetterling;
// New York: Cambridge University Press, 1990 (1st ed, p. 207)),      
// the following comments are made:                                   
//                                                                    
// "If you want to generate a random integer between 1 and 10,        
// you should always do it by                                         
//                                                                    
//    j=1+(int) (10.0*rand()/(RAND_MAX+1.0));                         
//                                                                    
// and never by anything resembling                                   
//                                                                    
//    j=1+((int) (1000000.0*rand()) % 10);                            
//                                                                    
// (which uses lower-order bits)."                                    
//**************************************************************************
//lint -esym(714, random_int)
//lint -esym(759, random_int)
//lint -esym(765, random_int)
unsigned random_int(unsigned n)
{
   //  Note that this *may* overflow if n > 0xFFFF
   return (unsigned) ((unsigned) rand2() * n / (unsigned) (RAND_MAX2 + 1)) ;
}

//****************************************************************************
//lint -esym(714, control_key_pressed)
//lint -esym(759, control_key_pressed)
//lint -esym(765, control_key_pressed)
bool control_key_pressed(void)
{
   if (GetKeyState(VK_CONTROL) & 0x8000)
      return true;
   return false;
}

//****************************************************************************
//  For some reason, on my Vista machine, I cannot access the file
//  using either FindFirstFile() or fopen().
//  It works fine on XP.
//  However, the ancient _stat() works even on Vista, so I'll use that...
//  ... until it also fails, after the third or fourth call.  duh.
//****************************************************************************
#include <sys/stat.h>

//lint -esym(714, file_exists)
//lint -esym(759, file_exists)
//lint -esym(765, file_exists)
bool file_exists(char const * const fefile)
{
   struct _stat st ;
   if (_stat(fefile, &st) == 0)
      return true;
   return false;
}

//lint -esym(714, drive_exists)
//lint -esym(759, drive_exists)
//lint -esym(765, drive_exists)
bool drive_exists(char const * const fefile)
{
   DWORD gld_return = GetLogicalDrives() ;
   char drive_letter = *fefile ;
   drive_letter |= 0x20 ;  //  convert to lower case
   uint drive_mask = 1U << (uint) (drive_letter - 'a') ; //lint !e571
   return (gld_return & drive_mask) ? true : false ;
}

//lint -esym(714, dir_exists)
//lint -esym(759, dir_exists)
//lint -esym(765, dir_exists)
bool dir_exists(char const * const fefile)
{
   if (strlen(fefile) == 2) {
      return drive_exists(fefile) ;
   } else {
      struct _stat st ;
      if (_stat(fefile, &st) == 0) {
         if (st.st_mode & _S_IFDIR)
            return true;
      }
   }
   return false;
}

//*******************************************************************************
static const uint crc_16_table[16] =
{
  0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
  0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400 
};

//lint -esym(714, crc_16)
//lint -esym(759, crc_16)
//lint -esym(765, crc_16)
uint crc_16(uint start, u8 *code_ptr, uint code_size)
{ // This routine does a CRC-16 check on a block of addressable space.
  // code_ptr arg is a pointer to byte, and points to the start of the block.
  // code_size arg is the size of the block in bytes.
  // start arg is the initial value we want to use for CRC, this allows us to 
  // generate a CRC for discontinuous blocks of data through multiple function
  // calls.  In normal usage it should be set to 0.

  uint  r = 0;
  uint  crc = start;
  uint j = 0, k = 0;
  uint segment_size = 0x10000;
  
  // Generate a checksum for that code space...
  while (k<code_size)
  {
      for (j=0; j<segment_size && k<code_size; j++, k++)
      {       
          // compute checksum of lower four bits of data byte
          r = crc_16_table[crc & 0xF];
          crc = (crc >> 4) & 0x0FFF;
          crc = crc ^ r ^ crc_16_table[ code_ptr[j] & 0xF];

          // now compute checksum of upper four bits of data byte
          r = crc_16_table[crc & 0xF];
          crc = (crc >> 4) & 0x0FFF;
          crc = crc ^ r ^ crc_16_table[(code_ptr[j] >> 4) & 0xF];
      }
   
      code_ptr = code_ptr + segment_size;
  }
  
  return crc;
} // End crc_16()

//*****************************************************************************
//lint -esym(714, proc_time)
//lint -esym(759, proc_time)
//lint -esym(765, proc_time)
u64 proc_time(void)
{
   // return (unsigned) clock() ;
   LARGE_INTEGER ti ;
   QueryPerformanceCounter(&ti) ;
   return (u64) ti.QuadPart ;
}

//*************************************************************************
//lint -esym(714, get_clocks_per_second)
//lint -esym(759, get_clocks_per_second)
//lint -esym(765, get_clocks_per_second)
u64 get_clocks_per_second(void)
{
   static u64 clocks_per_sec64 = 0 ;
   if (clocks_per_sec64 == 0) {
      LARGE_INTEGER tfreq ;
      QueryPerformanceFrequency(&tfreq) ;
      clocks_per_sec64 = (u64) tfreq.QuadPart ;
   }
   return clocks_per_sec64 ;
}

//****************************************************************************
//lint -esym(714, calc_elapsed_time)
//lint -esym(759, calc_elapsed_time)
//lint -esym(765, calc_elapsed_time)
uint calc_elapsed_time(bool done)
{
   static u64 ti = 0 ;
   uint secs = 0 ;
   if (!done) {
      ti = proc_time() ;
   } else {
      u64 tf = proc_time() ;
      secs = (uint) ((tf - ti) / get_clocks_per_second()) ;
      // syslog("send_serial_msg: %u seconds", secs) ;
   }
   return secs;
}

//****************************************************************************
//lint -esym(714, calc_elapsed_msec)
//lint -esym(759, calc_elapsed_msec)
//lint -esym(765, calc_elapsed_msec)
uint calc_elapsed_msec(bool done)
{
   static u64 ti = 0 ;
   uint secs = 0 ;
   if (!done) {
      ti = proc_time() ;
   } else {
      u64 tf = proc_time() ;
      secs = (uint) ((tf - ti) / (get_clocks_per_second()/1000)) ;
      // syslog("send_serial_msg: %u seconds", secs) ;
   }
   return secs;
}

//****************************************************************************
//lint -esym(714, swap_rgb)
//lint -esym(759, swap_rgb)
//lint -esym(765, swap_rgb)
uint swap_rgb(uint invalue)
{
   ul2uc_t uconv ;
   uconv.ul = invalue ;
   u8 utemp = uconv.uc[0] ;
   uconv.uc[0] = uconv.uc[2] ;
   uconv.uc[2] = utemp;
   return uconv.ul;
}

//*************************************************************
// static bool file_exists(char *target)
// {
//    HANDLE hdl = CreateFileA(target, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
//       FILE_ATTRIBUTE_NORMAL, NULL) ;
//    if (hdl == INVALID_HANDLE_VALUE) 
//       return false;
// 
//    CloseHandle(hdl) ;
//    return true ;
// }

//*************************************************************************
//  04/23/13  NOTE
//  We *could* use GetLocalTime() here, instead of time() and localtime()
//*************************************************************************

#define  USE_SYSTIME    1
//lint -esym(714, get_dtimes_str)
//lint -esym(759, get_dtimes_str)
//lint -esym(765, get_dtimes_str)
char *get_dtimes_str(char *dest)
{
   static char ctm[GET_TIME_LEN+1] ;

   if (dest == NULL)
       dest = ctm ;

#ifdef  USE_SYSTIME
   SYSTEMTIME  stime ;
   GetLocalTime(&stime) ;

   sprintf(dest,  "%02u/%02u/%02u, %02u:%02u:%02u", 
      stime.wMonth, stime.wDay,    stime.wYear % 100,
      stime.wHour,  stime.wMinute, stime.wSecond) ;
   return dest;
#else
   time_t ttm ;
   struct tm *gtm ;
   size_t slen ;

   time(&ttm) ;
   gtm = localtime(&ttm) ;
   slen = strftime(dest, GET_TIME_LEN, "%m/%d/%y, %H:%M:%S", gtm) ;
   *(dest+slen) = 0 ;   //  strip newline from string
   return dest;
#endif   
}

//*************************************************************************
//lint -esym(714, secs_to_date_time_str)
//lint -esym(759, secs_to_date_time_str)
//lint -esym(765, secs_to_date_time_str)
char *secs_to_date_time_str(uint total_seconds, char *dest)
{
   static char ctm[GET_TIME_LEN+1] ;
   uint secs, mins, hours ;

   if (dest == NULL)
       dest = ctm ;
       
   secs = total_seconds % 60 ;
   total_seconds /= 60 ;
   if (total_seconds == 0) {
      sprintf(dest, "%u", secs) ;
      goto exit_point;
   }
   mins = total_seconds % 60 ;
   total_seconds /= 60 ;
   if (total_seconds == 0) {
      sprintf(dest, "%u:%2u", mins, secs) ;
      goto exit_point;
   }
   hours = total_seconds % 24 ;
   total_seconds /= 24 ;
   if (total_seconds == 0) {
      sprintf(dest, "%u:%2u:%2u", hours, mins, secs) ;
      goto exit_point;
   }
   //  print results
   // sprintf(bfr, "%u seconds = %u days (year %u, month %u, day %u), %02u:%02u:%02u\n", 
   //    orig_seconds, orig_days, year, month, days_left, hours, mins, secs) ;
   sprintf(dest, "%u Days, %02u:%02u:%02u", total_seconds, hours, mins, secs) ;
       
exit_point:
   return dest;
}

//****************************************************************************
//lint -esym(714, get_file_datetime)
//lint -esym(759, get_file_datetime)
//lint -esym(765, get_file_datetime)
bool get_file_datetime(char const * const file_name, SYSTEMTIME *sdt, file_time_select_t time_select)
{
   WIN32_FIND_DATAA fdata ;
   HANDLE fd = FindFirstFileA(file_name, &fdata) ;
   if (fd == INVALID_HANDLE_VALUE) {
      return false;
   }
   FindClose(fd) ;

   FILETIME lft ;
   switch (time_select) {
   case FILE_DATETIME_CREATE:     FileTimeToLocalFileTime(&(fdata.ftCreationTime),   &lft);  break;
   case FILE_DATETIME_LASTACCESS: FileTimeToLocalFileTime(&(fdata.ftLastAccessTime), &lft);  break;
   case FILE_DATETIME_LASTWRITE:  FileTimeToLocalFileTime(&(fdata.ftLastWriteTime),  &lft);  break;
   default:
      return false;
   }
   FileTimeToSystemTime(&lft, sdt) ;
   return true;
}

//****************************************************************************
//  this should be called first, before other functions which use exec_fname
//****************************************************************************
//lint -esym(714, load_exec_filename)
//lint -esym(759, load_exec_filename)
//lint -esym(765, load_exec_filename)
DWORD load_exec_filename(void)
{
   //  get fully-qualified name of executable program
   DWORD result = GetModuleFileNameA(NULL, exec_fname, PATH_MAX) ;
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
//  This appends filename to the base path previous 
//  derived by load_exec_filename()
//*************************************************************
//lint -esym(714, derive_file_path)
//lint -esym(759, derive_file_path)
//lint -esym(765, derive_file_path)
LRESULT derive_file_path(char *drvbfr, char const * const filename)
{
   if (exec_fname[0] == 0) {
      syslog("cannot find name of executable\n") ;
      return ERROR_FILE_NOT_FOUND ;
   }
   strncpy(drvbfr, exec_fname, PATH_MAX) ;
   //  this should never fail; failure would imply
   //  an executable with no .exe extension!
   char *sptr = strrchr(drvbfr, '\\') ;
   if (sptr == 0) {
      syslog("%s: no valid separator\n", drvbfr) ;
      return ERROR_BAD_FORMAT;
   }
   sptr++ ; //  point past the backslash
   strcpy(sptr, filename) ;
   return 0;
}

//*************************************************************
//  returns <exec_path>\\Svr10.<new_ext>
//*************************************************************
//lint -esym(714, derive_filename_from_exec)
//lint -esym(759, derive_filename_from_exec)
//lint -esym(765, derive_filename_from_exec)
LRESULT derive_filename_from_exec(char *drvbfr, char const * const new_ext)
{
   if (exec_fname[0] == 0) {
      syslog("cannot find name of executable\n") ;
      return ERROR_FILE_NOT_FOUND ;
   }
   strncpy(drvbfr, exec_fname, PATH_MAX) ;
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

   strcpy(sptr, new_ext) ;
   // syslog("derived [%s]\n", drvbfr) ;
   return 0;
}

//*************************************************************
//  returns <exec_path>\\Svr10
//*************************************************************
//lint -esym(714, get_base_filename)
//lint -esym(759, get_base_filename)
//lint -esym(765, get_base_filename)
LRESULT get_base_filename(char *drvbfr)
{
   if (exec_fname[0] == 0) {
      syslog("cannot find name of executable\n") ;
      return ERROR_FILE_NOT_FOUND ;
   }
   strncpy(drvbfr, exec_fname, PATH_MAX) ;
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
//  returns <exec_path>\\               .
//*************************************************************
//lint -esym(714, get_base_path)
//lint -esym(759, get_base_path)
//lint -esym(765, get_base_path)
LRESULT get_base_path(char *drvbfr)
{
   if (exec_fname[0] == 0) {
      syslog("cannot find name of executable\n") ;
      return ERROR_FILE_NOT_FOUND ;
   }
   strncpy(drvbfr, exec_fname, PATH_MAX) ;
   //  this should never fail; failure would imply
   //  an executable with no .exe extension!
   char *sptr = strrchr(drvbfr, '\\') ;
   if (sptr == 0) {
      syslog("%s: no valid appname\n", drvbfr) ;
      return ERROR_BAD_FORMAT;
   }
   sptr++ ; //  retain backslash
   *sptr = 0 ; //  strip extension
   return 0;
}

//*************************************************************
//lint -esym(714, get_base_path_wide)
//lint -esym(759, get_base_path_wide)
//lint -esym(765, get_base_path_wide)
LRESULT get_base_path_wide(TCHAR *drvbfr)
{
   //  get fully-qualified name of executable program
   DWORD result = GetModuleFileName(NULL, drvbfr, PATH_MAX) ;
   if (result == 0) {
      *drvbfr = 0 ;
      syslog("GetModuleFileName: %s\n", get_system_message()) ;
   }
   TCHAR *sptr = _tcsrchr(drvbfr, '\\') ;
   if (sptr == 0) {
      syslog("%s: unexpected file format\n", drvbfr) ;
      return ERROR_BAD_FORMAT;
   }
   sptr++ ; //  preserve the backslash
   *sptr = 0 ; //  strip extension
   return 0;
}

/*---------------------------------------------------------
 * IcmpSendEcho() Error Strings
 * 
 * The values in the status word returned in the ICMP Echo 
 *  Reply buffer after calling IcmpSendEcho() all have a
 *  base value of 11000 (IP_STATUS_BASE).  At times,
 *  when IcmpSendEcho() fails outright, GetLastError() will 
 *  subsequently return these error values also.
 *
 * Two Errors value defined in ms_icmp.h are missing from 
 *  this string table (just to simplify use of the table):
 *    "IP_GENERAL_FAILURE (11050)"
 *    "IP_PENDING (11255)"
 * 
 * 07/23/14 - Later on, I found that these codes actually overlap WSA/WinSock error codes.
 * In some cases, I am substituting the WSA messages here...
 */
#define IP_STATUS_BASE 11000
#define MAX_ICMP_ERR_STRING  (IP_STATUS_BASE + 22)
//lint -esym(843, aszSendEchoErr)
static char *aszSendEchoErr[] = {   //lint !e843
   "IP_STATUS_BASE (11000)",
   "WSAHOST_NOT_FOUND (11001)",
   "WSATRY_AGAIN (11002)",
   "IP_DEST_HOST_UNREACHABLE (11003)",
   "IP_DEST_PROT_UNREACHABLE (11004)",
   "IP_DEST_PORT_UNREACHABLE (11005)",
   "IP_NO_RESOURCES (11006)",
   "IP_BAD_OPTION (11007)",
   "IP_HW_ERROR (11008)",
   "IP_PACKET_TOO_BIG (11009)",
   "IP_REQ_TIMED_OUT (11010)",
   "IP_BAD_REQ (11011)",
   "IP_BAD_ROUTE (11012)",
   "IP_TTL_EXPIRED_TRANSIT (11013)",
   "IP_TTL_EXPIRED_REASSEM (11014)",
   "IP_PARAM_PROBLEM (11015)",
   "IP_SOURCE_QUENCH (11016)",
   "IP_OPTION_TOO_BIG (11017)",
   "IP_BAD_DESTINATION (11018)",
   "IP_ADDR_DELETED (11019)",
   "IP_SPEC_MTU_CHANGE (11020)",
   "IP_MTU_CHANGE (11021)",
   "IP_UNLOAD (11022)"
};

//*************************************************************
//  we also need a special handler for SNMP error codes
//*************************************************************
// #define SNMP_MGMTAPI_TIMEOUT  40
// #define SNMP_MGMTAPI_SELECT_FDERRORS   41
// #define SNMP_MGMTAPI_TRAP_ERRORS 42
// #define SNMP_MGMTAPI_TRAP_DUPINIT   43
// #define SNMP_MGMTAPI_NOTRAPS  44
// #define SNMP_MGMTAPI_AGAIN 45
// #define SNMP_MGMTAPI_INVALID_CTL 46
// #define SNMP_MGMTAPI_INVALID_SESSION   47
// #define SNMP_MGMTAPI_INVALID_BUFFER 48

#define  SNMP_ERROR_FIRST  40
#define  SNMP_ERROR_LAST   48
static char * const snmp_error_msgs[9] = {
"SNMP No Response Received",
"SNMP SELECT_FDERRORS",
"SNMP TRAP_ERRORS",
"SNMP TRAP_DUPINIT",
"SNMP NOTRAPS",
"SNMP AGAIN",
"SNMP INVALID_CTL",
"SNMP INVALID_SESSION",
"SNMP INVALID_BUFFER"
} ;

//*************************************************************************************
//  each subsequent call to this function overwrites
//  the previous report.
//*************************************************************************************
// http://stackoverflow.com/questions/6264449/formatmessage-fails-with-error-code-317
// Using the error code lookup tool from Visual Studio, 317 is described as
// 
//     The system cannot find message text for message number 0x%1 in the message file for %2.
// 
// This means that a message description does not exist in the system for the previous error code. 
// The MSDN documentation for FormatMessage with flag FORMAT_MESSAGE_FROM_SYSTEM states that:
// 
// Not all Windows Embedded CE–based devices will contain the system message-table resources. 
// This is a selectable part of the Windows Embedded CE operating system and is often removed to conserve space.
//*************************************************************************************

//*************************************************************
//lint -esym(714, get_system_message)
//lint -esym(759, get_system_message)
//lint -esym(765, get_system_message)
char *get_system_message(DWORD errcode)
{
   // static char msg[261] ;
   static char msg[1024] ;
   // int slen ;
   int result = (int) errcode ;
   if (result < 0) {
      result = -result ;
      errcode = (DWORD) result ;
      // wsprintfA(msg, "Win32: unknown error code %d", result) ;
      // return msg;
   }
   if (errcode >= IP_STATUS_BASE  &&  errcode <= MAX_ICMP_ERR_STRING) {
      return aszSendEchoErr[errcode - IP_STATUS_BASE];
   }
   if (errcode >= SNMP_ERROR_FIRST  &&  errcode <= SNMP_ERROR_LAST) {
      return snmp_error_msgs[errcode - SNMP_ERROR_FIRST];
   }

   LPVOID lpMsgBuf;
   DWORD dresult = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      errcode,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPSTR) &lpMsgBuf,
      0, 0);
   // Process any inserts in lpMsgBuf.
   // ...
   // Display the string.
   if (dresult == 0) {
      DWORD glError = GetLastError() ;
      if (glError == 317) {   //  see comment at start of function
         sprintf(msg, "FormatMessage(): no message for error code %d", result) ;
      } else {
         sprintf(msg, "FormatMessage() failed: [%u], errcode %d", (uint) GetLastError(), result) ;
      }
      
   } else
   if (lpMsgBuf == NULL) {
      sprintf(msg, "NULL buffer in response from FormatMessage() [%u]", (uint) GetLastError()) ;
   } else 
   {
      strncpy(msg, (char *) lpMsgBuf, sizeof(msg)) ;
      // Free the buffer.
      LocalFree( lpMsgBuf );
   }

   //  trim the newline off the message before copying it...
   strip_newlines(msg) ;

   return msg;
}

//*************************************************************
//  each subsequent call to this function overwrites
//  the previous report.
//*************************************************************
char *get_system_message(void)
{
   return get_system_message(GetLastError());
}

//********************************************************************
//  On Windows platform, try to redefine printf/fprintf
//  so we can output code to a debug window.
//  Also, shadow syslog() within OutputDebugStringA()
//  Note: printf() remapping was unreliable,
//  but syslog worked great.
//********************************************************************
//lint -esym(714, syslog)
//lint -esym(759, syslog)
//lint -esym(765, syslog)
int syslog(const char *fmt, ...)
{
   char consoleBuffer[3000] ;
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   // if (common_logging_enabled)
   //    fprintf(cmlogfd, "%s", consoleBuffer) ;
   OutputDebugStringA(consoleBuffer) ;
   va_end(al);
   return 1;
}

//********************************************************************
//  On Windows platform, try to redefine printf/fprintf
//  so we can output code to a debug window.
//  Also, shadow syslog() within OutputDebugStringA()
//  Note: printf() remapping was unreliable,
//  but syslog worked great.
//********************************************************************
//lint -esym(714, syslogW)
//lint -esym(759, syslogW)
//lint -esym(765, syslogW)
int syslogW(const TCHAR *fmt, ...)
{
   TCHAR consoleBuffer[3000] ;
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530 !e516
   _vstprintf(consoleBuffer, fmt, al);   //lint !e64
   // if (common_logging_enabled)
   //    fprintf(cmlogfd, "%s", consoleBuffer) ;
   OutputDebugString(consoleBuffer) ;
   va_end(al);
   return 1;
}

//**********************************************************************
//lint -esym(714, show_error)
//lint -esym(759, show_error)
//lint -esym(765, show_error)
char *show_error(int error_code)
{
   static char *message0 = "no response from ODU" ;
   uint ecode = (uint) (error_code < 0) ? -error_code : error_code ; //lint !e732
   if (ecode == 0)
      return message0 ;
   else
      return get_system_message(ecode) ;
}  //lint !e843

//**********************************************************************
//lint -esym(714, IsCharNum)
//lint -esym(759, IsCharNum)
//lint -esym(765, IsCharNum)
bool IsCharNum(char inchr)
{
   // if (inchr >= '0'  &&  inchr <= '9')
   if (inchr >= '0'  &&  inchr <= '9')
      return true ;
   return false;
}

//**********************************************************************
//lint -esym(714, next_field)
//lint -esym(759, next_field)
//lint -esym(765, next_field)
char *next_field(char *q)
{
   while (*q != ' '  &&  *q != HTAB  &&  *q != 0)
      q++ ; //  walk past non-spaces
   while (*q == ' '  ||  *q == HTAB)
      q++ ; //  walk past all spaces
   return q;
}

//********************************************************************
//  this function searches input string for CR/LF chars.
//  If any are found, it will replace ALL CR/LF with 0,
//  then return pointer to next non-CR/LF char.
//  If NO CR/LF are found, it returns NULL
//********************************************************************
//lint -esym(714, find_newlines)
//lint -esym(759, find_newlines)
//lint -esym(765, find_newlines)
char *find_newlines(char *hd)
{
   char *tl = hd ;
   while (1) {
      if (*tl == 0)
         return 0;
      if (*tl == CR  ||  *tl == LF) {
         while (*tl == CR  ||  *tl == LF) 
            *tl++ = 0 ;
         return tl;
      }
      tl++ ;
   }
}

//**********************************************************************
//lint -esym(714, strip_newlines)
//lint -esym(759, strip_newlines)
//lint -esym(765, strip_newlines)
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
//lint -esym(714, strip_leading_spaces)
//lint -esym(759, strip_leading_spaces)
//lint -esym(765, strip_leading_spaces)
char *strip_leading_spaces(char *str)
{
   if (str == 0)
      return 0;
   char *tptr = str ;
   while (LOOP_FOREVER) {
      if (*tptr == 0)
         return tptr;
      if (*tptr != SPC  &&  *tptr != HTAB)
         return tptr;
      tptr++ ;
   }
}

//**********************************************************************
//lint -esym(714, strip_leading_zeros)
//lint -esym(759, strip_leading_zeros)
//lint -esym(765, strip_leading_zeros)
//  00081
//  00000
void strip_leading_zeros(char *str)
{
   if (str == 0)
      return ;
   char *tptr = str ;
   while (LOOP_FOREVER) {
      if (*tptr == 0)
         break;
      if (*tptr != ZERO)
         break;
      tptr++ ;
   }
   if (tptr != str)
      strcpy(str, tptr) ;
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
//lint -esym(714, get_hex8)
//lint -esym(759, get_hex8)
//lint -esym(765, get_hex8)
u8 get_hex8(char const * const ptr)
{
   char hex[3] ;
   hex[0] = *(ptr) ;
   hex[1] = *(ptr+1) ;
   hex[2] = 0 ;
   return (u8) strtoul(hex, 0, 16);
}

//**********************************************************************
//lint -esym(714, get_hex16)
//lint -esym(759, get_hex16)
//lint -esym(765, get_hex16)
u16 get_hex16(char const * const ptr)
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
// :08000003ECC6030024CE00004E
//lint -esym(714, get_hex32)
//lint -esym(759, get_hex32)
//lint -esym(765, get_hex32)
u32 get_hex32(char const * const ptr)
{
   char hex[9] ;
   hex[0] = *(ptr) ;
   hex[1] = *(ptr+1) ;
   hex[2] = *(ptr+2) ;
   hex[3] = *(ptr+3) ;
   hex[4] = *(ptr+4) ;
   hex[5] = *(ptr+5) ;
   hex[6] = *(ptr+6) ;
   hex[7] = *(ptr+7) ;
   hex[8] = 0 ;
   return (u32) strtoul(hex, 0, 16);
}

//**********************************************************
//lint -esym(714, uabs)
//lint -esym(759, uabs)
//lint -esym(765, uabs)
uint uabs(uint uvalue1, uint uvalue2)
{
   return (uvalue1 > uvalue2)
        ? (uvalue1 - uvalue2)
        : (uvalue2 - uvalue1) ;
}

//**********************************************************
//lint -esym(714, dabs)
//lint -esym(759, dabs)
//lint -esym(765, dabs)
double dabs(double dvalue1, double dvalue2)
{
   return (dvalue1 > dvalue2)
        ? (dvalue1 - dvalue2)
        : (dvalue2 - dvalue1) ;
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
static const int high_chars = 0 ; //  print using high-ascii chars, not used for now

//lint -esym(714, hex_dump)
//lint -esym(759, hex_dump)
//lint -esym(765, hex_dump)
int hex_dump(u8 const * const bfr, int bytes, unsigned addr)
{
   int j, len ;
   char tail[40] ;
   char pstr[81] ;
   static bool hex_dump_active = false ;
   if (hex_dump_active)
      return 0;
   hex_dump_active = true ;

   tail[0] = 0 ;
   int idx = 0 ;
   int plen = 0 ;
   while (1) {
      int leftovers = bytes - idx ;
      if (leftovers > 16)
          leftovers = 16 ;

      plen = wsprintfA(pstr, "%05X:  ", addr+idx) ;  //lint !e737
      len = 0 ;
      for (j=0; j<leftovers; j++) {
         u8 chr = bfr[idx+j] ;
         plen += wsprintfA(&pstr[plen], "%02X ", chr) ;
         if (chr < 32) {
            len += wsprintfA(tail+len, ".") ;
         } else if (chr < 127  ||  high_chars) {   //lint !e506 !e774
            len += wsprintfA(tail+len, "%c", chr) ;
         } else {
            len += wsprintfA(tail+len, "?") ;
         }
      }
      //  last, print fill spaces
      for (; j<16; j++) {
         plen += wsprintfA(&pstr[plen], "   ") ;
         len += wsprintfA(tail+len, " ") ;
      }

      // printf(" | %s |\n", tail) ;
      strcat(pstr, " | ") ;
      strcat(pstr, tail) ;
      strcat(pstr, " |") ;
      // printf("%s\n", pstr) ;
      syslog("%s\n", pstr) ;

      idx += leftovers ;
      if (idx >= bytes)
         break;
   }
   hex_dump_active = false ;
   return 0;
}

//**************************************************************************
int hex_dump(u8 const * const bfr, int bytes)
{
   return hex_dump(bfr, bytes, 0) ;
}

//**************************************************************************
//  Now *here* is a function which should have been available in the 
//  crtl a *long* time ago, but it never was.
//  This function returns number of lines copied,
//  or (-errno) on error
//**************************************************************************
//lint -esym(714, file_copy_by_line)
//lint -esym(759, file_copy_by_line)
//lint -esym(765, file_copy_by_line)
int file_copy_by_line(char *source_file, char *dest_file)
{
   FILE *infile = fopen(source_file, "rt") ;
   if (infile == NULL) {
      syslog("%s: %s\n", source_file, strerror(errno)) ;
      return -errno;
   }
   FILE *outfile = fopen(dest_file, "wt") ;
   if (outfile == NULL) {
      syslog("%s: %s\n", dest_file, strerror(errno)) ;
      return -errno;
   }
   char inpstr[260] ;
   int line_count = 0 ;
   while (fgets(inpstr, sizeof(inpstr), infile) != 0) {
      fputs(inpstr, outfile) ;
      line_count++ ;
   }
   fclose(infile) ;
   fclose(outfile) ;
   return line_count;
}

//*****************************************************************************
// ULLONG_MAX = 18,446,744,073,709,551,615
//*****************************************************************************
//lint -esym(714, convert_to_commas)
//lint -esym(759, convert_to_commas)
//lint -esym(765, convert_to_commas)
char *convert_to_commas(ULONGLONG uli, char *outstr)
{  //lint !e1066
   int slen, inIdx, j ;
   char *strptr ;
   char temp_ull_str[MAX_ULL_COMMA_LEN+1] ;
   static char local_ull_str[MAX_ULL_COMMA_LEN+1] ;
   if (outstr == NULL)
       outstr = local_ull_str ;

   //sprintf(temp_ull_str, "%llu", uli);
   sprintf(temp_ull_str, "%"PRIu64"", uli);
   //sprintf(temp_ull_str, "%I64u", uli);
   // _ui64toa(uli, temp_ull_str, 10) ;
   slen = (int) strlen(temp_ull_str) ;
   inIdx = --slen ;//  convert byte-count to string index 

   //  put NULL at end of output string
   strptr = outstr + MAX_ULL_COMMA_LEN ;
   *strptr-- = 0 ;   //  make sure there's a NULL-terminator

   for (j=0; j<slen; j++) {
      *strptr-- = temp_ull_str[inIdx--] ;
      if ((j+1) % 3 == 0)
         *strptr-- = ',' ;
   }
   *strptr = temp_ull_str[inIdx] ;

   //  copy string from tail-aligned to head-aligned
   strcpy(outstr, strptr) ;
   return outstr ;
}


