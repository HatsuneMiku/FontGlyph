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

int getGlyphData(BYTE **epoc, BYTE **flags, int **xcoords, int **ycoords,
  int *numepoc, char ch)
{
  int i, j, k, pos = 0, num = 0, code = 0, fpos = 0, epnum = 0, fnum = 0;
  *epoc = *flags = NULL, *xcoords = *ycoords = NULL, *numepoc = 0;
  while(pos < glyf_len){
    num = b2h(&glyf[pos]), code = b2h(&glyf[pos + 2]);
    epnum = gflg[fpos], fnum = (num + 7) / 8;
    if(code == ch){
      if(!(*epoc = (BYTE *)malloc(*numepoc = epnum))) break;
      for(i = 0; i < epnum; ++i) (*epoc)[i] = gflg[fpos + 1 + i];
      if(!(*flags = (BYTE *)malloc(num))) break;
      for(i = 0, k = 0; i < fnum; ++i){
        BYTE b = gflg[fpos + 1 + epnum + i];
        BYTE m = 0x80;
        for(j = 0; j < 8 - (i == fnum - 1 ? fnum * 8 - num : 0); ++j, m >>= 1)
          (*flags)[k++] = b & m ? 1 : 0;
      }
      break;
    }
    pos += (num + 1) * 4;
    fpos += epnum + fnum + 1;
  }
  if(!*epoc || !*flags) return 0;
  if(!(*xcoords = (int *)malloc(num * sizeof(int)))) return 0;
  if(!(*ycoords = (int *)malloc(num * sizeof(int)))) return 0;
  for(i = 0; i < num; ++i){
    (*xcoords)[i] = (i ? (*xcoords)[i-1] : 0) + b2h(&glyf[pos + (i+1)*4]);
    (*ycoords)[i] = (i ? (*ycoords)[i-1] : 0) + b2h(&glyf[pos + (i+1)*4 + 2]);
  }
  return num;
}

int drawGlyph(HDC hdc, int scale, int szx, int szy, int ox, int oy,
  int *w, int *h, int ch)
{
  BYTE *epoc, *flags;
  int *xcoords, *ycoords, numepoc, i, j;
  int numflags = getGlyphData(&epoc, &flags, &xcoords, &ycoords, &numepoc, ch);
  *w = 80, *h = 160;
  if(numflags){
    for(j = 0; j < numepoc; ++j){
      int pflg, px, py, k = j ? epoc[j - 1] + 1 : 0;
      for(i = k; i <= epoc[j]; ++i){
        int flg = flags[i], x = xcoords[i], y = ycoords[i];
        if(SHOW_MARK){
          int r = i == k ? 4 : 3;
          COLORREF col = flg & 0x01 ? RGB(0, 0, 255) : RGB(255, 0, 0);
          HPEN hpen = CreatePen(PS_SOLID, 1, col);
          HPEN hopen = (HPEN)SelectObject(hdc, hpen);
          HBRUSH hbrush, hobrush;
          if(i == k) hbrush = CreateSolidBrush(RGB(0, 255, 0));
          else hbrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
          hobrush = (HBRUSH)SelectObject(hdc, hbrush);
          Ellipse(hdc, ox - r + x / scale, szy - (oy - r + y / scale),
            ox + r + x / scale, szy - (oy + r + y / scale));
          SelectObject(hdc, hobrush);
          if(i == k) DeleteObject(hbrush);
          SelectObject(hdc, hopen);
          DeleteObject(hpen);
        }
        if(i == k && !(flg & 0x01))
          MessageBox(NULL, "off curve first", TITLE, MB_ICONEXCLAMATION|IDOK);
        if(i != k){
          int n = i == epoc[j] ? k : i + 1;
          int nflg = flags[n], nx = xcoords[n], ny = ycoords[n];
          if(curve(hdc, scale, szx, szy, ox, oy,
            &pflg, &px, &py, flg, x, y, nflg, nx, ny)) continue;
        }
        pflg = flg, px = x, py = y;
      }
      curve(hdc, scale, szx, szy, ox, oy, &pflg, &px, &py,
        flags[k], xcoords[k], ycoords[k],
        flags[k + 1], xcoords[k + 1], ycoords[k + 1]);
    }
  }
  if(epoc) free(epoc);
  if(flags) free(flags);
  if(xcoords) free(xcoords);
  if(ycoords) free(ycoords);
  return 0;
}

int bezier(HDC hdc, int scale, int szx, int szy, int ox, int oy,
  int px, int py, int x, int y, int nx, int ny)
{
  int t, m = ((nx - px) * (nx - px) + (ny - py) * (ny - py)) / 5000;
  MoveToEx(hdc, ox + px / scale, szy - (oy + py / scale), NULL);
  for(t = 1; t < m; ++t){
    float f = t / (float)m;
    float a[] = {(1 - f) * (1 - f), 2 * f * (1 - f), f * f};
    int tx = a[0] * px + a[1] * x + a[2] * nx;
    int ty = a[0] * py + a[1] * y + a[2] * ny;
    LineTo(hdc, ox + tx / scale, szy - (oy + ty / scale));
  }
  LineTo(hdc, ox + nx / scale, szy - (oy + ny / scale));
  return 0;
}

int curve(HDC hdc, int scale, int szx, int szy, int ox, int oy,
  int *pflg, int *px, int *py, int flg, int x, int y, int nflg, int nx, int ny)
{
  if(flg & 0x01){
    if(!(*pflg & 0x01))
      MessageBox(NULL, "off curve bug", TITLE, MB_ICONEXCLAMATION|IDOK);
    stroke(hdc, scale, szx, szy, ox, oy, *px, *py, x, y);
    return 0;
  }
  if(nflg & 0x01){
    bezier(hdc, scale, szx, szy, ox, oy, *px, *py, x, y, nx, ny);
    *pflg = nflg, *px = nx, *py = ny;
    return !0;
  }else{
    int mx = (x + nx) / 2, my = (y + ny) / 2;
    bezier(hdc, scale, szx, szy, ox, oy, *px, *py, x, y, mx, my);
    *pflg = 1, *px = mx, *py = my;
    return !0;
  }
}

int stroke(HDC hdc, int scale, int szx, int szy, int ox, int oy,
  int xs, int ys, int xe, int ye)
{
  POINT p;
  MoveToEx(hdc, ox + xs / scale, szy - (oy + ys / scale), &p);
  LineTo(hdc, ox + xe / scale, szy - (oy + ye / scale));
  return 0;
}

int drawStrokes(int scale, int ofx, int ofy, int spx, int spy, char *str)
{
  int szx, szy, ox = ofx, oy = ofy;
  char *p;
  HWND hwnd = GetDesktopWindow();
  HDC hdc = GetDC(hwnd);
  HPEN hpen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
  HPEN hopen = (HPEN)SelectObject(hdc, hpen);
  RECT rc;
  GetClientRect(hwnd, &rc);
  szx = rc.right - rc.left, szy = rc.bottom - rc.top;
  for(p = str; *p; ++p){
    int w, h;
    drawGlyph(hdc, scale, szx, szy, ox, oy, &w, &h, *p);
    if(*p == '\n') ox = ofx, oy -= h + spy;
    else ox += w + spx;
  }
  SelectObject(hdc, hopen);
  DeleteObject(hpen);
  ReleaseDC(hwnd, hdc);
  return 0;
}

int main(int ac, char **av)
{
  if(!initGlyph(gflg)) fprintf(stderr, "error 1");
  if(!(glyf_len = initGlyph(glyf))) fprintf(stderr, "error 2");
  drawStrokes(10, 120, 480, 40, 40, "Hello,\nworld!");
  return 0;
}