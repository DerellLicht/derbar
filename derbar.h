//**********************************************************************
//  Copyright (c) 2009-2017  Daniel D Miller
//  derbar.exe - Another WinBar application
//  derbar.cpp: main interface functions
//  
//  Written by:   Daniel D. Miller
//**********************************************************************

extern HINSTANCE g_hinst ;

extern HBRUSH hbEdit ;
extern COLORREF fgnd_edit ;
extern COLORREF bgnd_edit ;

//  config.cpp
extern uint x_pos ;
extern uint y_pos ;
extern uint tbar_on ;
extern bool keep_on_top ;
// extern uint ip_iface_idx ;  //  0 means read all, otherwise read selected iface

//  function prototypes
LRESULT save_cfg_file(void);
LRESULT read_config_file(void);

//  ClearIcon.cpp
extern unsigned ci_attr ;
unsigned select_color(COLORREF orig_attr);
void reset_icon_colors(bool my_select_color);

