//**********************************************************************
//  Copyright (c) 2009-2026  Derell Licht
//  derbar.exe - Another WinBar application
//  ip_iface.h: IP interface data struct
//  
//  This program is licenced under Creative Commons CC0 1.0 Universal
// 
//  The person who associated a work with this deed has dedicated the work to the public domain 
//  by waiving all of his or her rights to the work worldwide under copyright law, including all 
//  related and neighboring rights, to the extent allowed by law.
// 
//  You can copy, modify, distribute and perform the work, even for commercial purposes, 
//  all without asking permission.
// 
//**********************************************************************

//***********************************************************************
typedef struct ip_iface_entry_s {
   struct ip_iface_entry_s *next ;
   DWORD dwAddr;              // IP address
   DWORD dwIndex;             // interface index
   DWORD dwMask;              // subnet mask
   DWORD dwBCastAddr;         // broadcast address 
   DWORD dwReasmSize;         // rassembly size 

   //  interface counters
   DWORD dwInOctets ;
   DWORD dwOutOctets ;
   DWORD dwInErrors ;
   DWORD dwOutErrors ;

   TCHAR  iface_name[MAX_INTERFACE_NAME_LEN+1] ;  //  256 bytes under Vista
   TCHAR  ipaddr_str[16] ;
   // uint  cbox_row ;

   //  listview data (will probably supercede cbox_row)
   uint  lview_row ;
   bool  if_active ;
   
} ip_iface_entry_t, *ip_iface_entry_p ;

ip_iface_entry_p get_next_ip_iface(ip_iface_entry_p prev);

