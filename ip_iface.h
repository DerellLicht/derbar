//**********************************************************************
//  Copyright (c) 2009  Daniel D Miller
//  derbar.exe - Another WinBar application
//  ip_iface.h: IP interface data struct
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

   char  iface_name[MAX_INTERFACE_NAME_LEN+1] ;  //  256 bytes under Vista
   char  ipaddr_str[16] ;
   // uint  cbox_row ;

   //  listview data (will probably supercede cbox_row)
   uint  lview_row ;
   bool  if_active ;
   
} ip_iface_entry_t, *ip_iface_entry_p ;

ip_iface_entry_p get_next_ip_iface(ip_iface_entry_p prev);

