/*
  HelloWorld.c

  cl kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib
    HelloWorld.c
    ParseCommandLine.c ConvWCS.c CheckVersion.c DrawTransparent.c
    Glyph.c Base64.c MetaObject.c
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ParseCommandLine.h"
#include "CheckVersion.h"
#include "Glyph.h"
#include "DrawTransparent.h"

char *TITLE = "glyf";
char fontfilename[MAX_PATH + 1] = {0};
TTFont ttf;

int drawFunc(HDC hdc, int szx, int szy, COLORREF transparent_color)
{
  DrawInfo di = {hdc, szx, szy, 20, szy - 200, 20, 40, 10, &ttf, 1, 0};
  HPEN hpen = CreatePen(PS_SOLID, 1, transparent_color);
  HPEN hopen = (HPEN)SelectObject(hdc, hpen);
  HBRUSH hbrush = CreateSolidBrush(transparent_color);
  HBRUSH hobrush = (HBRUSH)SelectObject(hdc, hbrush);
  Rectangle(hdc, 0, 0, szx, szy);
  drawStrokes(&di, RGB(199, 199, 199), "Hello,\nworld!");
  DeleteObject(SelectObject(hdc, hobrush));
  DeleteObject(SelectObject(hdc, hopen));
  return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
  switch(msg){
  case WM_CREATE:{
    int result = 0;
    int i, argc = 0;
    LPTSTR *argv = parseCommandLine(GetCommandLine(), &argc);
    if(argv){
      if(argc >= 2) strncpy(fontfilename, argv[1], sizeof(fontfilename));
#if 0
      for(i = 0; i < argc; ++i)
        MessageBox(hwnd, argv[i], TITLE, MB_ICONINFORMATION|IDOK);
#endif
      freeArgv(argv);
    }
    MessageBox(hwnd,
      fontfilename[0] ? fontfilename : "none", TITLE, MB_ICONINFORMATION|IDOK);
    if(result = initGlyph(&ttf, fontfilename)){
      char buf[32];
      sprintf(buf, "glyph error %d", result);
      MessageBox(hwnd, buf, TITLE, MB_ICONSTOP|IDOK);
    }else if(!checkVersion(4, 0, 3)){
      MessageBox(hwnd, "version error", TITLE, MB_ICONSTOP|IDOK);
      result = 255;
    }
    if(result) PostQuitMessage(result);
    }return FALSE;
  case WM_DESTROY:
    PostQuitMessage(0);
    return FALSE;
  case WM_PAINT:{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);
    {
      int szx = rc.right - rc.left, szy = rc.bottom - rc.top;
      HDC hmdc = CreateCompatibleDC(hdc);
      HBITMAP hbmp = CreateCompatibleBitmap(hdc, szx, szy);
      HBITMAP hobmp = (HBITMAP)SelectObject(hmdc, hbmp);
      DrawInfo di = {hdc, szx, szy, 10, 240, 30, 60, 10, &ttf, 1, 0};
      drawStrokes(&di, RGB(183, 183, 183), "Hello,\nworld!");
      BitBlt(hmdc, 0, 0, szx, szy, hdc, 0, 0, SRCCOPY); // for Aero
      drawTransparent(hdc, 0, 0, szx, szy, hmdc, szx, szy,
        RGB(255, 255, 0), drawFunc);
      DeleteObject(SelectObject(hmdc, hobmp));
      DeleteDC(hmdc);
    }
    // InvalidateRect(hwnd, NULL, FALSE);
    EndPaint(hwnd, &ps);
    }return FALSE;
  }
  return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, PSTR cmd, int nCmdShow)
{
  HWND hwnd;
  MSG msg;
  WNDCLASS wc;
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = wc.cbWndExtra = 0;
  wc.hInstance = hInst;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = TITLE;
  if(!RegisterClass(&wc)) return 1;
  if(!(hwnd = CreateWindow(TITLE, TITLE,
    WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    320, 240, 640, 480, // CW_USEDEFAULT, CW_USEDEFAULT
    NULL, NULL, hInst, NULL))) return 2;
  ShowWindow(hwnd, nCmdShow);
  while(GetMessage(&msg, NULL, 0, 0)){
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  if(ttf.loca) free(ttf.loca);
  return msg.wParam;
}
