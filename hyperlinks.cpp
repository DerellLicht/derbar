// Hyperlinks.cpp
//
// Copyright 2002 Neal Stublen
// All rights reserved.
//
// http://www.awesoftware.com
//
// DDM: obtained on 10/05/10 from:
// http://www.codeguru.com/cpp/controls/staticctrl/article.php/c5803

#include <windows.h>

//lint -esym(759, ConvertStaticToHyperlink)
//lint -esym(765, ConvertStaticToHyperlink)

#include "iface_32_64.h"
#include "hyperlinks.h"

#define PROP_ORIGINAL_FONT    TEXT("_Hyperlink_Original_Font_")
#define PROP_ORIGINAL_PROC    TEXT("_Hyperlink_Original_Proc_")
#define PROP_STATIC_HYPERLINK TEXT("_Hyperlink_From_Static_")
#define PROP_UNDERLINE_FONT   TEXT("_Hyperlink_Underline_Font_")

//*************************************************************************************
static LRESULT CALLBACK _HyperlinkParentProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   WNDPROC pfnOrigProc = (WNDPROC) GetProp(hwnd, PROP_ORIGINAL_PROC);   //lint !e611

   switch (message)
   {
   case WM_CTLCOLORSTATIC:
      {
         HDC hdc = (HDC) wParam;
         HWND hwndCtl = (HWND) lParam;

         BOOL fHyperlink = (NULL != GetProp(hwndCtl, PROP_STATIC_HYPERLINK));
         if (fHyperlink)
         {
            LRESULT lr = CallWindowProc(pfnOrigProc, hwnd, message, wParam, lParam);
            SetTextColor(hdc, RGB(0, 0, 192));
            return lr;
         }

         break;
      }
   case WM_DESTROY:
      {
#ifdef _WIN64      
         SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR) pfnOrigProc);
#else         
         SetWindowLong(hwnd, GWL_WNDPROC, (LONG) pfnOrigProc);
#endif         
         RemoveProp(hwnd, PROP_ORIGINAL_PROC);
         break;
      }
   }  //lint !e744
   return CallWindowProc(pfnOrigProc, hwnd, message, wParam, lParam);
}

//*************************************************************************************
// static LRESULT CALLBACK _HyperlinkProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
static INT_PTR CALLBACK _HyperlinkProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   WNDPROC pfnOrigProc = (WNDPROC) GetProp(hwnd, PROP_ORIGINAL_PROC);   //lint !e611

   switch (message)
   {
   case WM_DESTROY:
      {
#ifdef _WIN64      
         SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR) pfnOrigProc);
#else         
         SetWindowLong(hwnd, GWL_WNDPROC, (LONG) pfnOrigProc);
#endif         
         RemoveProp(hwnd, PROP_ORIGINAL_PROC);

         HFONT hOrigFont = (HFONT) GetProp(hwnd, PROP_ORIGINAL_FONT);
         SendMessage(hwnd, WM_SETFONT, (WPARAM) hOrigFont, 0);
         RemoveProp(hwnd, PROP_ORIGINAL_FONT);

         HFONT hFont = (HFONT) GetProp(hwnd, PROP_UNDERLINE_FONT);
         DeleteObject(hFont);
         RemoveProp(hwnd, PROP_UNDERLINE_FONT);

         RemoveProp(hwnd, PROP_STATIC_HYPERLINK);

         break;
      }
   case WM_MOUSEMOVE:
      {
         if (GetCapture() != hwnd)
         {
            HFONT hFont = (HFONT) GetProp(hwnd, PROP_UNDERLINE_FONT);
            SendMessage(hwnd, WM_SETFONT, (WPARAM) hFont, false);
            InvalidateRect(hwnd, NULL, false);
            SetCapture(hwnd);
         }
         else
         {
            RECT rect;
            GetWindowRect(hwnd, &rect);

            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            ClientToScreen(hwnd, &pt);

            if (!PtInRect(&rect, pt))
            {
               HFONT hFont = (HFONT) GetProp(hwnd, PROP_ORIGINAL_FONT);
               SendMessage(hwnd, WM_SETFONT, (WPARAM) hFont, false);
               InvalidateRect(hwnd, NULL, false);
               ReleaseCapture();
            }
         }
         break;
      }
   case WM_SETCURSOR:
      {
         // Since IDC_HAND is not available on all operating systems,
         // we will load the arrow cursor if IDC_HAND is not present.
         // HCURSOR hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));
         HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
         if (NULL == hCursor)
         {
            // hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
            hCursor = LoadCursor(NULL, IDC_ARROW);
         }
         SetCursor(hCursor);
         return true;
      }
   }  //lint !e744

   return CallWindowProc(pfnOrigProc, hwnd, message, wParam, lParam);
}

//*************************************************************************************
BOOL ConvertStaticToHyperlink(HWND hwndCtl)
{
   // Subclass the parent so we can color the controls as we desire.

   HWND hwndParent = GetParent(hwndCtl);
   if (NULL != hwndParent)
   {
      WNDPROC pfnOrigProc = (WNDPROC) GetWindowLongIF(hwndParent, GWx_WNDPROC);
      if (pfnOrigProc != _HyperlinkParentProc)
      {
         SetProp(hwndParent, PROP_ORIGINAL_PROC, (HANDLE) pfnOrigProc); //lint !e611
         SetWindowLong(hwndParent, GWx_WNDPROC, (LONGx) (WNDPROC) _HyperlinkParentProc);
      }
   }

   // Make sure the control will send notifications.

   DWORD dwStyle = GetWindowLongIF(hwndCtl, GWL_STYLE);
   SetWindowLong(hwndCtl, GWL_STYLE, dwStyle | SS_NOTIFY);

   // Subclass the existing control.

   WNDPROC pfnOrigProc = (WNDPROC) GetWindowLongIF(hwndCtl, GWx_WNDPROC);
   SetProp(hwndCtl, PROP_ORIGINAL_PROC, (HANDLE) pfnOrigProc); //lint !e611
   SetWindowLongIF(hwndCtl, GWx_WNDPROC, (LONGx) (WNDPROC) _HyperlinkProc);

   // Create an updated font by adding an underline.

   HFONT hOrigFont = (HFONT) SendMessage(hwndCtl, WM_GETFONT, 0, 0);
   SetProp(hwndCtl, PROP_ORIGINAL_FONT, (HANDLE) hOrigFont);

   LOGFONT lf;
   GetObject(hOrigFont, sizeof(lf), &lf);
   lf.lfUnderline = true;

   HFONT hFont = CreateFontIndirect(&lf);
   SetProp(hwndCtl, PROP_UNDERLINE_FONT, (HANDLE) hFont);

   // Set a flag on the control so we know what color it should be.

   SetProp(hwndCtl, PROP_STATIC_HYPERLINK, (HANDLE) 1);

   return true;
}

//*************************************************************************************
BOOL ConvertStaticToHyperlink(HWND hwndParent, UINT uiCtlId)
{
   return ConvertStaticToHyperlink(GetDlgItem(hwndParent, uiCtlId));
}
