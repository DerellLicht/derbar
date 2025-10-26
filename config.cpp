//****************************************************************************
//  Copyright (c) 2009  Daniel D Miller
//  derbar.exe - Another WinBar application
//  config.cpp - manage configuration data file
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
//  Written by:  Dan Miller
//****************************************************************************
//  Filename will be same as executable, but will have .ini extensions.
//  Config file will be stored in same location as executable file.
//  Comments will begin with '#'
//  First line:
//  device_count=%u
//  Subsequent file will have a section for each device.
//****************************************************************************
#include <windows.h>
#include <stdio.h>   //  fopen, etc
#include <stdlib.h>  //  atoi()
#include <tchar.h>

//lint -esym(746, _wfopen, fgetws)
//lint -esym(1055, _wfopen, fgetws)
//lint -e64  Type mismatch (initialization) (struct _iobuf * = int)

#include "common.h"
#include "derbar.h"

//  system.cpp
extern void update_iface_flags(TCHAR *cfg_str);
extern void write_iface_enables(FILE *fd);

//****************************************************************************
static TCHAR ini_name[MAX_PATH_LEN+1] = _T("") ;

uint x_pos = 0 ;
uint y_pos = 200 ;
uint tbar_on = 0 ;
// uint ip_iface_idx = 0 ;  //  0 means read all, otherwise read selected iface

//****************************************************************************
static void strip_comments(TCHAR *bfr)
{
   TCHAR *hd = _tcschr(bfr, _T('#')) ;
   if (hd != 0)
      *hd = 0 ;
}

//****************************************************************************
static LRESULT save_default_ini_file(void)
{
   FILE *fd = _tfopen(ini_name, _T("wt")) ;
   if (fd == 0) {
      LRESULT result = (LRESULT) GetLastError() ;
      syslog(_T("%s open: %s\n"), ini_name, get_system_message(result)) ;
      return result;
   }
   //  save any global vars
   _ftprintf(fd, _T("x_pos=%u\n"), x_pos) ;
   _ftprintf(fd, _T("y_pos=%u\n"), y_pos) ;
   _ftprintf(fd, _T("tbar=%u\n"), tbar_on) ;
   _ftprintf(fd, _T("ontop=%u\n"), keep_on_top) ;
   _ftprintf(fd, _T("login_uptime=%u\n"), use_logon_time_for_uptime) ;
   _ftprintf(fd, _T("uptime_seconds=%u\n"), show_seconds_for_uptime) ;
   _ftprintf(fd, _T("editfg=0x%06X\n"), (uint) fgnd_edit) ;
   _ftprintf(fd, _T("editbg=0x%06X\n"), (uint) bgnd_edit) ;
   _ftprintf(fd, _T("staticfg=0x%06X\n"), (uint) fgnd_static) ;
   _ftprintf(fd, _T("ci_attr=%u\n"), ci_attr) ;
   _ftprintf(fd, _T("debug=%u\n"), (show_winmsgs) ? 1U : 0U) ;
   // _ftprintf(fd, _T("ip_iface=%u\n"), ip_iface_idx) ;
   write_iface_enables(fd) ;
   fclose(fd) ;
   return ERROR_SUCCESS;
}

//****************************************************************************
LRESULT save_cfg_file(void)
{
   return save_default_ini_file() ;
}

//****************************************************************************
//  - derive ini filename from exe filename
//  - attempt to open file.
//  - if file does not exist, create it, with device_count=0
//    no other data.
//  - if file *does* exist, open/read it, create initial configuration
//****************************************************************************
LRESULT read_config_file(void)
{
   TCHAR inpstr[128] ;
   uint uvalue ;
   LRESULT result = derive_filename_from_exec(ini_name, (TCHAR *) _T(".ini")) ;
   if (result != 0)
      return result;

   if (show_winmsgs) {
      syslog(_T("ini file: %s\n"), ini_name);
   }
   FILE *fd = _tfopen(ini_name, _T("rt")) ;
   if (fd == 0) {
      return save_default_ini_file() ;
   }

   while (_fgetts(inpstr, sizeof(inpstr), fd) != 0) {
      strip_comments(inpstr) ;
      strip_newlines(inpstr) ;
      if (_tcslen(inpstr) == 0)
         continue;

      if (_tcsncmp(inpstr, _T("x_pos="), 6) == 0) {
         // syslog("enabling factory mode\n") ;
         x_pos = (uint) _tcstoul(&inpstr[6], 0, 0) ;
      } else
      if (_tcsncmp(inpstr, _T("y_pos="), 6) == 0) {
         y_pos = (uint) _tcstoul(&inpstr[6], 0, 0) ;
      } else
      if (_tcsncmp(inpstr, _T("tbar="), 5) == 0) {
         tbar_on = (uint) _tcstoul(&inpstr[5], 0, 0) ;
      } else
      if (_tcsncmp(inpstr, _T("ontop="), 6) == 0) {
         keep_on_top = (bool) (uint) _tcstoul(&inpstr[6], 0, 0) ;
      } else
      if (_tcsncmp(inpstr, _T("login_uptime="), 13) == 0) {
         use_logon_time_for_uptime = (bool) (uint) _tcstoul(&inpstr[13], 0, 0) ;
      } else
      if (_tcsncmp(inpstr, _T("uptime_seconds="), 15) == 0) {
         show_seconds_for_uptime = (bool) (uint) _tcstoul(&inpstr[15], 0, 0) ;
      } else
      if (_tcsncmp(inpstr, _T("editfg="), 7) == 0) {
         fgnd_edit = (uint) _tcstoul(&inpstr[7], 0, 0) ;
      } else
      if (_tcsncmp(inpstr, _T("editbg="), 7) == 0) {
         bgnd_edit = (uint) _tcstoul(&inpstr[7], 0, 0) ;
      } else
      if (_tcsncmp(inpstr, _T("staticfg="), 9) == 0) {
         fgnd_static = (uint) _tcstoul(&inpstr[9], 0, 0) ;
      } else
      if (_tcsncmp(inpstr, _T("ci_attr="), 8) == 0) {
         ci_attr = (uint) _tcstoul(&inpstr[8], 0, 0) ;
      } else
      if (_tcsncmp(inpstr, _T("debug="), 6) == 0) {
         uvalue = (uint) _tcstoul(&inpstr[6], 0, 0) ;
         show_winmsgs = (uvalue == 0) ? false : true ;
      } else
      if (_tcsncmp(inpstr, _T("ip_iface="), 9) == 0) {
         // ip_iface_idx = (uint) _tcstoul(&inpstr[9], 0, 0) ;
         update_iface_flags(&inpstr[9]) ;
      } else
      {
         syslog(_T("unknown: [%s]\n"), inpstr) ;
      }
   }
   return 0;
}

