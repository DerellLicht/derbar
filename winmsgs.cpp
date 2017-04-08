//**************************************************************************
//  Copyright (c) 2011-2013  Daniel D Miller
//  This code was removed from common_funcs.cpp because it added 15KB
//  to the output file size, and was rarely needed.
//
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//**************************************************************************
#include <windows.h>

//**************************************************************************
typedef struct win_msgs_s {
   unsigned msg_num ;
   char msg_name[41] ;
} win_msgs_t ;

#define  WM_NOT_FOUND   100000

static win_msgs_t win_msgs[] = {
{ 0, "WM_NULL" },
{ 1, "WM_CREATE" },
{ 2, "WM_DESTROY" },
{ 3, "WM_MOVE" },
{ 5, "WM_SIZE" },
{ 6, "WM_ACTIVATE" },
{ 7, "WM_SETFOCUS" },
{ 8, "WM_KILLFOCUS" },
{ 10, "WM_ENABLE" },
{ 11, "WM_SETREDRAW" },
{ 12, "WM_SETTEXT" },
{ 13, "WM_GETTEXT" },
{ 14, "WM_GETTEXTLENGTH" },
{ 15, "WM_PAINT" },
{ 16, "WM_CLOSE" },
{ 17, "WM_QUERYENDSESSION" },
{ 18, "WM_QUIT" },
{ 19, "WM_QUERYOPEN" },
{ 20, "WM_ERASEBKGND" },
{ 21, "WM_SYSCOLORCHANGE" },
{ 22, "WM_ENDSESSION" },
{ 24, "WM_SHOWWINDOW" },
{ 26, "WM_SETTINGCHANGE" },
{ 26, "WM_WININICHANGE" },
{ 27, "WM_DEVMODECHANGE" },
{ 28, "WM_ACTIVATEAPP" },
{ 29, "WM_FONTCHANGE" },
{ 30, "WM_TIMECHANGE" },
{ 31, "WM_CANCELMODE" },
{ 32, "WM_SETCURSOR" },
{ 33, "WM_MOUSEACTIVATE" },
{ 34, "WM_CHILDACTIVATE" },
{ 35, "WM_QUEUESYNC" },
{ 36, "WM_GETMINMAXINFO" },
{ 38, "WM_PAINTICON" },
{ 39, "WM_ICONERASEBKGND" },
{ 40, "WM_NEXTDLGCTL" },
{ 42, "WM_SPOOLERSTATUS" },
{ 43, "WM_DRAWITEM" },
{ 44, "WM_MEASUREITEM" },
{ 45, "WM_DELETEITEM" },
{ 46, "WM_VKEYTOITEM" },
{ 47, "WM_CHARTOITEM" },
{ 48, "WM_SETFONT" },
{ 49, "WM_GETFONT" },
{ 50, "WM_SETHOTKEY" },
{ 51, "WM_GETHOTKEY" },
{ 55, "WM_QUERYDRAGICON" },
{ 57, "WM_COMPAREITEM" },
{ 61, "WM_GETOBJECT" },
{ 65, "WM_COMPACTING" },
{ 68, "WM_COMMNOTIFY" },
{ 70, "WM_WINDOWPOSCHANGING" },
{ 71, "WM_WINDOWPOSCHANGED" },
{ 72, "WM_POWER" },
{ 74, "WM_COPYDATA" },
{ 75, "WM_CANCELJOURNAL" },
{ 78, "WM_NOTIFY" },
{ 80, "WM_INPUTLANGCHANGEREQUEST" },
{ 81, "WM_INPUTLANGCHANGE" },
{ 82, "WM_TCARD" },
{ 83, "WM_HELP" },
{ 84, "WM_USERCHANGED" },
{ 85, "WM_NOTIFYFORMAT" },
{ 123, "WM_CONTEXTMENU" },
{ 124, "WM_STYLECHANGING" },
{ 125, "WM_STYLECHANGED" },
{ 126, "WM_DISPLAYCHANGE" },
{ 127, "WM_GETICON" },
{ 128, "WM_SETICON" },
{ 129, "WM_NCCREATE" },
{ 130, "WM_NCDESTROY" },
{ 131, "WM_NCCALCSIZE" },
{ 132, "WM_NCHITTEST" },
{ 133, "WM_NCPAINT" },
{ 134, "WM_NCACTIVATE" },
{ 135, "WM_GETDLGCODE" },
{ 136, "WM_SYNCPAINT " },     // 0x88
//  from http://blog.airesoft.co.uk/2009/11/wm_messages/
// #define WM_SYNCTASK 0x0089
// // <undefined> 0x008a
// #define WM_KLUDGEMINRECT 0x008b
// #define WM_LPKDRAWSWITCHWND 0x008c
// // <undefined> 0x008d
// // <undefined> 0x008e
// // <undefined> 0x008f
// #define WM_UAHDESTROYWINDOW 0x0090
// #define WM_UAHDRAWMENU 0x0091
// #define WM_UAHDRAWMENUITEM 0x0092
// #define WM_UAHINITMENU 0x0093
// #define WM_UAHMEASUREMENUITEM 0x0094
// #define WM_UAHNCPAINTMENUPOPUP 0x0095
{ 137, "WM_SYNCTASK" },               // 0x0089
{ 139, "WM_KLUDGEMINRECT" },          // 0x008b
{ 140, "WM_LPKDRAWSWITCHWND" },       // 0x008c
{ 144, "WM_UAHDESTROYWINDOW" },       // 0x0090
{ 145, "WM_UAHDRAWMENU" },            // 0x0091
{ 146, "WM_UAHDRAWMENUITEM" },        // 0x0092
{ 147, "WM_UAHINITMENU" },            // 0x0093
{ 148, "WM_UAHMEASUREMENUITEM" },     // 0x0094
{ 149, "WM_UAHNCPAINTMENUPOPUP" },    // 0x0095
{ 160, "WM_NCMOUSEMOVE" },    // 0xA0
{ 161, "WM_NCLBUTTONDOWN" },
{ 162, "WM_NCLBUTTONUP" },
{ 163, "WM_NCLBUTTONDBLCLK" },
{ 164, "WM_NCRBUTTONDOWN" },
{ 165, "WM_NCRBUTTONUP" },
{ 166, "WM_NCRBUTTONDBLCLK" },
{ 167, "WM_NCMBUTTONDOWN" },
{ 168, "WM_NCMBUTTONUP" },
{ 169, "WM_NCMBUTTONDBLCLK" },
{ 171, "WM_NCXBUTTONDOWN" },
{ 172, "WM_NCXBUTTONUP" },
{ 173, "WM_NCXBUTTONDBLCLK" },
// http://social.msdn.microsoft.com/Forums/en-US/vbgeneral/thread/49e84457-42fe-42d3-ac93-4749b67ab9a2
// Wednesday, November 17, 2010 1:43 AM
// Tom Shelton, 8,570 Points
// Apparently, 0x00AE and 0x00AF are undocumented windows messages related to themes:
// WM_NCUAHDRAWCAPTION = 0x00AE
// WM_NCUAHDRAWFRAME = 0x00AF
{ 174, "WM_NCUAHDRAWCAPTION" },  // WM_NCUAHDRAWCAPTION = 0x00AE
{ 175, "WM_NCUAHDRAWFRAME" },    // WM_NCUAHDRAWFRAME = 0x00AF
{ 177, "EM_SETSEL" },
{ 255, "WM_INPUT" },
{ 256, "WM_KEYDOWN" },
{ 256, "WM_KEYFIRST" },
{ 257, "WM_KEYUP" },
{ 258, "WM_CHAR" },
{ 259, "WM_DEADCHAR" },
{ 260, "WM_SYSKEYDOWN" },
{ 261, "WM_SYSKEYUP" },
{ 262, "WM_SYSCHAR" },
{ 263, "WM_SYSDEADCHAR" },
{ 264, "WM_KEYLAST" },
{ 265, "WM_UNICHAR" },
{ 269, "WM_IME_STARTCOMPOSITION" },
{ 270, "WM_IME_ENDCOMPOSITION" },
{ 271, "WM_IME_KEYLAST" },
{ 272, "WM_INITDIALOG" },
{ 273, "WM_COMMAND" },
{ 274, "WM_SYSCOMMAND" },
{ 275, "WM_TIMER" },
{ 276, "WM_HSCROLL" },
{ 277, "WM_VSCROLL" },
{ 278, "WM_INITMENU" },
{ 279, "WM_INITMENUPOPUP" },
// #if(WINVER >= 0x0601)
{ 281, "WM_GESTURE" }, //                     0x0119
{ 282, "WM_GESTURENOTIFY" }, //                0x011A

{ 287, "WM_MENUSELECT" },
{ 288, "WM_MENUCHAR" },
{ 289, "WM_ENTERIDLE" },
{ 290, "WM_MENURBUTTONUP" },
{ 291, "WM_MENUDRAG" },
{ 292, "WM_MENUGETOBJECT" },
{ 293, "WM_UNINITMENUPOPUP" },
{ 294, "WM_MENUCOMMAND" },
{ 295, "WM_CHANGEUISTATE" },
{ 296, "WM_UPDATEUISTATE" },
{ 297, "WM_QUERYUISTATE" },
{ 306, "WM_CTLCOLORMSGBOX" },
{ 307, "WM_CTLCOLOREDIT" },
{ 308, "WM_CTLCOLORLISTBOX" },
{ 309, "WM_CTLCOLORBTN" },
{ 310, "WM_CTLCOLORDLG" },
{ 311, "WM_CTLCOLORSCROLLBAR" },
{ 312, "WM_CTLCOLORSTATIC" },

/*
 * Combo Box messages
 */
{ 320, "CB_GETEDITSEL" },  //               0x0140
{ 321, "CB_LIMITTEXT" },  //                0x0141
{ 322, "CB_SETEDITSEL" },  //               0x0142
{ 323, "CB_ADDSTRING" },  //                0x0143
{ 324, "CB_DELETESTRING" },  //             0x0144
{ 325, "CB_DIR" },  //                      0x0145
{ 326, "CB_GETCOUNT" },  //                 0x0146
{ 327, "CB_GETCURSEL" },  //                0x0147
{ 328, "CB_GETLBTEXT" },  //                0x0148
{ 329, "CB_GETLBTEXTLEN" },  //             0x0149
{ 330, "CB_INSERTSTRING" },  //             0x014A
{ 331, "CB_RESETCONTENT" },  //             0x014B
{ 332, "CB_FINDSTRING" },  //               0x014C
{ 333, "CB_SELECTSTRING" },  //             0x014D
{ 334, "CB_SETCURSEL" },  //                0x014E
{ 335, "CB_SHOWDROPDOWN" },  //             0x014F
{ 336, "CB_GETITEMDATA" },  //              0x0150
{ 337, "CB_SETITEMDATA" },  //              0x0151
{ 338, "CB_GETDROPPEDCONTROLRECT" },  //    0x0152
{ 339, "CB_SETITEMHEIGHT" },  //            0x0153
{ 340, "CB_GETITEMHEIGHT" },  //            0x0154
{ 341, "CB_SETEXTENDEDUI" },  //            0x0155
{ 342, "CB_GETEXTENDEDUI" },  //            0x0156
{ 343, "CB_GETDROPPEDSTATE" },  //          0x0157
{ 344, "CB_FINDSTRINGEXACT" },  //          0x0158
{ 345, "CB_SETLOCALE" },  //                0x0159
{ 346, "CB_GETLOCALE" },  //                0x015A
{ 347, "CB_GETTOPINDEX" },  //              0x015b
{ 348, "CB_SETTOPINDEX" },  //              0x015c
{ 349, "CB_GETHORIZONTALEXTENT" },  //      0x015d
{ 350, "CB_SETHORIZONTALEXTENT" },  //      0x015e
{ 351, "CB_GETDROPPEDWIDTH" },  //          0x015f
{ 352, "CB_SETDROPPEDWIDTH" },  //          0x0160
{ 353, "CB_INITSTORAGE" },  //              0x0161
{ 354, "CB_MULTIPLEADDSTRING" },  //        0x0163
{ 355, "CB_GETCOMBOBOXINFO" },  //          0x0164

// { 512, "WM_MOUSEFIRST" },
{ 512, "WM_MOUSEMOVE" },
{ 513, "WM_LBUTTONDOWN" },
{ 514, "WM_LBUTTONUP" },
{ 515, "WM_LBUTTONDBLCLK" },
{ 516, "WM_RBUTTONDOWN" },
{ 517, "WM_RBUTTONUP" },
{ 518, "WM_RBUTTONDBLCLK" },
{ 519, "WM_MBUTTONDOWN" },
{ 520, "WM_MBUTTONUP" },
{ 521, "WM_MBUTTONDBLCLK" },
{ 522, "WM_MOUSEWHEEL" },
{ 523, "WM_XBUTTONDOWN" },
{ 524, "WM_XBUTTONUP" },
{ 525, "WM_MOUSELAST" },
{ 528, "WM_PARENTNOTIFY" },
{ 529, "WM_ENTERMENULOOP" },
{ 530, "WM_EXITMENULOOP" },
{ 531, "WM_NEXTMENU" },
{ 532, "WM_SIZING" },
{ 533, "WM_CAPTURECHANGED" },
{ 534, "WM_MOVING" },
{ 536, "WM_POWERBROADCAST" },
{ 537, "WM_DEVICECHANGE" },
{ 544, "WM_MDICREATE" },
{ 545, "WM_MDIDESTROY" },
{ 546, "WM_MDIACTIVATE" },
{ 547, "WM_MDIRESTORE" },
{ 548, "WM_MDINEXT" },
{ 549, "WM_MDIMAXIMIZE" },
{ 550, "WM_MDITILE" },
{ 551, "WM_MDICASCADE" },
{ 552, "WM_MDIICONARRANGE" },
{ 553, "WM_MDIGETACTIVE" },
{ 560, "WM_MDISETMENU" },
{ 561, "WM_ENTERSIZEMOVE" },
{ 562, "WM_EXITSIZEMOVE" },
{ 563, "WM_DROPFILES" },
{ 564, "WM_MDIREFRESHMENU" },
// http://msdn.microsoft.com/en-us/library/windows/desktop/hh454922(v=vs.85).aspx
{ 568, "WM_POINTERDEVICECHANGE" },  //       0X238
{ 569, "WM_POINTERDEVICEINRANGE" },  //     0X239

// #if (_WIN32_WINNT >= 0x0601)
{ 576, "WM_TOUCHMOVE" },
{ 577, "WM_TOUCHDOWN" },
{ 578, "WM_TOUCHUP" },
{ 588, "WM_POINTERCAPTURECHANGED" },  //           0x024C

{ 641, "WM_IME_SETCONTEXT" },
{ 642, "WM_IME_NOTIFY" },
{ 643, "WM_IME_CONTROL" },
{ 644, "WM_IME_COMPOSITIONFULL" },
{ 645, "WM_IME_SELECT" },
{ 646, "WM_IME_CHAR" },
{ 648, "WM_IME_REQUEST" },
{ 656, "WM_IME_KEYDOWN" },
{ 657, "WM_IME_KEYUP" },
{ 672, "WM_NCMOUSEHOVER" },
{ 673, "WM_MOUSEHOVER" },
{ 674, "WM_NCMOUSELEAVE" },
{ 675, "WM_MOUSELEAVE" },
{ 689, "WM_WTSSESSION_CHANGE" },
{ 704, "WM_TABLET_FIRST" },
{ 735, "WM_TABLET_LAST" },
{ 768, "WM_CUT" },
{ 769, "WM_COPY" },
{ 770, "WM_PASTE" },
{ 771, "WM_CLEAR" },
{ 772, "WM_UNDO" },
{ 773, "WM_RENDERFORMAT" },
{ 774, "WM_RENDERALLFORMATS" },
{ 775, "WM_DESTROYCLIPBOARD" },
{ 776, "WM_DRAWCLIPBOARD" },
{ 777, "WM_PAINTCLIPBOARD" },
{ 778, "WM_VSCROLLCLIPBOARD" },
{ 779, "WM_SIZECLIPBOARD" },
{ 780, "WM_ASKCBFORMATNAME" },
{ 781, "WM_CHANGECBCHAIN" },
{ 782, "WM_HSCROLLCLIPBOARD" },
{ 783, "WM_QUERYNEWPALETTE" },
{ 784, "WM_PALETTEISCHANGING" },
{ 785, "WM_PALETTECHANGED" },
{ 786, "WM_HOTKEY" },
{ 791, "WM_PRINT" },
{ 792, "WM_PRINTCLIENT" },
{ 793, "WM_APPCOMMAND" },
{ 794, "WM_THEMECHANGED" },
// http://sda-framework.googlecode.com/svn-history/r37/trunk/sdaUnits/sdaMessages.pas
// WM_DWMCOMPOSITIONCHANGED          = $031E;
// WM_DWMNCRENDERINGCHANGED          = $031F;
// WM_DWMCOLORIZATIONCOLORCHANGED    = $0320;
// WM_DWMWINDOWMAXIMIZEDCHANGE       = $0321;
// WM_DWMSENDICONICTHUMBNAIL         = $0323;
// WM_DWMSENDICONICLIVEPREVIEWBITMAP = $0326;
// WM_GETTITLEBARINFOEX              = $033F;
{ 798, "WM_DWMCOMPOSITIONCHANGED" },         // 0x031E;
{ 799, "WM_DWMNCRENDERINGCHANGED" },         // 0x031F;
{ 800, "WM_DWMCOLORIZATIONCOLORCHANGED" },   // 0x0320;
{ 801, "WM_DWMWINDOWMAXIMIZEDCHANGE" },      // 0x0321;
{ 803, "WM_DWMSENDICONICTHUMBNAIL" },        // 0x0323;
{ 806, "WM_DWMSENDICONICLIVEPREVIEWBITMAP" },// 0x0326;
{ 831, "WM_DWMSENDICONICLIVEPREVIEWBITMAP" },// 0x033F;

{ 856, "WM_HANDHELDFIRST" },
{ 863, "WM_HANDHELDLAST" },
{ 864, "WM_AFXFIRST" },
{ 895, "WM_AFXLAST" },
{ 896, "WM_PENWINFIRST" },
{ 911, "WM_PENWINLAST" },

// http://sda-framework.googlecode.com/svn-history/r37/trunk/sdaUnits/sdaMessages.pas
// { 831, "WM_DDE_FIRST    " },   // $03E0;
{ 992, "WM_DDE_INITIATE " },   // WM_DDE_FIRST + 0;
{ 993, "WM_DDE_TERMINATE" },   // WM_DDE_FIRST + 1;
{ 994, "WM_DDE_ADVISE   " },   // WM_DDE_FIRST + 2;
{ 995, "WM_DDE_UNADVISE " },   // WM_DDE_FIRST + 3;
{ 996, "WM_DDE_ACK      " },   // WM_DDE_FIRST + 4;
{ 997, "WM_DDE_DATA     " },   // WM_DDE_FIRST + 5;
{ 998, "WM_DDE_REQUEST  " },   // WM_DDE_FIRST + 6;
{ 999, "WM_DDE_POKE     " },   // WM_DDE_FIRST + 7;
{ 1000, "WM_DDE_EXECUTE  " },  // WM_DDE_FIRST + 8;

{ 1024, "DM_GETDEFID" },
{ 1025, "DM_SETDEFID" },
{ 1026, "DM_REPOSITION" },

//  PSM mesages
{ 1125, "PSM_SETCURSEL" },
{ 1126, "PSM_REMOVEPAGE" },
{ 1127, "PSM_ADDPAGE" },  // { 1127, "WM_DO_COMM_TASK" },
{ 1128, "PSM_CHANGED" },  // { 1128, "WM_COMM_TASK_DONE" },
{ 1129, "PSM_RESTARTWINDOWS" },   
{ 1130, "PSM_REBOOTSYSTEM" },     
{ 1131, "PSM_CANCELTOCLOSE" },    
{ 1132, "PSM_QUERYSIBLINGS" },    
{ 1133, "PSM_UNCHANGED" },        
{ 1134, "PSM_APPLY" },            
{ 1135, "PSM_SETTITLEA" },        
{ 1136, "PSM_SETWIZBUTTONS" },    
{ 1137, "PSM_PRESSBUTTON" },         //  WM_USER + 113
{ 1138, "PSM_SETCURSELID" },         
{ 1139, "PSM_SETFINISHTEXTA" },      
{ 1140, "PSM_GETTABCONTROL" },       
{ 1141, "PSM_ISDIALOGMESSAGE" },     
{ 1142, "PSM_GETCURRENTPAGEHWND" },  
{ 1144, "PSM_SETTITLEW" },
{ 1145, "PSM_SETFINISHTEXTW" },      

{ 4096, "LVM_GETBKCOLOR" },
{ 4097, "LVM_SETBKCOLOR" },
{ 4098, "LVM_GETIMAGELIST" },
{ 4099, "LVM_SETIMAGELIST" },
{ 4100, "LVM_GETITEMCOUNT" },
{ 4101, "LVM_GETITEMA" },
{ 4102, "LVM_SETITEMA" },
{ 4103, "LVM_INSERTITEMA" },
{ 4104, "LVM_DELETEITEM" },
{ 4105, "LVM_DELETEALLITEMS" },
{ 4106, "LVM_GETCALLBACKMASK" },
{ 4107, "LVM_SETCALLBACKMASK" },
{ 4108, "LVM_GETNEXTITEM" },
{ 4109, "LVM_FINDITEMA" },
{ 4110, "LVM_GETITEMRECT" },
{ 4111, "LVM_SETITEMPOSITION" },
{ 4112, "LVM_GETITEMPOSITION" },
{ 4113, "LVM_GETSTRINGWIDTHA" },
{ 4114, "LVM_HITTEST" },
{ 4115, "LVM_ENSUREVISIBLE" },
{ 4116, "LVM_SCROLL" },
{ 4117, "LVM_REDRAWITEMS" },
{ 4118, "LVM_ARRANGE" },
{ 4119, "LVM_EDITLABELA" },
{ 4120, "LVM_GETEDITCONTROL" },
{ 4121, "LVM_GETCOLUMNA" },
{ 4122, "LVM_SETCOLUMNA" },
{ 4123, "LVM_INSERTCOLUMNA" },
{ 4124, "LVM_DELETECOLUMN" },
{ 4125, "LVM_GETCOLUMNWIDTH" },
{ 4126, "LVM_SETCOLUMNWIDTH" },
{ 4127, "LVM_GETHEADER" },
{ 4129, "LVM_CREATEDRAGIMAGE" },
{ 4130, "LVM_GETVIEWRECT" },
{ 4131, "LVM_GETTEXTCOLOR" },
{ 4132, "LVM_SETTEXTCOLOR" },
{ 4133, "LVM_GETTEXTBKCOLOR" },
{ 4134, "LVM_SETTEXTBKCOLOR" },
{ 4135, "LVM_GETTOPINDEX" },
{ 4136, "LVM_GETCOUNTPERPAGE" },
{ 4137, "LVM_GETORIGIN" },
{ 4137, "LVM_GETORIGIN" },
{ 4138, "LVM_UPDATE" },
{ 4139, "LVM_SETITEMSTATE" },
{ 4140, "LVM_GETITEMSTATE" },
{ 4141, "LVM_GETITEMTEXTA" },
{ 4142, "LVM_SETITEMTEXTA" },
{ 4143, "LVM_SETITEMCOUNT" },
{ 4144, "LVM_SORTITEMS" },
{ 4145, "LVM_SETITEMPOSITION32" },
{ 4146, "LVM_GETSELECTEDCOUNT" },
{ 4147, "LVM_GETITEMSPACING" },
{ 4148, "LVM_GETISEARCHSTRINGA" },
{ 4149, "LVM_SETICONSPACING" },
{ 4150, "LVM_SETEXTENDEDLISTVIEWSTYLE" },
{ 4151, "LVM_GETEXTENDEDLISTVIEWSTYLE" },
{ 4152, "LVM_GETSUBITEMRECT" },
{ 4153, "LVM_SUBITEMHITTEST" },
{ 4154, "LVM_SETCOLUMNORDERARRAY" },
{ 4155, "LVM_GETCOLUMNORDERARRAY" },
{ 4156, "LVM_SETHOTITEM" },
{ 4157, "LVM_GETHOTITEM" },
{ 4158, "LVM_SETHOTCURSOR" },
{ 4159, "LVM_GETHOTCURSOR" },
{ 4160, "LVM_APPROXIMATEVIEWRECT" },
{ 4161, "LVM_SETWORKAREAS" },
{ 4162, "LVM_GETSELECTIONMARK" },
{ 4163, "LVM_SETSELECTIONMARK" },
{ 4164, "LVM_SETBKIMAGEA" },
{ 4165, "LVM_GETBKIMAGEA" },
{ 4166, "LVM_GETWORKAREAS" },
{ 4167, "LVM_SETHOVERTIME" },
{ 4168, "LVM_GETHOVERTIME" },
{ 4169, "LVM_GETNUMBEROFWORKAREAS" },
{ 4170, "LVM_SETTOOLTIPS" },
{ 4171, "LVM_GETITEMW" },
{ 4172, "LVM_SETITEMW" },
{ 4173, "LVM_INSERTITEMW" },
{ 4174, "LVM_GETTOOLTIPS" },
{ 4177, "LVM_SORTITEMSEX" },
{ 4179, "LVM_FINDITEMW" },
{ 4183, "LVM_GETSTRINGWIDTHW" },
{ 4191, "LVM_GETCOLUMNW" },
{ 4192, "LVM_SETCOLUMNW" },
{ 4193, "LVM_INSERTCOLUMNW" },
{ 4211, "LVM_GETITEMTEXTW" },
{ 4212, "LVM_SETITEMTEXTW" },
{ 4213, "LVM_GETISEARCHSTRINGW" },
{ 4214, "LVM_EDITLABELW" },
{ 4234, "LVM_SETBKIMAGEW" },
{ 4235, "LVM_GETBKIMAGEW" },
{ 4236, "LVM_SETSELECTEDCOLUMN" },
{ 4238, "LVM_SETVIEW" },
{ 4239, "LVM_GETVIEW" },
{ 4241, "LVM_INSERTGROUP" },
{ 4243, "LVM_SETGROUPINFO" },
{ 4245, "LVM_GETGROUPINFO" },
{ 4246, "LVM_REMOVEGROUP" },
{ 4247, "LVM_MOVEGROUP" },
{ 4251, "LVM_SETGROUPMETRICS" },
{ 4252, "LVM_GETGROUPMETRICS" },
{ 4253, "LVM_ENABLEGROUPVIEW" },
{ 4254, "LVM_SORTGROUPS" },
{ 4255, "LVM_INSERTGROUPSORTED" },
{ 4256, "LVM_REMOVEALLGROUPS" },
{ 4257, "LVM_HASGROUP" },
{ 4258, "LVM_SETTILEVIEWINFO" },
{ 4259, "LVM_GETTILEVIEWINFO" },
{ 4260, "LVM_SETTILEINFO" },
{ 4261, "LVM_GETTILEINFO" },
{ 4262, "LVM_SETINSERTMARK" },
{ 4263, "LVM_GETINSERTMARK" },
{ 4264, "LVM_INSERTMARKHITTEST" },
{ 4265, "LVM_GETINSERTMARKRECT" },
{ 4266, "LVM_SETINSERTMARKCOLOR" },
{ 4267, "LVM_GETINSERTMARKCOLOR" },
{ 4269, "LVM_SETINFOTIP" },
{ 4270, "LVM_GETSELECTEDCOLUMN" },
{ 4271, "LVM_ISGROUPVIEWENABLED" },
{ 4272, "LVM_GETOUTLINECOLOR" },
{ 4273, "LVM_SETOUTLINECOLOR" },
{ 4275, "LVM_CANCELEDITLABEL" },
{ 4277, "LVM_MAPIDTOINDEX" },
{ 4352, "TVM_INSERTITEMA" },
{ 4353, "TVM_DELETEITEM" },
{ 4354, "TVM_EXPAND" },
{ 4356, "TVM_GETITEMRECT" },
{ 4357, "TVM_GETCOUNT" },
{ 4358, "TVM_GETINDENT" },
{ 4359, "TVM_SETINDENT" },
{ 4360, "TVM_GETIMAGELIST" },
{ 4361, "TVM_SETIMAGELIST" },
{ 4362, "TVM_GETNEXTITEM" },
{ 4363, "TVM_SELECTITEM" },
{ 4364, "TVM_GETITEMA" },
{ 4365, "TVM_SETITEMA" },
{ 4366, "TVM_EDITLABELA" },
{ 4367, "TVM_GETEDITCONTROL" },
{ 4368, "TVM_GETVISIBLECOUNT" },
{ 4369, "TVM_HITTEST" },
{ 4370, "TVM_CREATEDRAGIMAGE" },
{ 4371, "TVM_SORTCHILDREN" },
{ 4372, "TVM_ENSUREVISIBLE" },
{ 4373, "TVM_SORTCHILDRENCB" },
{ 4374, "TVM_ENDEDITLABELNOW" },
{ 4375, "TVM_GETISEARCHSTRINGA" },
{ 4376, "TVM_SETTOOLTIPS" },
{ 4377, "TVM_GETTOOLTIPS" },
{ 4378, "TVM_SETINSERTMARK" },
{ 4379, "TVM_SETITEMHEIGHT" },
{ 4380, "TVM_GETITEMHEIGHT" },
{ 4381, "TVM_SETBKCOLOR" },
{ 4382, "TVM_SETTEXTCOLOR" },
{ 4383, "TVM_GETBKCOLOR" },
{ 4384, "TVM_GETTEXTCOLOR" },
{ 4385, "TVM_SETSCROLLTIME" },
{ 4386, "TVM_GETSCROLLTIME" },
{ 4389, "TVM_SETINSERTMARKCOLOR" },
{ 4390, "TVM_GETINSERTMARKCOLOR" },
{ 4391, "TVM_GETITEMSTATE" },
{ 4392, "TVM_SETLINECOLOR" },
{ 4393, "TVM_GETLINECOLOR" },
{ 4402, "TVM_INSERTITEMW" },
{ 4414, "TVM_GETITEMW" },
{ 4415, "TVM_SETITEMW" },
{ 4416, "TVM_GETISEARCHSTRINGW" },
{ 4417, "TVM_EDITLABELW" },
{ 4608, "HDM_GETITEMCOUNT" },
{ 4609, "HDM_INSERTITEMA" },
{ 4610, "HDM_DELETEITEM" },
{ 4611, "HDM_GETITEMA" },
{ 4612, "HDM_SETITEMA" },
{ 4613, "HDM_LAYOUT" },
{ 4614, "HDM_HITTEST" },
{ 4615, "HDM_GETITEMRECT" },
{ 4616, "HDM_SETIMAGELIST" },
{ 4617, "HDM_GETIMAGELIST" },
{ 4618, "HDM_INSERTITEMW" },
{ 4619, "HDM_GETITEMW" },
{ 4620, "HDM_SETITEMW" },
{ 4623, "HDM_ORDERTOINDEX" },
{ 4624, "HDM_CREATEDRAGIMAGE" },
{ 4625, "HDM_GETORDERARRAY" },
{ 4626, "HDM_SETORDERARRAY" },
{ 4627, "HDM_SETHOTDIVIDER" },
{ 4866, "TCM_GETIMAGELIST" },
{ 4867, "TCM_SETIMAGELIST" },
{ 4868, "TCM_GETITEMCOUNT" },
{ 4869, "TCM_GETITEMA" },
{ 4870, "TCM_SETITEMA" },
{ 4871, "TCM_INSERTITEMA" },
{ 4872, "TCM_DELETEITEM" },
{ 4873, "TCM_DELETEALLITEMS" },
{ 4874, "TCM_GETITEMRECT" },
{ 4875, "TCM_GETCURSEL" },
{ 4876, "TCM_SETCURSEL" },
{ 4877, "TCM_HITTEST" },
{ 4878, "TCM_SETITEMEXTRA" },
{ 4904, "TCM_ADJUSTRECT" },
{ 4905, "TCM_SETITEMSIZE" },
{ 4906, "TCM_REMOVEIMAGE" },
{ 4907, "TCM_SETPADDING" },
{ 4908, "TCM_GETROWCOUNT" },
{ 4909, "TCM_GETTOOLTIPS" },
{ 4910, "TCM_SETTOOLTIPS" },
{ 4911, "TCM_GETCURFOCUS" },
{ 4912, "TCM_SETCURFOCUS" },
{ 4913, "TCM_SETMINTABWIDTH" },
{ 4914, "TCM_DESELECTALL" },
{ 4915, "TCM_HIGHLIGHTITEM" },
{ 4916, "TCM_SETEXTENDEDSTYLE" },
{ 4917, "TCM_GETEXTENDEDSTYLE" },
{ 4924, "TCM_GETITEMW" },
{ 4925, "TCM_SETITEMW" },
{ 4926, "TCM_INSERTITEMW" },
{ 5121, "PGM_SETCHILD" },
{ 5122, "PGM_RECALCSIZE" },
{ 5123, "PGM_FORWARDMOUSE" },
{ 5124, "PGM_SETBKCOLOR" },
{ 5125, "PGM_GETBKCOLOR" },
{ 5126, "PGM_SETBORDER" },
{ 5127, "PGM_GETBORDER" },
{ 5128, "PGM_SETPOS" },
{ 5129, "PGM_GETPOS" },
{ 5130, "PGM_SETBUTTONSIZE" },
{ 5131, "PGM_GETBUTTONSIZE" },
{ 5132, "PGM_GETBUTTONSTATE" },

{ 32768, "WM_APP" },
{ WM_NOT_FOUND, "WM_NOT_FOUND" } };

// #define NM_FIRST  0
// #define NM_LAST   ((UINT)-99)
// #define LVN_FIRST ((UINT)-100)
// #define LVN_LAST  ((UINT)-199)
// #define HDN_FIRST ((UINT)-300)
// #define HDN_LAST  ((UINT)-399)
// #define TVN_FIRST ((UINT)-400)
// #define TVN_LAST  ((UINT)-499)
// #define TTN_FIRST ((UINT)-520)
// #define TTN_LAST  ((UINT)-549)
// #define TCN_FIRST ((UINT)-550)
// #define TCN_LAST  ((UINT)-580)
// #ifndef CDN_FIRST
// #define CDN_FIRST ((UINT)-601) /* also in commdlg.h */
// #define CDN_LAST  ((UINT)-699)
// #endif
// #define TBN_FIRST ((UINT)-700)
// #define TBN_LAST  ((UINT)-720)
// #define UDN_FIRST ((UINT)-721)
// #define UDN_LAST  ((UINT)-740)
// #if (_WIN32_IE >= 0x0300)
// #define RBN_FIRST ((UINT)-831)
// #define RBN_LAST  ((UINT)-859)
// #define MCN_FIRST ((UINT)-750)
// #define MCN_LAST  ((UINT)-759)
// #define DTN_FIRST ((UINT)-760)
// #define DTN_LAST  ((UINT)-799)
// #define CBEN_FIRST   ((UINT)-800)
// #define CBEN_LAST ((UINT)-830)
// #endif /* _WIN32_IE */
// #if (_WIN32_IE >= 0x0400)
// #define IPN_FIRST ((UINT)-860)
// #define IPN_LAST  ((UINT)-879)
// #define IPN_FIELDCHANGED   (IPN_FIRST-0)
// #define SBN_FIRST ((UINT)-880U)
// #define SBN_LAST  ((UINT)-899U)
// #define PGN_FIRST ((UINT)-900U)
// #define PGN_LAST  ((UINT)-950U)
// #define PGN_SCROLL   (PGN_FIRST-1)
// #define PGN_CALCSIZE (PGN_FIRST-2)

// #define TTN_FIRST ((UINT)-520)

static win_msgs_t win_neg_msgs[] = {
{ 0, "WM_NULL" },

{ 1,  "NM_OUTOFMEMORY" },
{ 2,  "NM_CLICK" },
{ 3,  "NM_DBLCLK" },          
{ 4,  "NM_RETURN" },          
{ 5,  "NM_RCLICK" },          
{ 6,  "NM_RDBLCLK" },         
{ 7,  "NM_SETFOCUS" },        
{ 8,  "NM_KILLFOCUS" },       
{ 12, "NM_CUSTOMDRAW" },      
{ 13, "NM_HOVER" },           
{ 14, "NM_NCHITTEST" },       
{ 15, "NM_KEYDOWN" },         
{ 16, "NM_RELEASEDCAPTURE" }, 
{ 17, "NM_SETCURSOR" },       
{ 18, "NM_CHAR" },            
{ 19, "NM_TOOLTIPSCREATED" }, 

{ 100, "LVN_ITEMCHANGING" },     
{ 101, "LVN_ITEMCHANGED" },      
{ 102, "LVN_INSERTITEM" },       
{ 103, "LVN_DELETEITEM" },       
{ 104, "LVN_DELETEALLITEMS" },   
{ 105, "LVN_BEGINLABELEDITA" },  
{ 106, "LVN_ENDLABELEDITA" },    
{ 108, "LVN_COLUMNCLICK" },      
{ 109, "LVN_BEGINDRAG" },        
{ 111, "LVN_BEGINRDRAG" },       
{ 113, "LVN_ODCACHEHINT" },      
{ 114, "LVN_ITEMACTIVATE" },     
{ 115, "LVN_ODSTATECHANGED" },   

{ 121, "LVN_HOTTRACK" },

{ 150, "LVN_GETDISPINFOA" },      
{ 151, "LVN_SETDISPINFOA" },
{ 152, "LVN_ODFINDITEMA" },
{ 155, "LVN_KEYDOWN" },
{ 156, "LVN_MARQUEEBEGIN" },
{ 157, "LVN_GETINFOTIPA" },
{ 158, "LVN_GETINFOTIPW" },

{ 175, "LVN_BEGINLABELEDITW" },   
{ 176, "LVN_ENDLABELEDITW" },     
{ 177, "LVN_GETDISPINFOW" },
{ 178, "LVN_SETDISPINFOW" },      
{ 179, "LVN_ODFINDITEMW" },       

//  prsht.h
{ 200, "PSN_SETACTIVE" },              
{ 201, "PSN_KILLACTIVE" },             
{ 202, "PSN_APPLY" },                  
{ 203, "PSN_RESET" },                  
{ 205, "PSN_HELP" },                   
{ 206, "PSN_WIZBACK" },                
{ 207, "PSN_WIZNEXT" },                
{ 208, "PSN_WIZFINISH" },              
{ 209, "PSN_QUERYCANCEL" },            
{ 210, "PSN_GETOBJECT" },              
{ 212, "PSN_TRANSLATEACCELERATOR" },   
{ 213, "PSN_QUERYINITIALFOCUS" },      

// #define HDN_FIRST ((UINT)-300)
// #define HDN_LAST  ((UINT)-399)
{ 300, "HDN_ITEMCHANGINGA" },     
{ 301, "HDN_ITEMCHANGEDA" },      
{ 302, "HDN_ITEMCLICKA" },        
{ 303, "HDN_ITEMDBLCLICKA" },     
{ 305, "HDN_DIVIDERDBLCLICKA" },  
{ 306, "HDN_BEGINTRACKA" },       
{ 307, "HDN_ENDTRACKA" },         
{ 308, "HDN_TRACKA" },            
{ 309, "HDN_GETDISPINFOA" },      
{ 310, "HDN_BEGINDRAG" },         
{ 311, "HDN_ENDDRAG" },           
{ 320, "HDN_ITEMCHANGINGW" },     
{ 321, "HDN_ITEMCHANGEDW" },      
{ 322, "HDN_ITEMCLICKW" },        
{ 323, "HDN_ITEMDBLCLICKW" },     
{ 325, "HDN_DIVIDERDBLCLICKW" },  
{ 326, "HDN_BEGINTRACKW" },       
{ 327, "HDN_ENDTRACKW" },         
{ 328, "HDN_TRACKW" },            
{ 329, "HDN_GETDISPINFOW" },      

{ 401, "TVN_SELCHANGINGA" },
{ 402, "TVN_SELCHANGEDA" },
{ 403, "TVN_GETDISPINFOA" },
{ 404, "TVN_SETDISPINFOA" },
{ 405, "TVN_ITEMEXPANDINGA" },
{ 406, "TVN_ITEMEXPANDEDA" },
{ 407, "TVN_BEGINDRAGA" },
{ 408, "TVN_BEGINRDRAGA" },
{ 409, "TVN_DELETEITEMA" },
{ 410, "TVN_BEGINLABELEDITA" },
{ 411, "TVN_ENDLABELEDITA" },
{ 412, "TVN_KEYDOWN" },
{ 413, "TVN_GETINFOTIPA" },
{ 414, "TVN_GETINFOTIPW" },
{ 415, "TVN_SINGLEEXPAND" },
{ 450, "TVN_SELCHANGINGW" },
{ 451, "TVN_SELCHANGEDW" },
{ 452, "TVN_GETDISPINFOW" },
{ 453, "TVN_SETDISPINFOW" },
{ 454, "TVN_ITEMEXPANDINGW" },
{ 455, "TVN_ITEMEXPANDEDW" },
{ 456, "TVN_BEGINDRAGW" },
{ 457, "TVN_BEGINRDRAGW" },
{ 458, "TVN_DELETEITEMW" },
{ 459, "TVN_BEGINLABELEDITW" },
{ 460, "TVN_ENDLABELEDITW" },

{ 520, "TTN_GETDISPINFOA" },      
{ 521, "TTN_SHOW" },      
{ 522, "TTN_POP" },      
{ 530, "TTN_GETDISPINFOW" },      

{ 550, "TCN_KEYDOWN" },      
{ 551, "TCN_SELCHANGE" },      
{ 552, "TCN_SELCHANGING" },      

{ 700, "TBN_GETBUTTONINFOA" },
{ 701, "TBN_BEGINDRAG" },
{ 702, "TBN_ENDDRAG" },
{ 703, "TBN_BEGINADJUST" },
{ 704, "TBN_ENDADJUST" },
{ 705, "TBN_RESET" },
{ 706, "TBN_QUERYINSERT" },
{ 707, "TBN_QUERYDELETE" },
{ 708, "TBN_TOOLBARCHANGE" },
{ 709, "TBN_CUSTHELP" },
{ 710, "TBN_DROPDOWN" },
{ 713, "TBN_HOTITEMCHANGE" },
{ 714, "TBN_DRAGOUT" },
{ 715, "TBN_DELETINGBUTTON" },
{ 716, "TBN_GETDISPINFOA" },
{ 717, "TBN_GETDISPINFOW" },
{ 718, "TBN_GETINFOTIPA" },
{ 719, "TBN_GETINFOTIPW" },
{ 720, "TBN_GETBUTTONINFOW" },
{ 721, "TBN_RESTORE" },
{ 722, "TBN_SAVE" },
{ 722, "UDN_DELTAPOS" },
{ 723, "TBN_INITCUSTOMIZE" },
{ 800, "CBEN_GETDISPINFOA" },
{ 801, "CBEN_INSERTITEM" },
{ 802, "CBEN_DELETEITEM" },
{ 804, "CBEN_BEGINEDIT" },
{ 805, "CBEN_ENDEDITA" },
{ 806, "CBEN_ENDEDITW" },
{ 807, "CBEN_GETDISPINFOW" },
{ 808, "CBEN_DRAGBEGINA" },
{ 809, "CBEN_DRAGBEGINW" },
{ 831, "RBN_HEIGHTCHANGE" },
{ 832, "RBN_GETOBJECT" },
{ 833, "RBN_LAYOUTCHANGED" },
{ 834, "RBN_AUTOSIZE" },
{ 835, "RBN_BEGINDRAG" },
{ 836, "RBN_ENDDRAG" },
{ 837, "RBN_DELETINGBAND" },
{ 838, "RBN_DELETEDBAND" },
{ 839, "RBN_CHILDSIZE" },
{ 860, "IPN_FIELDCHANGED" },
{ 880, "SBN_SIMPLEMODECHANGE" },
{ 901, "PGN_SCROLL" },
{ 902, "PGN_CALCSIZE" },

{ WM_NOT_FOUND, "WM_NOT_FOUND" } };

//*********************************************************************
//lint -esym(714, lookup_winmsg_name)
//lint -esym(759, lookup_winmsg_name)
//lint -esym(765, lookup_winmsg_name)
char *lookup_winmsg_name(int msg_code)
{
   static char msgstr[81] ;
   unsigned j ;
   bool is_negative = false ;
   if (msg_code >= 0) {
      if (msg_code >= WM_APP) {
         wsprintfA(msgstr, "WM_APP + %u", msg_code - WM_APP) ;
         return msgstr;
      }
      for (j=0; win_msgs[j].msg_num != WM_NOT_FOUND; j++) {
         if (win_msgs[j].msg_num == (unsigned) msg_code)
            return win_msgs[j].msg_name ;
      }
   } else {
      msg_code = -msg_code ;
      is_negative = true ;
      for (j=0; win_neg_msgs[j].msg_num != WM_NOT_FOUND; j++) {
         if (win_neg_msgs[j].msg_num == (unsigned) msg_code)
            return win_neg_msgs[j].msg_name ;
      }
   }
   if (is_negative)
      wsprintfA(msgstr, "Msg not found [-%d]", msg_code) ;
   else
      wsprintfA(msgstr, "Msg not found [%d]", msg_code) ;
   return msgstr;
}

