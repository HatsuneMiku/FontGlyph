/*
  cl user32.lib gdi32.lib test_draw_glyph.c
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define SHOW_MARK 1

char *TITLE = "glyf";
BYTE *gflg = "AQv/8AIYH8k5JMkBA/ACCxeSSSQBA/ABDP/4ARLJ+SACEyDkknySAAIDB/8=";
BYTE *glyf = "AAwASAU7AAD/OgAAAAAC2f0ZAAAAAP0n/zoAAAAABdEAxgAAAAD9uALnAAAAAAJI"
             "AMYAAAAgAGUEYAIb/MkAAAAA/5kAPv9nADb/zwA0/9AAj//QAFYAAAByAAAA5wBb"
             "ADEALAAKAAAAAP8z/6H/2P86/8r/kwAA/uoAAP7IAS0AAAEVAAABEgErAUIA9AAA"
             "AOIAAAD1/vgAAP8N/0kAKv//AJT/bQCi/2oAAP9pAAD/Tf9O//T/fAAEAGwBdwAA"
             "/0QAAAAABhQAvAAAABgAbwRxAi4AAP7v/uj+xP8VAAD/EwAA/ukBPAAAAREAAAER"
             "ARcBPQDtAAAA6wAAARj+w/8+/u8AAADZ/1YA0/9pAAD/ZwAA/1f/LQAA/ycAAP8u"
             "AKr/JwCYAAAAlgAAAKsA1wAEACwCPwEd/ub9cf9uAAAArgKPAA0AdwY2BF3+3fuj"
             "/1IAAP7hA13+4/yj/1MAAP7aBF0AxAAAAM38nwEXA2EAmwAAAR78nwDCA2EAEwBy"
             "A3IDkP/2AAD/1gAK/7EACf/KAAD/qQAA/17/s/+1/8MAAPzn/0QAAAAABF0AvAAA"
             "AAD/WwBwAFoAqwBLAFkAAAAxAAAALP/7ACz/+QAhAGQEQwAA/0QAAAAAAHX/r/+6"
             "/1D/sv+ZAAD/OAAA/xUBNAAAAREAAACOAFEA3gBFAE4ARABMALUAUABhAAAAWAAA"
             "AIj/2wBL/9kAAAHkALwAAP9E+v8AAAKB/7QAIv+IABr/uQAA/2IAAP9Q/yQAAP82"
             "AAD/OQCI/zEAlgAAAFAAAACkAEcACAAhAgMF0f/m+8n/VgAA/+QENwDW+i//NAAA"
             "AAAA0wDMAAA=";
int glyf_len = 0;

typedef struct _CurvePoint {
  BYTE flg;
  int x, y;
} CurvePoint;

typedef struct _DrawInfo {
  HDC hdc;
  int szx, szy, ox, oy, spx, spy, scale;
} DrawInfo;

int scalex(DrawInfo *di, int x)
{
  return di->ox + x / di->scale;
}

int scaley(DrawInfo *di, int y)
{
  return di->szy - (di->oy + y / di->scale);
}

int b64decode(BYTE *dst, size_t *dst_size, BYTE *src, int src_size)
{
  static BYTE b64o[80] = {
    62,  0, 62,  0, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,
     0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
     0,  0,  0, 62, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};
  int i, words = src_size/4, oversize = *dst_size+2, lasti = oversize/3-1;
  if(src_size % 4) return 1;
  if(*dst_size < words * 3) return 2;
  for(i = 0; i <= lasti; i++){
    int j;
    BYTE c[4];
    for(j = 0; j < sizeof(c); j++)
      c[j] = b64o[(src[i * sizeof(c) + j] - '+') % sizeof(b64o)];
    dst[i * 3 + 0] = ((c[0] << 2) & 0x00fc) | ((c[1] >> 4) & 0x03);
    if(i < lasti || oversize % 3)
      dst[i * 3 + 1] = ((c[1] << 4) & 0x00f0) | ((c[2] >> 2) & 0x0f);
    if(i < lasti || (oversize % 3) == 2)
      dst[i * 3 + 2] = ((c[2] << 6) & 0x00c0) | (c[3] & 0x3f);
  }
  if(src[src_size - 1] == '='){
    --(*dst_size);
    if(src[src_size - 2] == '=') --(*dst_size);
  }
  return 0;
}

int initGlyph(BYTE *str)
{
  int len = (strlen(str) >> 2) * 3;
  if(b64decode(str, &len, str, strlen(str))) return 0;
  return len;
}

short b2h(BYTE *b)
{
  return (short)((b[0] << 8) | b[1]);
}

int getGlyphData(CurvePoint **cp, BYTE **epoc, int *numepoc, char ch)
{
  int i, j, k, pos = 0, num = 0, code = 0, fpos = 0, epnum = 0, fnum = 0;
  *cp = NULL, *epoc = NULL, *numepoc = 0;
  while(pos < glyf_len){
    num = b2h(&glyf[pos]), code = b2h(&glyf[pos + 2]);
    epnum = gflg[fpos], fnum = (num + 7) / 8;
    if(code == ch){
      if(!(*cp = (CurvePoint *)malloc(num * sizeof(CurvePoint)))) break;
      if(!(*epoc = (BYTE *)malloc(*numepoc = epnum))) break;
      for(i = 0; i < epnum; ++i) (*epoc)[i] = gflg[fpos + 1 + i];
      for(i = 0, k = 0; i < fnum; ++i){
        BYTE b = gflg[fpos + 1 + epnum + i];
        BYTE m = 0x80;
        for(j = 0; j < 8 - (i == fnum - 1 ? fnum * 8 - num : 0); ++j, m >>= 1)
          (*cp)[k++].flg = b & m ? 1 : 0;
      }
      for(i = 0; i < num; ++i){
        (*cp)[i].x = (i ? (*cp)[i-1].x : 0) + b2h(&glyf[pos + (i+1)*4]);
        (*cp)[i].y = (i ? (*cp)[i-1].y : 0) + b2h(&glyf[pos + (i+1)*4 + 2]);
      }
      return num;
    }
    pos += (num + 1) * 4;
    fpos += epnum + fnum + 1;
  }
  return 0;
}

int drawGlyph(DrawInfo *di, int *w, int *h, int ch)
{
  CurvePoint *cp;
  BYTE *epoc;
  int numepoc, i, j;
  int numflags = getGlyphData(&cp, &epoc, &numepoc, ch);
  *w = 80, *h = 160;
  if(numflags){
    for(j = 0; j < numepoc; ++j){
      CurvePoint prv;
      int k = j ? epoc[j - 1] + 1 : 0;
      for(i = k; i <= epoc[j]; ++i){
        CurvePoint cur = cp[i];
        if(SHOW_MARK){
          int r = i == k ? 4 : 3;
          COLORREF col = cur.flg & 0x01 ? RGB(0, 0, 255) : RGB(255, 0, 0);
          HPEN hpen = CreatePen(PS_SOLID, 1, col);
          HPEN hopen = (HPEN)SelectObject(di->hdc, hpen);
          HBRUSH hbrush, hobrush;
          if(i == k) hbrush = CreateSolidBrush(RGB(0, 255, 0));
          else hbrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
          hobrush = (HBRUSH)SelectObject(di->hdc, hbrush);
          Ellipse(di->hdc, scalex(di, cur.x) - r, scaley(di, cur.y) + r,
            scalex(di, cur.x) + r, scaley(di, cur.y) - r);
          SelectObject(di->hdc, hobrush);
          if(i == k) DeleteObject(hbrush);
          SelectObject(di->hdc, hopen);
          DeleteObject(hpen);
        }
        if(i == k && !(cur.flg & 0x01))
          MessageBox(NULL, "off curve first", TITLE, MB_ICONEXCLAMATION|IDOK);
        if(i != k)
          if(curve(di, &prv, &cur, &cp[i == epoc[j] ? k : i + 1])) continue;
        prv = cur;
      }
      curve(di, &prv, &cp[k], &cp[k + 1]);
    }
  }
  if(cp) free(cp);
  if(epoc) free(epoc);
  return 0;
}

int bezier(DrawInfo *di, int px, int py, int x, int y, int nx, int ny)
{
#if 0
  int t, m = ((nx - px) * (nx - px) + (ny - py) * (ny - py)) / 5000;
  MoveToEx(di->hdc, scalex(di, px), scaley(di, py), NULL);
  for(t = 1; t < m; ++t){
    float f = t / (float)m;
    float a[] = {(1 - f) * (1 - f), 2 * f * (1 - f), f * f};
    int tx = a[0] * px + a[1] * x + a[2] * nx;
    int ty = a[0] * py + a[1] * y + a[2] * ny;
    LineTo(di->hdc, scalex(di, tx), scaley(di, ty));
  }
  LineTo(di->hdc, scalex(di, nx), scaley(di, ny));
#else
  int i;
  POINT lppt[4];
  lppt[0].x = px, lppt[0].y = py;
  lppt[1].x = px + (x - px) * 2 / 3, lppt[1].y = py + (y - py) * 2 / 3;
  lppt[2].x = lppt[1].x + (nx - px) / 3, lppt[2].y = lppt[1].y + (ny - py) / 3;
  lppt[3].x = nx, lppt[3].y = ny;
  for(i = 0; i < sizeof(lppt) / sizeof(lppt[0]); ++i){
    lppt[i].x = scalex(di, lppt[i].x);
    lppt[i].y = scaley(di, lppt[i].y);
  }
  PolyBezier(di->hdc, lppt, sizeof(lppt) / sizeof(lppt[0]));
#endif
  return 0;
}

int curve(DrawInfo *di, CurvePoint *prv, CurvePoint *cur, CurvePoint *nxt)
{
  if(cur->flg & 0x01){
    if(!(prv->flg & 0x01))
      MessageBox(NULL, "off curve bug", TITLE, MB_ICONEXCLAMATION|IDOK);
    stroke(di, prv->x, prv->y, cur->x, cur->y);
    return 0;
  }
  if(nxt->flg & 0x01){
    bezier(di, prv->x, prv->y, cur->x, cur->y, nxt->x, nxt->y);
    *prv = *nxt;
    return !0;
  }else{
    int mx = (cur->x + nxt->x) / 2, my = (cur->y + nxt->y) / 2;
    bezier(di, prv->x, prv->y, cur->x, cur->y, mx, my);
    prv->flg = 1, prv->x = mx, prv->y = my;
    return !0;
  }
}

int stroke(DrawInfo *di, int xs, int ys, int xe, int ye)
{
  POINT p;
  MoveToEx(di->hdc, scalex(di, xs), scaley(di, ys), &p);
  LineTo(di->hdc, scalex(di, xe), scaley(di, ye));
  return 0;
}

int drawStrokes(DrawInfo *di, char *str)
{
  int ox = di->ox, oy = di->oy;
  COLORREF foreground_color = RGB(0, 255, 0);
  HPEN hpen = CreatePen(PS_SOLID, 1, foreground_color);
  HPEN hopen = (HPEN)SelectObject(di->hdc, hpen);
  HBRUSH hbrush = CreateSolidBrush(foreground_color);
  HBRUSH hobrush = (HBRUSH)SelectObject(di->hdc, hbrush);
  char *p;
  for(p = str; *p; ++p){
    int w, h;
    drawGlyph(di, &w, &h, *p);
    if(*p == '\n') di->ox = ox, di->oy -= h + di->spy;
    else di->ox += w + di->spx;
  }
  DeleteObject(SelectObject(di->hdc, hobrush));
  DeleteObject(SelectObject(di->hdc, hopen));
  return 0;
}

int drawCallback(HDC hdc, int szx, int szy, COLORREF transparent_color)
{
  DrawInfo di = {hdc, szx, szy, 120, szy - 400, 40, 40, 10};
  HPEN hpen = CreatePen(PS_SOLID, 1, transparent_color);
  HPEN hopen = (HPEN)SelectObject(hdc, hpen);
  HBRUSH hbrush = CreateSolidBrush(transparent_color);
  HBRUSH hobrush = (HBRUSH)SelectObject(hdc, hbrush);
  Rectangle(hdc, 0, 0, szx, szy);
  drawStrokes(&di, "Hello,\nworld!");
  DeleteObject(SelectObject(hdc, hobrush));
  DeleteObject(SelectObject(hdc, hopen));
  return 0;
}

int drawTransparent(HDC hdc, int x, int y, int w, int h,
  HDC hmdc, int szx, int szy,
  COLORREF transparent_color, int (*callback)(HDC, int, int, COLORREF))
{
  HDC hndc = CreateCompatibleDC(hmdc);
  HBITMAP hnbmp = CreateBitmap(szx, szy, 1, 1, NULL);
  HBITMAP honbmp = (HBITMAP)SelectObject(hndc, hnbmp);
  HDC hgndc = CreateCompatibleDC(hmdc);
  HBITMAP hgnbmp = CreateCompatibleBitmap(hmdc, szx, szy);
  HBITMAP hognbmp = (HBITMAP)SelectObject(hgndc, hgnbmp);
  HDC hpdc = CreateCompatibleDC(hmdc);
  HBITMAP hpbmp = CreateBitmap(szx, szy, 1, 1, NULL);
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

int main(int ac, char **av)
{
  if(!initGlyph(gflg)) fprintf(stderr, "error 1");
  if(!(glyf_len = initGlyph(glyf))) fprintf(stderr, "error 2");
  {
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
        RGB(255, 255, 0), drawCallback);
      DeleteObject(SelectObject(hmdc, hobmp));
      DeleteDC(hmdc);
    }
    ReleaseDC(hwnd, hdc);
  }
  return 0;
}