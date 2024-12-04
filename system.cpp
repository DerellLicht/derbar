//**********************************************************************
//  Copyright (c) 2009-2014  Daniel D Miller
//  derbar.exe - Another WinBar application
//  system.cpp: functions for gathering system information
//  
//  Written by:   Daniel D. Miller
//  
//  Last Update:  04/10/09 09:50
//**********************************************************************

//lint -esym(767, _WIN32_WINNT)
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>   //  for sprintf, for %f support
#include <time.h>
#include <iphlpapi.h>
#include <pdh.h>
#ifdef _lint
#include <stdlib.h>  //  atoi()
#endif

#include "common.h"
#include "derbar.h"
#include "ip_iface.h"
#include "PdhMsg.h"

//********************************************************
static uint RxBytes = 0 ;
static uint TxBytes = 0 ;
uint RxBytesPerSec = 0 ;
uint TxBytesPerSec = 0 ;
static DWORD PrevTickCount = 0 ;
DWORD SampleMsec = 0 ;
u64 freemem = 0 ;
u64 min_freemem = 0 ;
u64 totalmem = 0 ;

static ip_iface_entry_p itable_top = NULL ;
static ip_iface_entry_p itable_tail ;

//*******************************************************************
void update_memory_readings(void)
{
   MEMORYSTATUSEX statex;
   statex.dwLength = sizeof (statex);
   GlobalMemoryStatusEx(&statex);

   // DWORDLONG 
   // printf ("%ld percent of memory is in use.\n",
   //         statex.dwMemoryLoad);
   // printf ("There are %*I64d total %sbytes of physical memory.\n",
   //         WIDTH, statex.ullTotalPhys/DIV, divisor);
   // printf ("There are %*I64d free %sbytes of physical memory.\n",
   //         WIDTH, statex.ullAvailPhys/DIV, divisor);
   // printf ("There are %*I64d total %sbytes of paging file.\n",
   //         WIDTH, statex.ullTotalPageFile/DIV, divisor);
   // printf ("There are %*I64d free %sbytes of paging file.\n",
   //         WIDTH, statex.ullAvailPageFile/DIV, divisor);
   // printf ("There are %*I64x total %sbytes of virtual memory.\n",
   //         WIDTH, statex.ullTotalVirtual/DIV, divisor);
   // printf ("There are %*I64x free %sbytes of virtual memory.\n",
   //         WIDTH, statex.ullAvailVirtual/DIV, divisor);
   // 
   // // Show the amount of extended memory available.
   // printf ("There are %*I64x free %sbytes of extended memory.\n",
   //         WIDTH, statex.ullAvailExtendedVirtual/DIV, divisor);
   freemem  = statex.ullAvailPhys ;
   totalmem = statex.ullTotalPhys ;
   if (min_freemem == 0  ||  min_freemem > freemem) {
      min_freemem = freemem ;
   }
}

//***********************************************************************
static int build_IPAddress_table(void)
{
   PMIB_IPADDRTABLE pIpAddrTable ;
   DWORD dwSize = 0;
   int dwRet;

   // Figure out how much memory we need
   dwRet = GetIpAddrTable(NULL, &dwSize, TRUE);
   if (dwRet != ERROR_INSUFFICIENT_BUFFER) {
      return dwRet;
   }
   pIpAddrTable = (PMIB_IPADDRTABLE) new char[dwSize];   //lint !e433 !e826
   if (pIpAddrTable == NULL) {
      return ERROR_NOT_ENOUGH_MEMORY;
   }

   // Actually get the IP address table.
   dwRet = GetIpAddrTable(pIpAddrTable, &dwSize, TRUE);
   if (dwRet != ERROR_SUCCESS) {
      goto error_cleanup ;
   }

   // printf("Number of IP address entries: %u\n", (unsigned) pIpAddrTable->dwNumEntries) ;
   for (DWORD dwLoopIndex = 0; dwLoopIndex < pIpAddrTable->dwNumEntries; dwLoopIndex++) {
      ip_iface_entry_p iptr = new ip_iface_entry_t ;
      memset((char *) iptr, 0, sizeof(ip_iface_entry_t)) ;
      iptr->dwAddr      = pIpAddrTable->table[dwLoopIndex].dwAddr      ;
      iptr->dwIndex     = pIpAddrTable->table[dwLoopIndex].dwIndex     ;
      iptr->dwMask      = pIpAddrTable->table[dwLoopIndex].dwMask      ;
      iptr->dwBCastAddr = pIpAddrTable->table[dwLoopIndex].dwBCastAddr ;
      iptr->dwReasmSize = pIpAddrTable->table[dwLoopIndex].dwReasmSize ;

      if (itable_top == NULL)
         itable_top = iptr ;
      else
         itable_tail->next = iptr ;
      itable_tail = iptr ;

      // syslog("Adding index %u: 0x%08X\n", iptr->dwIndex, iptr->dwAddr) ;
   }
   puts("") ;

error_cleanup:
   if (pIpAddrTable != NULL) 
      delete[] pIpAddrTable ;
   return dwRet;
}

//***********************************************************************
//  WARNING:  Everyone who calls this function, needs to delete
//  the return value when they're done with it!!
//***********************************************************************
static PMIB_IFROW get_iface_entry(DWORD idx)
{
   PMIB_IFROW pIfRow = new MIB_IFROW ;
   ZeroMemory(pIfRow, sizeof(MIB_IFROW));
   pIfRow->dwIndex = idx ;
   DWORD result = GetIfEntry(pIfRow) ;
   if (result == NO_ERROR) 
      return pIfRow ;
   delete pIfRow ;
   return NULL;
}

//***********************************************************************
static void build_iface_table(void)
{
   ip_iface_entry_p iptr ;
   // uint cbox_row = 1 ;
   uint lview_row = 0 ;
   for (iptr = itable_top; iptr != NULL; iptr = iptr->next) {
      // syslog("found index %u: %s\n", iptr->dwIndex, iptr->ipaddr_str) ;
      PMIB_IFROW pIfRow = get_iface_entry(iptr->dwIndex) ;
      if (pIfRow != NULL) {
         ul2uc_t uconv ;
         uconv.ul = iptr->dwAddr ;
         sprintf(iptr->ipaddr_str, "%u.%u.%u.%u", 
           (u8) uconv.uc[0], (u8) uconv.uc[1], (u8) uconv.uc[2], (u8) uconv.uc[3]) ;
         // syslog("found index %u: %s\n", iptr->dwIndex, iptr->ipaddr_str) ;

         // WideCharToMultiByte(CP_ACP, 0, pIfRow->wszName, -1, SomeAsciiStr, MAX_INTERFACE_NAME_LEN, NULL, NULL);
         // printf("Index[%u]:  %s\n", (unsigned) pIfRow->dwIndex, (char *) SomeAsciiStr);
         memcpy(iptr->iface_name, pIfRow->bDescr, pIfRow->dwDescrLen) ;
         iptr->iface_name[pIfRow->dwDescrLen] = 0 ;
         // iptr->cbox_row = cbox_row++ ;
         iptr->lview_row = lview_row++ ;

         delete pIfRow ;
      }
   }
}

//***********************************************************************
static void enable_if_row(char *cfg_str)
{
   uint idx = (unsigned) atoi(cfg_str) ;
   for (ip_iface_entry_p iptr = itable_top; iptr != NULL; iptr = iptr->next) {
      if (iptr->lview_row == idx) {
         iptr->if_active = true ;
         break;
      }
   }
}

//***********************************************************************
static void disable_all_ifaces(void)
{
   for (ip_iface_entry_p iptr = itable_top; iptr != NULL; iptr = iptr->next) 
      iptr->if_active = false ;
}

//***********************************************************************
void update_iface_flags(char *cfg_str)
{
   disable_all_ifaces() ;
   while (LOOP_FOREVER) {
      if (cfg_str == 0)
         break;
      enable_if_row(cfg_str) ;
      char *tl = strchr(cfg_str, ',') ;
      if (tl == NULL)
         break;
      cfg_str = tl+1 ;
   }
}

//***********************************************************************
void write_iface_enables(FILE *fd)
{
   char bfr[81] ;
   int slen = 0 ;
   for (ip_iface_entry_p iptr = itable_top; iptr != NULL; iptr = iptr->next) {
      if (iptr->if_active) {
         if (slen > 0) 
            slen += wsprintf(bfr+slen, ",") ;
         slen += wsprintf(bfr+slen, "%u", iptr->lview_row) ;
      }
   }
   if (slen > 0)
      fprintf(fd, "ip_iface=%s\n", bfr) ;
}

//***********************************************************************
//  this is called once, at program startup
//***********************************************************************
void build_iface_tables(void)
{
   build_IPAddress_table() ;
   build_iface_table() ;
}

//***********************************************************************
//  let listview iterate over interface list,
//  without actually knowing about our internal data
//***********************************************************************
ip_iface_entry_p get_next_ip_iface(ip_iface_entry_p prev)
{
   if (prev == NULL) 
      return itable_top ;
   return prev->next ;
}

//***********************************************************************
void update_iface_counters(void)
{
   uint txb = 0 ;
   uint rxb = 0 ;
   ip_iface_entry_p iptr ;
   for (iptr = itable_top; iptr != NULL; iptr = iptr->next) {
      //  if program is set to any interface other than "Read All",
      //  skip all entries except for the chosen interface.
      // if (ip_iface_idx != 0  &&  ip_iface_idx != iptr->cbox_row)
      //    continue;
      if (!iptr->if_active) 
         continue;

      PMIB_IFROW pIfRow = get_iface_entry(iptr->dwIndex) ;
      if (pIfRow != NULL) {
         
         iptr->dwInOctets  = pIfRow->dwInOctets ;
         iptr->dwOutOctets = pIfRow->dwOutOctets ;
         iptr->dwInErrors  = pIfRow->dwInErrors ;
         iptr->dwOutErrors = pIfRow->dwOutErrors ;

         // syslog("[%u/%u] %s\n", iptr->dwInOctets, iptr->dwOutOctets, iptr->iface_name) ;
         rxb += pIfRow->dwInOctets ;
         txb += pIfRow->dwOutOctets ;

         delete pIfRow ;
      }
   }

   if (PrevTickCount == 0) {
      PrevTickCount = GetTickCount() ;
      SampleMsec = 1000 ;
   } else {
      DWORD CurrTickCount = GetTickCount() ;
      if (CurrTickCount > PrevTickCount) {
         SampleMsec = CurrTickCount - PrevTickCount ;
      } else {
         //  handle counter wrap
         SampleMsec = CurrTickCount + (0xFFFFFFFF - PrevTickCount) + 1 ;
      }
      PrevTickCount = CurrTickCount ;
   }

   if (RxBytes == 0) {
      RxBytes = rxb ;
      RxBytesPerSec = 0 ;
   } else if (rxb < RxBytes) {
      //  try to deal with counter wrap
      RxBytes = rxb ;
      RxBytesPerSec = 0 ;
   } else {
      RxBytesPerSec = rxb - RxBytes ;
      RxBytes = rxb ;
   }
   if (TxBytes == 0) {
      TxBytes = txb ;
      TxBytesPerSec = 0 ;
   } else if (txb < TxBytes) {
      //  try to deal with counter wrap
      TxBytes = txb ;
      TxBytesPerSec = 0 ;
   } else {
      TxBytesPerSec = txb - TxBytes ;
      TxBytes = txb ;
   }
}

//*********************************************************************
static HQUERY hQuery;
static HCOUNTER hCounter;

void cpu_usage_setup(void)
{
   PDH_STATUS presult ;
   presult = PdhOpenQuery(NULL, 0, &hQuery);
   if (presult != ERROR_SUCCESS) {
      syslog("PdhOpenQuery: 0x%08X\n", (uint) presult) ;
      return ;
   }

   presult = PdhAddCounter(hQuery, "\\Processor(_Total)\\% Processor Time", 0, &hCounter);
   if (presult != ERROR_SUCCESS) {
      syslog("PdhAddCounter: 0x%08X\n", (uint) presult) ;
      return ;
   }
}

//*********************************************************************
double cpu_usage_report(void)
{
   PDH_FMT_COUNTERVALUE FmtValue;
   static bool first_query_done = false ;
   PDH_STATUS presult = PdhCollectQueryData(hQuery);
   if (presult != ERROR_SUCCESS) {
      syslog("PdhCollectQueryData1: 0x%08X\n", (uint) presult) ;
      return 0.0 ;
   }
   //  this returns PDH_INVALID_DATA: 
   //  The specified counter does not contain valid data or a successful status code. 
   if (!first_query_done) {
      first_query_done = true ;
      return 0.0 ;
   }
   //*****************************************************************
   // MessageId: PDH_CALC_NEGATIVE_DENOMINATOR
   //
   // MessageText:
   //
   //  A counter with a negative denominator value was detected.
   //*****************************************************************
   //  #define PDH_CALC_NEGATIVE_DENOMINATOR    ((DWORD)0x800007D6L)
   //  [3628] PdhGetFormattedCounterValue: 0x800007D6

   //  https://www.netiq.com/support/kb/doc.php?id=7010545
   //  The return values like PDH_CALC_NEGATIVE_DENOMINATOR can occur because of 
   //  either a bug in the provider or overflow of values in data storage. In these 
   //  situations, you have an option to ignore this return value and retry a bit 
   //  later like 1 second to get the new values. This is not a fatal error that 
   //  application can’t continue correctly, like INVALID_HANDLE or INVALID_DATA. 
   //  
   //  08/20/14 - adding PDH_FMT_NOCAP100 to the function call did NOT solve this 
   //  issue.  BTW, this mostly occurs on WinXP systems, I think.
   //**********************************************************************************
   presult = PdhGetFormattedCounterValue(hCounter, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, NULL, &FmtValue);
   if (presult != ERROR_SUCCESS) {
      if ((uint) presult != PDH_CALC_NEGATIVE_DENOMINATOR)
         syslog("PdhGetFormattedCounterValue: 0x%08X\n", (uint) presult) ;
      return 0.0 ;
   }
   // printf("The cpu usage is : %f%%\n", FmtValue.doubleValue);
   return FmtValue.doubleValue ;
}

//*********************************************************************
void cpu_usage_release(void)
{
   PdhCloseQuery(hQuery);
}

