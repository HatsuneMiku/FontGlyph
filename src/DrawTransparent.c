/*
  DrawTransparent.c

  cl DrawTransparent.c
*/

#include "DrawTransparent.h"

int drawTransparent(HDC hdc, int x, int y, int w, int h,
  HDC hmdc, int szx, int szy,
  COLORREF transparent_color, drawTransparentCallback callback)
{
  HDC hndc = CreateCompatibleDC(hmdc); // negative mask
  HBITMAP hnbmp = CreateBitmap(szx, szy, 1, 1, NULL); // planes: 1, bits: 1
  HBITMAP honbmp = (HBITMAP)SelectObject(hndc, hnbmp);
  HDC hgndc = CreateCompatibleDC(hmdc); // negative mask (grayscale)
  HBITMAP hgnbmp = CreateCompatibleBitmap(hmdc, szx, szy);
  HBITMAP hognbmp = (HBITMAP)SelectObject(hgndc, hgnbmp);
  HDC hpdc = CreateCompatibleDC(hmdc); // positive mask
  HBITMAP hpbmp = CreateBitmap(szx, szy, 1, 1, NULL); // planes: 1, bits: 1
  HBITMAP hopbmp = (HBITMAP)SelectObject(hpdc, hpbmp);
  COLORREF hocol = SetBkColor(hmdc, transparent_color);
  callback(hmdc, szx, szy, transparent_color);
  BitBlt(hndc, 0, 0, szx, szy, hmdc, 0, 0, SRCCOPY);
  BitBlt(hgndc, 0, 0, szx, szy, hndc, 0, 0, SRCCOPY);
  BitBlt(hpdc, 0, 0, szx, szy, hndc, 0, 0, NOTSRCCOPY);
  SetBkColor(hmdc, hocol);
  BitBlt(hmdc, 0, 0, szx, szy, hpdc, 0, 0, SRCAND);
  BitBlt(hdc, x, y, w, h, hgndc, 0, 0, SRCAND);
  BitBlt(hdc, x, y, w, h, hmdc, 0, 0, SRCPAINT);
  DeleteObject(SelectObject(hpdc, hopbmp));
  DeleteDC(hpdc);
  DeleteObject(SelectObject(hgndc, hognbmp));
  DeleteDC(hgndc);
  DeleteObject(SelectObject(hndc, honbmp));
  DeleteDC(hndc);
  return 0;
}
