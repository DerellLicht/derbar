//**********************************************************************
//  Copyright (c) 2021  Daniel D Miller
//  derbar.exe - Another WinBar application
//  login_lsa.cpp: determine most-recent logon time
//  
//  Written by:   Daniel D. Miller
//  
//  Last Update:  06/06/21 17:51
//**********************************************************************
//  build: g++ -Wall -O2 -DSTAND_ALONE=1 login_lsa.cpp -o login_lsa.exe

// #define  STAND_ALONE    1

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <ntsecapi.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>   // required for %"PRIu64" printf artifact

#ifndef STAND_ALONE
#include "common.h"
#include "derbar.h"

#else
typedef  unsigned int         uint ;
typedef  unsigned long long   u64;
#endif

//lint -e10  Expecting '}'

//lint -esym(754, _SECURITY_LOGON_SESSION_DATA::Size, _SECURITY_LOGON_SESSION_DATA::LogonId)
//lint -esym(754, _SECURITY_LOGON_SESSION_DATA::LogonDomain, _SECURITY_LOGON_SESSION_DATA::AuthenticationPackage)
//lint -esym(754, _SECURITY_LOGON_SESSION_DATA::Session, _SECURITY_LOGON_SESSION_DATA::Sid)
//lint -esym(754, _SECURITY_LOGON_SESSION_DATA::LogonServer, _SECURITY_LOGON_SESSION_DATA::DnsDomainName)
//lint -esym(754, _SECURITY_LOGON_SESSION_DATA::Upn, _SECURITY_LOGON_SESSION_DATA::UserName)

//lint -esym(534, wprintf, LsaFreeReturnBuffer, strftime, time)

//lint -esym(1013, LowPart, HighPart)   // not a member of class '_ULARGE_INTEGER'
//lint -esym(40, LowPart, HighPart)
//lint -e63  Expected an lvalue

// ----------------------------------------------------------------------------
// https://fossies.org/linux/gkrellm/src/sysdeps/win32.c
/* Structs and typedefs used to determine the number of logged in users.
 * These should be in ntsecapi.h but are missing in MinGW currently, they
 * are present in the headers provided by mingw-w64.
 * Docs: http://msdn.microsoft.com/en-us/library/aa378290(VS.85).aspx
 */ 
#if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
typedef struct _SECURITY_LOGON_SESSION_DATA
{
   ULONG Size;
   LUID LogonId;
   LSA_UNICODE_STRING UserName;
   LSA_UNICODE_STRING LogonDomain;
   LSA_UNICODE_STRING AuthenticationPackage;
   ULONG LogonType;
   ULONG Session;
   PSID Sid;
   LARGE_INTEGER LogonTime;
   LSA_UNICODE_STRING LogonServer;
   LSA_UNICODE_STRING DnsDomainName;
   LSA_UNICODE_STRING Upn;
} SECURITY_LOGON_SESSION_DATA;

typedef SECURITY_LOGON_SESSION_DATA *  PSECURITY_LOGON_SESSION_DATA;
#endif

// Definitions for function pointers (functions resolved manually at runtime)
typedef NTSTATUS (NTAPI *pfLsaEnumerateLogonSessions)(
        ULONG *LogonSessionCount, LUID **LogonSessionList);
typedef NTSTATUS (NTAPI *pfLsaGetLogonSessionData)(
        LUID *LogonId, SECURITY_LOGON_SESSION_DATA **ppLogonSessionData);
typedef NTSTATUS (NTAPI *pfLsaFreeReturnBuffer)(VOID *Buffer);

// Library handle for secur32.dll, lib is loaded at runtime
static HINSTANCE hSecur32 = NULL;

// Function pointers to various functions from secur32.dll
static pfLsaEnumerateLogonSessions pfLELS = NULL;
static pfLsaFreeReturnBuffer pfLFRB = NULL;
static pfLsaGetLogonSessionData pfLGLSD = NULL;

//*******************************************************************************
// https://www.gamedev.net/forums/topic/565693-converting-filetime-to-time_t-on-windows/
//*******************************************************************************
// static time_t filetime_to_timet(FILETIME const& ft)
// {    
//    ULARGE_INTEGER ull;    
//    ull.LowPart  = ft.dwLowDateTime;    
//    ull.HighPart = ft.dwHighDateTime;    
//    return ull.QuadPart / 10000000ULL - 11644473600ULL;  //lint !e712 !e530
// }

static time_t u64_to_timet(LARGE_INTEGER const& ull)
{    
   // ULARGE_INTEGER ull;    
   // ull.LowPart  = ft.dwLowDateTime;    
   // ull.HighPart = ft.dwHighDateTime;    
   return ull.QuadPart / 10000000ULL - 11644473600ULL;  //lint !e737 !e573 !e712
}

#ifdef STAND_ALONE
//  used by get_dtimes_str()
#define  GET_TIME_LEN   30
#endif

//*************************************************************************
//lint -esym(714, secs_to_date_time_str)
//lint -esym(759, secs_to_date_time_str)
//lint -esym(765, secs_to_date_time_str)
#ifdef STAND_ALONE
static char *secs_to_date_time_str(uint total_seconds, char *dest)
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
      sprintf(dest, "%u:%02u:%02u", hours, mins, secs) ;
      goto exit_point;
   }
   //  print results
   // sprintf(bfr, "%u seconds = %u days (year %u, month %u, day %u), %02u:%02u:%02u\n", 
   //    orig_seconds, orig_days, year, month, days_left, hours, mins, secs) ;
   sprintf(dest, "%u Days, %02u:%02u:%02u", total_seconds, hours, mins, secs) ;
       
exit_point:
   return dest;
}
#endif

//*******************************************************************************
// https://fossies.org/linux/gkrellm/src/sysdeps/win32.c
// create pointers to LSA library functions for 32-bit MinGW
//*******************************************************************************
static bool load_LSA_library_pointers(void)
{
   hSecur32 = LoadLibraryW(L"secur32.dll");
   if (hSecur32 != NULL) {
      pfLELS = (pfLsaEnumerateLogonSessions)GetProcAddress(hSecur32, "LsaEnumerateLogonSessions");
      if (pfLELS == NULL) {
         return false;
      }

      pfLGLSD = (pfLsaGetLogonSessionData)GetProcAddress(hSecur32, "LsaGetLogonSessionData");
      if (pfLGLSD == NULL) {
         return false;
      }

      pfLFRB = (pfLsaFreeReturnBuffer)GetProcAddress(hSecur32, "LsaFreeReturnBuffer");
      if (pfLFRB == NULL) {
         return false;
      }
   }
   else {
      return false;
   }
   return true;   
}

//*******************************************************************************
//lint -esym(765, get_max_logon_time)
time_t get_max_logon_time(time_t curr_time)
{
   static time_t max_logon_time = 0 ;

   DWORD lc = 0;
   DWORD status = 0;
   // PLUID list = nullptr;
   PLUID list = NULL;

   if (max_logon_time == 0) {
      bool bresult = load_LSA_library_pointers();
      if (!bresult) {
#ifdef STAND_ALONE
         printf("Could not load function addresses from secur32.dll\n");
#endif
         return 0;
      }

      //**********************************************************************************************
      //  https://www.codeproject.com/Messages/5809886/Re-Windows-10-find-last-logon-time-in-Cplusplus
      //  Message from Randor, explaining how to obtain last-logon date/time on Windows 10.
      //**********************************************************************************************
      // LsaEnumerateLogonSessions(&lc, &list);
      (*pfLELS)(&lc, &list);  //lint !e534
      for (DWORD i = 0; i < lc; i++) {
         PSECURITY_LOGON_SESSION_DATA pData;
         time_t logon_time ;

         // status = LsaGetLogonSessionData((PLUID)((INT_PTR)list + sizeof(LUID) * i), &pData);
         status = (*pfLGLSD)((PLUID)((INT_PTR)list + sizeof(LUID) * i), &pData); //lint !e732 !e737
         if (0 == status) {
            if (Interactive == pData->LogonType) { //lint !e641
               // FILETIME ft;
               // ft.dwHighDateTime = pData->LogonTime.HighPart;
               // ft.dwLowDateTime  = pData->LogonTime.LowPart;
               // logon_time = filetime_to_timet(ft);
               logon_time = u64_to_timet(pData->LogonTime);

               if (logon_time < curr_time) {
                  if (max_logon_time < logon_time) {
                      max_logon_time = logon_time ;
                  }
               }

               // SYSTEMTIME st_utc, st_local;
               // TIME_ZONE_INFORMATION tzi;
               // GetTimeZoneInformation(&tzi);
               // FileTimeToSystemTime(&ft, &st_utc);
               // SystemTimeToTzSpecificLocalTime(&tzi, &st_utc, &st_local);

#ifdef STAND_ALONE
               wprintf(L"UserName: %s, %"PRIu64" \n", pData->UserName.Buffer, logon_time);   //lint !e707 !e559
#endif
            }

            // LsaFreeReturnBuffer(pData);
            (*pfLFRB)(pData);
         }
      }
   }
   
   return max_logon_time;
}

//*******************************************************************************
time_t get_logon_time(void)
{
   //  look at current time
   _tzset();
   time_t curr_time ;
   time(&curr_time);
#ifdef STAND_ALONE
   printf("current time: %"PRIu64"\n", (u64) curr_time);  //lint !e571
#endif

   //  now look up max logon time
   time_t max_logon_time = get_max_logon_time(curr_time);
   if (max_logon_time == 0) {
      return 0;
   }
      
#ifdef STAND_ALONE
   char ctm[GET_TIME_LEN+1] ;
   struct tm *gtm = localtime(&max_logon_time) ;
   strftime(ctm, GET_TIME_LEN, "%m/%d/%y, %H:%M:%S", gtm) ;
   printf("Last logon: %s [%"PRIu64"]\n", ctm, (u64) max_logon_time) ;  //lint !e571
#endif
   
   time_t delta_time = curr_time - max_logon_time ;
   return delta_time;
}

#ifdef STAND_ALONE
//*******************************************************************************
INT main(void)
{
   time_t delta_time = get_logon_time();
   printf("logon time: %s [%"PRIu64"]\n", secs_to_date_time_str(delta_time, NULL), (u64) delta_time);   //lint !e732 !e571
   return 0;
}
#endif

