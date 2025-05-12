//****************************************************************************
//  Copyright (c) 2009-2025  Daniel D Miller
//  images.cpp - handle ImageList functions for LED bitmaps
//
//  Produced and Directed by:  Dan Miller
//****************************************************************************
#include <windows.h>
// #include <vector>
#include <commctrl.h>
#include <tchar.h>

#include "resource.h"
#include "common.h"
#include "images.h"
#include "derbar.h"

//  led-image data
static HIMAGELIST hiSpriteList = 0 ;
static HICON hiLeds[6] = { 0, 0, 0, 0, 0, 0 } ;
// static std::vector<HICON> hiLeds { 0, 0, 0, 0, 0, 0 } ;

//****************************************************************************
bool load_led_images(void)
{
   int image_height = 0 ;
   int el_width = 0 ;
   hiSpriteList = ImageList_LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_LEDBMPS), image_height, 0, CLR_DEFAULT, IMAGE_BITMAP, LR_CREATEDIBSECTION) ;
   if (hiSpriteList == NULL) {
      syslog(L"ImageList_LoadBitmap: %s\n", get_system_message()) ;
      return false;
   }
   ImageList_GetIconSize(hiSpriteList, &el_width, &image_height) ;

   for (unsigned idx=0; idx<6; idx++) {
      hiLeds[idx] = ImageList_ExtractIcon(NULL, hiSpriteList, idx) ;
      if (hiLeds[idx] == NULL) {
         syslog(L"ImageList_ExtractIcon: %s\n", get_system_message()) ;
      }
   }

   // syslog("LED image list dimensions: width=%d, height=%d\n", el_width, image_height) ;
   //  FYI: ImageList_GetImageCount() returns number of elements in list
   return true;
}

//***********************************************************************
HIMAGELIST get_image_list(void)
{
   return hiSpriteList ;
}

//***********************************************************************
void release_led_images(void)
{
   for (unsigned idx=0; idx<6; idx++) {
      if (hiLeds[idx] != NULL) {
         DestroyIcon(hiLeds[idx]) ;
         hiLeds[idx] = NULL ;
      }
   }
   if (hiSpriteList != 0) {
      // DeleteObject(hiSpriteList) ;  //  delete image??
      hiSpriteList = 0 ;
   }
}

//***********************************************************************
// void draw_led(HWND hwndBmp, unsigned idx)
// {
//    if (hiLeds[idx] != NULL)
//       // SendMessageA(hwndBmp, STM_SETIMAGE, (WPARAM) IMAGE_ICON,(LPARAM) hiLeds[idx]);
//       PostMessageA(hwndBmp, STM_SETIMAGE, (WPARAM) IMAGE_ICON,(LPARAM) hiLeds[idx]);
// }

//***********************************************************************
// void draw_ledb(HWND hwndBmp, bool idx)
// {
//    if (idx)
//       PostMessageA(hwndBmp, STM_SETIMAGE, (WPARAM) IMAGE_ICON,(LPARAM) hiLeds[LED_IDX_GREEN]);
//    else
//       PostMessageA(hwndBmp, STM_SETIMAGE, (WPARAM) IMAGE_ICON,(LPARAM) hiLeds[LED_IDX_OFF]);
// }

