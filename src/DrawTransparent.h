/*
  DrawTransparent.h
*/

#ifndef __DrawTransparent_H__
#define __DrawTransparent_H__

#include <windows.h>

typedef int (*drawTransparentCallback)(HDC, int, int, COLORREF);
int drawTransparent(HDC hdc, int x, int y, int w, int h,
  HDC hmdc, int szx, int szy,
  COLORREF transparent_color, drawTransparentCallback callback);

#endif // __DrawTransparent_H__
