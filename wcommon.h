//*****************************************************************
//  extract common code for standard derelict Windows operations.
//  
//  - Find INI filename
//  - Create addon objects for Windows applications
//  
//  Written by:   Daniel D. Miller
//  
//  Last Update:  07/11/03 16:29
//*****************************************************************

typedef unsigned char      u8 ;
typedef unsigned short     u16 ;
typedef unsigned int       uint ;
typedef unsigned long long u64 ;

#define  STX   2
#define  ETX   3
#define  HTAB  9
#define  CR    13
#define  LF    10
#define  SPC   32

#define  LOOP_FOREVER   1

//***********************************************************************
typedef union ul2uc_u {
   uint  ul ;
   u8 uc[4] ;
} ul2uc_t ;

//  Windows standard COLORREF values
#define  WIN_BLACK      (0x00000000U)
#define  WIN_BLUE       (0x00AA0000U)
#define  WIN_GREEN      (0x0000AA00U)
#define  WIN_CYAN       (0x00AAAA00U)
#define  WIN_RED        (0x000000AAU)
#define  WIN_MAGENTA    (0x00AA00AAU)
#define  WIN_BROWN      (0x000055AAU)
#define  WIN_WHITE      (0x00AAAAAAU)
#define  WIN_GREY       (0x00555555U)
#define  WIN_BBLUE      (0x00FF5555U)
#define  WIN_BGREEN     (0x0055FF55U)
#define  WIN_BCYAN      (0x00FFFF55U)
#define  WIN_BRED       (0x005555FFU)
#define  WIN_BMAGENTA   (0x00FF55FFU)
#define  WIN_YELLOW     (0x0055FFFFU)
#define  WIN_BWHITE     (0x00FFFFFFU)

//***************************************************************
#define EZ_ATTR_NORMAL        0
#define EZ_ATTR_BOLD          1
#define EZ_ATTR_ITALIC        2
#define EZ_ATTR_UNDERLINE     4
#define EZ_ATTR_STRIKEOUT     8

//  font-management functions
HFONT build_font(char *fname, unsigned fheight, unsigned fbold, unsigned fitalic, 
      unsigned funderline, unsigned fstrikeout);
HFONT EzCreateFont(HDC hdc, char * szFaceName, int iDeciPtHeight,
      int iDeciPtWidth, unsigned iAttributes, int textangle, BOOL fLogRes);

//  common_funcs.cpp
char *get_system_message(void);
char *get_system_message(DWORD errcode);
void bzero(void *s, size_t n);
int syslog(const char *fmt, ...);
char *show_error(int error_code);
char *next_field(char *q);
void strip_newlines(char *rstr);
char *strip_leading_spaces(char *str);
u8 get_hex8(char *ptr);
u16 get_hex16(char *ptr);
int hex_dump(u8 *bfr, int bytes, unsigned addr);
int hex_dump(u8 *bfr, int bytes);
int lookup_winmsg_code(int msg_code);
char *get_winmsg_name(unsigned idx);
char *lookup_winmsg_name(int msg_code);
DWORD load_exec_filename(void);
LRESULT derive_filename_from_exec(char *drvbfr, char *new_ext);
LRESULT get_base_filename(char *drvbfr);
bool select_text_file(HWND hDlgWnd, char *command_filename);
void add_tooltip_target(HWND parent, HWND target, HWND hToolTip, char *msg);
unsigned select_color(unsigned curr_attr);

