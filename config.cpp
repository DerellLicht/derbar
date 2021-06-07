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
#include <limits.h>  //  PATH_MAX

#include "common.h"
#include "derbar.h"

//  system.cpp
extern void update_iface_flags(char *cfg_str);
extern void write_iface_enables(FILE *fd);

//****************************************************************************
static char ini_name[PATH_MAX+1] = "" ;

uint x_pos = 0 ;
uint y_pos = 200 ;
uint tbar_on = 0 ;
// uint ip_iface_idx = 0 ;  //  0 means read all, otherwise read selected iface

//****************************************************************************
static void strip_comments(char *bfr)
{
   char *hd = strchr(bfr, '#') ;
   if (hd != 0)
      *hd = 0 ;
}

//****************************************************************************
static LRESULT save_default_ini_file(void)
{
   FILE *fd = fopen(ini_name, "wt") ;
   if (fd == 0) {
      LRESULT result = (LRESULT) GetLastError() ;
      syslog("%s open: %s\n", ini_name, get_system_message(result)) ;
      return result;
   }
   //  save any global vars
   fprintf(fd, "x_pos=%u\n", x_pos) ;
   fprintf(fd, "y_pos=%u\n", y_pos) ;
   fprintf(fd, "tbar=%u\n", tbar_on) ;
   fprintf(fd, "ontop=%u\n", keep_on_top) ;
   fprintf(fd, "login_uptime=%u\n", use_logon_time_for_uptime) ;
   fprintf(fd, "editfg=0x%06X\n", (uint) fgnd_edit) ;
   fprintf(fd, "editbg=0x%06X\n", (uint) bgnd_edit) ;
   fprintf(fd, "ci_attr=%u\n", ci_attr) ;
   fprintf(fd, "debug=%u\n", (show_winmsgs) ? 1U : 0U) ;
   // fprintf(fd, "ip_iface=%u\n", ip_iface_idx) ;
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
   char inpstr[128] ;
   uint uvalue ;
   LRESULT result = derive_filename_from_exec(ini_name, (char *) ".ini") ;
   if (result != 0)
      return result;

   if (show_winmsgs) {
      syslog("ini file: %s\n", ini_name);
   }
   FILE *fd = fopen(ini_name, "rt") ;
   if (fd == 0) {
      return save_default_ini_file() ;
   }

   while (fgets(inpstr, sizeof(inpstr), fd) != 0) {
      strip_comments(inpstr) ;
      strip_newlines(inpstr) ;
      if (strlen(inpstr) == 0)
         continue;

      if (strncmp(inpstr, "x_pos=", 6) == 0) {
         // syslog("enabling factory mode\n") ;
         x_pos = (uint) strtoul(&inpstr[6], 0, 0) ;
      } else
      if (strncmp(inpstr, "y_pos=", 6) == 0) {
         // syslog("enabling factory mode\n") ;
         y_pos = (uint) strtoul(&inpstr[6], 0, 0) ;
      } else
      if (strncmp(inpstr, "tbar=", 5) == 0) {
         // syslog("enabling factory mode\n") ;
         tbar_on = (uint) strtoul(&inpstr[5], 0, 0) ;
         // tbar_on = 1 ;
      } else
      if (strncmp(inpstr, "ontop=", 6) == 0) {
         // syslog("enabling factory mode\n") ;
         keep_on_top = (bool) (uint) strtoul(&inpstr[6], 0, 0) ;
      } else
      if (strncmp(inpstr, "login_uptime=", 13) == 0) {
         // syslog("enabling factory mode\n") ;
         use_logon_time_for_uptime = (bool) (uint) strtoul(&inpstr[13], 0, 0) ;
      } else
      if (strncmp(inpstr, "editfg=", 7) == 0) {
         fgnd_edit = (uint) strtoul(&inpstr[7], 0, 0) ;
      } else
      if (strncmp(inpstr, "editbg=", 7) == 0) {
         bgnd_edit = (uint) strtoul(&inpstr[7], 0, 0) ;
      } else
      if (strncmp(inpstr, "ci_attr=", 8) == 0) {
         // syslog("enabling factory mode\n") ;
         ci_attr = (uint) strtoul(&inpstr[8], 0, 0) ;
      } else
      if (strncmp(inpstr, "debug=", 6) == 0) {
         uvalue = (uint) strtoul(&inpstr[6], 0, 0) ;
         show_winmsgs = (uvalue == 0) ? false : true ;
      } else
      if (strncmp(inpstr, "ip_iface=", 9) == 0) {
         // ip_iface_idx = (uint) strtoul(&inpstr[9], 0, 0) ;
         update_iface_flags(&inpstr[9]) ;
      } else
      {
         syslog("unknown: [%s]\n", inpstr) ;
      }
   }
   return 0;
}

