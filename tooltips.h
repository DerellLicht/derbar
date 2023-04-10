//****************************************************************************
//  Copyright (c) 2008-2023  Daniel D Miller
//  tooltips.cpp - tooltip functions/data
//
//  Written by:  Daniel D Miller 
//****************************************************************************
//  Usage:
//    HWND hToolTip = create_tooltips(hwnd, 150, 100, 10000) ;
//    add_tooltips(hwnd, hToolTip, name_of_tooltip_array) ;
//****************************************************************************

//  static tooltip-list struct
typedef struct tooltip_data_s {
   uint ControlID ;
   TCHAR *msg ;
} tooltip_data_t ;

//  tooltips.cpp
void create_and_add_tooltips(HWND hwnd, uint max_width, uint popup_msec, uint stayup_msec,
                             tooltip_data_t const * const tooltip_array);

