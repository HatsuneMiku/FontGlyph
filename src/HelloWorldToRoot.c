/*
  HelloWorldToRoot.c

  cl kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib
    HelloWorldToRoot.c
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
  drawStrokes(&di, RGB(0, 0, 127), "Hello,\nworld!");
  DeleteObject(SelectObject(hdc, hobrush));
  DeleteObject(SelectObject(hdc, hopen));
  return 0;
}

int main(int ac, char **av)
{
  int result = 0;
#if 0
  if(ac >= 2) strncpy(fontfilename, av[1], sizeof(fontfilename));
#else
  int i, argc = 0;
  LPTSTR *argv = parseCommandLine(GetCommandLine(), &argc);
  if(argv){
    if(argc >= 2) strncpy(fontfilename, argv[1], sizeof(fontfilename));
#if 0
    for(i = 0; i < argc; ++i) fprintf(stdout, "argv[%2d]: %s\n", i, argv[i]);
#endif
    freeArgv(argv);
  }
#endif
  fprintf(stdout, "ttf: %s\n", fontfilename[0] ? fontfilename : "none");
  if(result = initGlyph(&ttf, fontfilename)){
    fprintf(stderr, "glyph error %d", result);
  }else if(!checkVersion(4, 0, 3)){
    fprintf(stderr, "version error");
    result = 255;
  }
  if(!result){
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetDC(hwnd);
    RECT rc;
    GetClientRect(hwnd, &rc);
    {
      int szx = rc.right - rc.left, szy = rc.bottom - rc.top;
      HDC hmdc = CreateCompatibleDC(hdc);
      HBITMAP hbmp = CreateCompatibleBitmap(hdc, szx, szy);
      HBITMAP hobmp = (HBITMAP)SelectObject(hmdc, hbmp);
      BitBlt(hmdc, 0, 0, szx, szy, hdc, 0, 0, SRCCOPY); // for Aero
      drawTransparent(hdc, 0, 0, szx, szy, hmdc, szx, szy,
        RGB(255, 255, 0), drawFunc);
      DeleteObject(SelectObject(hmdc, hobmp));
      DeleteDC(hmdc);
    }
    ReleaseDC(hwnd, hdc);
  }
  if(ttf.loca) free(ttf.loca);
  return result;
}
