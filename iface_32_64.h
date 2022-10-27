//**********************************************************************
//  Copyright (c) 2009-2022  Daniel D Miller
//  iface_32_64.h - conversion constants for building programs
//  in 32 *or* 64 bit, with minimal problems.
//  
//  Written by:   Daniel D. Miller
//**********************************************************************


//  new definitions for 64-bit builds
#ifdef _WIN64
#define  LONGx             LONG_PTR
#define  UINTx             UINT_PTR
#define  GWx_WNDPROC       GWLP_WNDPROC
#define  GetWindowLongIF   GetWindowLongPtr
#define  SetWindowLongIF   SetWindowLongPtr
#else
#define  LONGx             LONG
#define  UINTx             UINT
#define  GWx_WNDPROC       GWL_WNDPROC
#define  GetWindowLongIF   GetWindowLong
#define  SetWindowLongIF   SetWindowLong
#endif

