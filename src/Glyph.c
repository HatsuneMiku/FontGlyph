/*
  Glyph.c

  cl Glyph.c
*/

#include "Glyph.h"
#include "Base64.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static BYTE *gflg = \
  "AQv/8AIYH8k5JMkBA/ACCxeSSSQBA/ABDP/4ARLJ+SACEyDkknySAAIDB/8=";
static BYTE *glyf = \
  "AAwASAU7AAD/OgAAAAAC2f0ZAAAAAP0n/zoAAAAABdEAxgAAAAD9uALnAAAAAAJI"
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
static int glyf_len = 0;

static Fields_DEF TTFInfo_DEF[] = {
  {">L", "sfntVersion"},
  {">H", "numTables"}, {">H", "searchRange"},
  {">H", "entrySelector"}, {">H", "rangeShift"},
  {NULL, NULL}};
static MetaObject _TTFInfo = {&Object, "TTFInfo", TTFInfo_DEF, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static Fields_DEF TTFTag_DEF[] = {
  {">4s", "tag"},
  {">L", "checkSum"}, {">L", "offset"}, {">L", "length"},
  {NULL, NULL}};
static MetaObject _TTFTag = {&Object, "TTFTag", TTFTag_DEF, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static Fields_DEF TTFCmap_DEF[] = {
  {">H", "tableVersion"}, {">H", "numEncodingTables"},
  {NULL, NULL}};
static MetaObject _TTFCmap = {&Object, "TTFCmap", TTFCmap_DEF, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static Fields_DEF TTFCmapEncodingTable_DEF[] = {
  {">H", "platformId"}, {">H", "specificEncordingId"}, {">L", "offset"},
  {NULL, NULL}};
static MetaObject _TTFCmapEncodingTable = {&Object,
  "TTFCmapEncodingTable", TTFCmapEncodingTable_DEF, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static Fields_DEF TTFCmapSubTableFormat_DEF[] = {
  {">H", "format"}, {">H", "length"}, {">H", "version"},
  {NULL, NULL}};
static MetaObject _TTFCmapSubTableFormat = {&Object,
  "TTFCmapSubTableFormat", TTFCmapSubTableFormat_DEF, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static Fields_DEF TTFCmapSTF0_DEF[] = {
  {">256B", "glyphIdArray"},
  {NULL, NULL}};
static MetaObject _TTFCmapSTF0 = {&Object,
  "TTFCmapSTF0", TTFCmapSTF0_DEF, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static Fields_DEF TTFMaxp_DEF[] = {
  {">L", "tableVersion"}, {">H", "numGlyphs"},
  {NULL, NULL}};
static MetaObject _TTFMaxp = {&Object, "TTFMaxp", TTFMaxp_DEF, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static Fields_DEF TTFMaxpAppendix_DEF[] = {
  {">H", "maxPoints"}, {">H", "maxContours"},
  {">H", "maxCompositePoints"}, {">H", "maxCompositeContours"},
  // {">H", "maxZones"},
  {">H", "maxTwilightPoints"}, {">H", "maxStorage"},
  {">H", "maxFunctionDefs"}, {">H", "maxInstructionDefs"},
  {">H", "maxStackElements"}, {">H", "maxSizeOfInstructions"},
  {">H", "maxComponentElements"}, {">H", "maxComponentDepth"},
  {NULL, NULL}};
static MetaObject _TTFMaxpAppendix = {&Object,
  "TTFMaxpAppendix", TTFMaxpAppendix_DEF, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static Fields_DEF TTFHead_DEF[] = {
  {">L", "tableVersion"}, {">L", "fontRevision"},
  {">L", "checkSumAdjustment"}, {">L", "magicNumber"},
  {">H", "flags"}, {">H", "unitsPerEm"}, {">Q", "created"}, {">Q", "modified"},
  {">H", "xMin"}, {">H", "yMin"}, {">H", "xMax"}, {">H", "yMax"}, // hhhh
  {">H", "macStyle"}, {">H", "lowestRecPPEM"}, {">h", "fontDirectionHint"},
  {">h", "indexToLocFormat"}, {">h", "glyphDataFormat"},
  {NULL, NULL}};
static MetaObject _TTFHead = {&Object, "TTFHead", TTFHead_DEF, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static Fields_DEF TTFLocaDynamic_DEF[] = {
  {">99999999W", "glyphOffsets"},
  {NULL, NULL}};
static MetaObject _TTFLocaDynamic = {&Object,
  "TTFLocaDynamic", TTFLocaDynamic_DEF, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static Fields_DEF TTFGlyfHead_DEF[] = {
  {"h", "numContours"},
  {"h", "xMin"}, {"h", "yMin"}, {"h", "xMax"}, {"h", "yMax"},
  {NULL, NULL}};
static MetaObject _TTFGlyfHead = {&Object,
  "TTFGlyfHead", TTFGlyfHead_DEF, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static int decodeData(BYTE *str)
{
  int len = (strlen(str) >> 2) * 3;
  if(b64decode(str, &len, str, strlen(str))) return 0;
  return len;
}

static int procCmap(TTFont *ttf, int idx, FILE *fp)
{
  if(!Object.create((Obj *)&ttf->cmap, &_TTFCmap)) return 1;
  if(!Object.restore((Obj *)&ttf->cmap, fp)) return 2;
  Object.dump((Obj *)&ttf->cmap, 0, 0);
  if(ttf->cmap.numEncodingTables > sizeof(ttf->cet) / sizeof(ttf->cet[0])){
    Object.debug(0, "out of pre allocated range (numEncodingTables/cet)");
    return 3;
  }else{
    int i;
    for(i = 0; i < ttf->cmap.numEncodingTables; ++i){
      fseek(fp, ttf->tags[idx].offset + 4 + 8 * i, SEEK_SET); // 2H + (2HL * i)
      if(!Object.create((Obj *)&ttf->cet[i], &_TTFCmapEncodingTable)) return 4;
      if(!Object.restore((Obj *)&ttf->cet[i], fp)) return 5;
      Object.debug(0, "--encoding table %02d:\n", i);
      Object.dump((Obj *)&ttf->cet[i], 0, 0);
      fseek(fp, ttf->tags[idx].offset + ttf->cet[i].offset, SEEK_SET);
      if(!Object.create((Obj *)&ttf->cstf[i], &_TTFCmapSubTableFormat))
        return 6;
      if(!Object.restore((Obj *)&ttf->cstf[i], fp)) return 7;
      Object.debug(0, "---format: %d length: %d version: %d\n",
        ttf->cstf[i].format, ttf->cstf[i].length, ttf->cstf[i].version);
      switch(ttf->cstf[i].format){
      case 0:{
        if(!Object.create((Obj *)&ttf->fmt0, &_TTFCmapSTF0)) return 8;
        if(!Object.restore((Obj *)&ttf->fmt0, fp)) return 9;
        Object.dump((Obj *)&ttf->fmt0, 0, 2);
        }break;
      case 2:{
        Object.debug(0, "not supported format (%d)\n", ttf->cstf[i].format);
        }break;
      case 4:{
        Object.debug(0, "not supported format (%d)\n", ttf->cstf[i].format);
        }break;
      case 6:{
        Object.debug(0, "not supported format (%d)\n", ttf->cstf[i].format);
        }break;
      case 8:{
        Object.debug(0, "not supported format (%d)\n", ttf->cstf[i].format);
        }break;
      case 10:{
        Object.debug(0, "not supported format (%d)\n", ttf->cstf[i].format);
        }break;
      case 12:{
        Object.debug(0, "not supported format (%d)\n", ttf->cstf[i].format);
        }break;
      case 14:{
        Object.debug(0, "not supported format (%d)\n", ttf->cstf[i].format);
        }break;
      default:
        Object.debug(0, "\nbad cstf.format (%d)\n", ttf->cstf[i].format);
        break;
      }
    }
  }
  return 0;
}

static int procMaxp(TTFont *ttf, int idx, FILE *fp)
{
  if(!Object.create((Obj *)&ttf->maxp, &_TTFMaxp)) return 1;
  if(!Object.restore((Obj *)&ttf->maxp, fp)) return 2;
  Object.dump((Obj *)&ttf->maxp, 0, 0);
  if(ttf->maxp.tableVersion != 0x00000005){
    if(!Object.create((Obj *)&ttf->maxp_appendix, &_TTFMaxpAppendix)) return 1;
    if(!Object.restore((Obj *)&ttf->maxp_appendix, fp)) return 2;
    Object.dump((Obj *)&ttf->maxp_appendix, 0, 0);
  }
  return 0;
}

static int procHead(TTFont *ttf, int idx, FILE *fp)
{
  if(!Object.create((Obj *)&ttf->head, &_TTFHead)) return 1;
  if(!Object.restore((Obj *)&ttf->head, fp)) return 2;
  Object.dump((Obj *)&ttf->head, 0, 0);
  return 0;
}

static int procLoca(TTFont *ttf, int idx, FILE *fp)
{
  char fmt[32];
  int dw, sz;
  if(!ttf->fmt0.obj.sz) return 1;
  sprintf(TTFLocaDynamic_DEF[0].typ, ">%d%c",
    ttf->maxp.numGlyphs + 1, ttf->head.indexToLocFormat ? 'L' : 'H');
  dw = ttf->head.indexToLocFormat ? sizeof(DWORD) : sizeof(WORD);
  sz = (ttf->maxp.numGlyphs + 1) * dw;
  if(!(ttf->loca = (TTFLocaDynamic *)malloc(sizeof(TTFLocaDynamic) + sz)))
    return 2;
  if(!Object.create((Obj *)ttf->loca, &_TTFLocaDynamic)) return 3;
  if(!Object.restore((Obj *)ttf->loca, fp)) return 4;
  Object.dump((Obj *)ttf->loca, 0, dw * 2);
  return 0;
}

static int procGlyf(TTFont *ttf, int idx, FILE *fp)
{
  ttf->glyfOffset = ttf->tags[idx].offset;
  return 0;
}

int initGlyph(TTFont *ttf, char *fontfilename)
{
#define close_done(r) do{ \
  Object.debug(0, "error: %d\n", result = r); goto done; }while(0)
  int result = 0;
  FILE *fp;
  if(!glyf_len){
    if(!decodeData(gflg)) return 1;
    if(!(glyf_len = decodeData(glyf))) return 2;
  }
  memset(ttf, 0, sizeof(TTFont));
  if(!(ttf->fontfilename = fontfilename)) return 0;
  if(!fontfilename[0]){ ttf->fontfilename = NULL; return 0; }
  if(!(fp = fopen(fontfilename, "rb"))){ ttf->fontfilename = NULL; return 3; }
  if(!Object.create((Obj *)&ttf->info, &_TTFInfo)) close_done(4);
  if(!Object.restore((Obj *)&ttf->info, fp)) close_done(5);
  Object.dump((Obj *)&ttf->info, 0, 0);
  if(ttf->info.numTables > sizeof(ttf->tags) / sizeof(ttf->tags[0])){
    Object.debug(0, "out of pre allocated range (numTables/tags)\n");
    close_done(6);
  }else{
    // This code should be changed to hash instead of linear search.
    Methods_DEF procs[] = {
      {(int (*)(void))procCmap, "cmap"}, {(int (*)(void))procMaxp, "maxp"},
      {(int (*)(void))procHead, "head"}, {(int (*)(void))procLoca, "loca"},
      {(int (*)(void))procGlyf, "glyf"}};
    int i, j;
    Object.debug(0, "tables: (No.: obj, tag, checkSum, offset, length)\n");
    for(i = 0; i < ttf->info.numTables; ++i){
      if(!Object.create((Obj *)&ttf->tags[i], &_TTFTag)) close_done(7);
      if(!Object.restore((Obj *)&ttf->tags[i], fp)) close_done(8);
      Object.dump((Obj *)&ttf->tags[i], 1, i);
    }
    for(i = 0; i < sizeof(procs) / sizeof(procs[0]); ++i)
      for(j = 0; j < ttf->info.numTables; ++j)
        if(!strncmp(ttf->tags[j].tag, procs[i].methodname, 4)){
          int r;
          int (*func)(TTFont *, int idx, FILE *);
          func = (int (*)(TTFont *, int idx, FILE *))procs[i].method;
          Object.debug(0, "tag table No.");
          Object.dump((Obj *)&ttf->tags[j], 1, j);
          fseek(fp, ttf->tags[j].offset, SEEK_SET);
          if(r = func(ttf, j, fp)) close_done(10000 + i * 100 + r);
        }
  }
done:
  fclose(fp);
  return result;
}

static int scalex(DrawInfo *di, int x)
{
  return di->ox + x / di->scale;
}

static int scaley(DrawInfo *di, int y)
{
  return di->szy - (di->oy + y / di->scale);
}

static int stroke(DrawInfo *di, int xs, int ys, int xe, int ye)
{
  if(di->state == 1){
    POINT p;
    MoveToEx(di->hdc, scalex(di, xs), scaley(di, ys), &p);
  }
  LineTo(di->hdc, scalex(di, xe), scaley(di, ye));
  return 0;
}

static int bezier(DrawInfo *di, int px, int py, int x, int y, int nx, int ny)
{
#if 0
  int t, m = ((nx - px) * (nx - px) + (ny - py) * (ny - py)) / 5000;
  if(di->state == 1)
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
  if(di->state == 1)
    MoveToEx(di->hdc, scalex(di, px), scaley(di, py), NULL);
  PolyBezierTo(di->hdc, lppt + 1, (sizeof(lppt) - 1) / sizeof(lppt[0]));
#endif
  return 0;
}

static int curve(DrawInfo *di,
  CurvePoint *prv, CurvePoint *cur, CurvePoint *nxt)
{
  if(cur->flg & 0x01){
    if(!(prv->flg & 0x01)) MessageBox(NULL,
      "off curve bug", di->ttf->fontfilename, MB_ICONEXCLAMATION|IDOK);
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

static int getGlyphData(CurvePoint **cp, BYTE **epoc, int *numepoc, char ch)
{
  int i, j, k, pos = 0, num = 0, code = 0, fpos = 0, epnum = 0, fnum = 0;
  *cp = NULL, *epoc = NULL, *numepoc = 0;
  while(pos < glyf_len){
    num = Object.b2h(&glyf[pos]), code = Object.b2h(&glyf[pos + 2]);
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
        (*cp)[i].x = (i ? (*cp)[i-1].x : 0) + Object.b2h(&glyf[pos+(i+1)*4]);
        (*cp)[i].y = (i ? (*cp)[i-1].y : 0) + Object.b2h(&glyf[pos+(i+1)*4+2]);
      }
      return num;
    }
    pos += (num + 1) * 4;
    fpos += epnum + fnum + 1;
  }
  return 0;
}

static int getGlyfSingle(CurvePoint **cp, BYTE **epoc, int *numepoc,
  TTFont *ttf, TTFGlyfHead *glyfHead, FILE *fp)
{
  int result = 0;
  int sz = (*numepoc = glyfHead->numContours) * sizeof(WORD);
  if(!(*epoc = (BYTE *)malloc(sz))) return 0;
  if(fread(*epoc, 1, sz, fp) != sz) return 0;
  else{
    WORD instrLength;
    int i;
    for(i = 0; i < *numepoc; ++i){
      WORD u = (WORD)Object.b2h((BYTE *)&((WORD *)(*epoc))[i]);
      if(u > 0x0ff) Object.debug(0, "over 0xff in endPtsOfContours[%d]\n", i);
      (*epoc)[i] = (BYTE)(u & 0x0ff);
    }
    if(fread(&instrLength, 1, sizeof(WORD), fp) != sizeof(WORD)) return 0;
    else{
      BYTE *instructions;
      instrLength = (WORD)Object.b2h((BYTE *)&instrLength);
      if(!(instructions = (BYTE *)malloc(instrLength))) return 0;
      if(fread(instructions, 1, instrLength, fp) != instrLength) return 0;
      else{
        int numFlags = (*epoc)[*numepoc - 1] + 1;
        if(*cp = (CurvePoint *)malloc(numFlags * sizeof(CurvePoint))){
          BYTE flg, b;
          int n, r = 0;
          for(i = 0; i < numFlags; ++i){
            if(r > 0) flg = (*cp)[i - 1].flg, --r;
            else
              if(fread(&b, 1, sizeof(BYTE), fp) != sizeof(BYTE)) break;
              else
                if(r == -1) flg = (*cp)[i - 1].flg, r = b - 1;
                else flg = b, r = b & 0x08 ? -1 : 0;
            (*cp)[i].flg = flg;
          }
          for(n = 0; n < 2; ++n){ // n == 0: x, 1: y
            for(i = 0; i < numFlags; ++i){
              int *coords = n ? &(*cp)[i].y : &(*cp)[i].x;
              BYTE p = (*cp)[i].flg & (0x10 << n);
              int w = (*cp)[i].flg & (0x02 << n) ? sizeof(BYTE) : sizeof(WORD);
              short v = 0; // reset all bytes (must be little endian)
              if(w != sizeof(BYTE) && p) v = 0;
              else
                if(fread(&v, 1, w, fp) != w) break; // WORD or first BYTE of v
                else if(w != sizeof(BYTE)) v = Object.b2h((BYTE *)&v);
              if(w == sizeof(BYTE) && !p) v = -v;
              *coords = v;
              if(i) *coords += *(n ? &(*cp)[i - 1].y : &(*cp)[i - 1].x);
            }
          }
          if(TRUE){ // debug information
            char *flgname[] = {
              "YDual/P", "XDual/P", "Repeat", "Y-Short", "X-Short", "On"};
            Object.debug(1, NULL);
            for(i = 0; i < *numepoc; ++i) Object.debug(2, " %04x", (*epoc)[i]);
            Object.debug(2, "\n");
            Object.debug(2, " instructionLength: %d\n", instrLength);
            Object.debug(2, " instructions: ...\n"); // " %02x", instructions
            for(i = 0; i < numFlags; ++i){
              int m = sizeof(flgname) / sizeof(flgname[0]);
              Object.debug(2, " flag[%3d]: ", i);
              for(n = 0; n < m; ++n)
                Object.debug(2, " %8s",
                  (*cp)[i].flg & (1 << (m - 1 - n)) ? flgname[n] : "");
              Object.debug(2, "\n");
            }
            for(i = 0; i < numFlags; ++i)
              Object.debug(2, " coords[%3d]: (%6d, %6d)\n", i,
                (*cp)[i].x, (*cp)[i].y);
            Object.debug(3, NULL);
          }
          result = numFlags;
        }
        free(instructions);
      }
    }
  }
  return result;
}

static int getGlyph(CurvePoint **cp, BYTE **epoc, int *numepoc,
  TTFont *ttf, char ch)
{
  int result = 0;
  *cp = NULL, *epoc = NULL, *numepoc = 0;
  if(!ttf || !ttf->fontfilename || !ttf->loca) return 0;
  else{
    DWORD off0, off1, idx = ttf->fmt0.glyphIdArray[ch];
    if(ttf->head.indexToLocFormat){
      off0 = ttf->loca->u.dwGlyphOffsets[idx];
      off1 = ttf->loca->u.dwGlyphOffsets[idx + 1];
    }else{
      off0 = ttf->loca->u.wGlyphOffsets[idx];
      off1 = ttf->loca->u.wGlyphOffsets[idx + 1];
    }
    if(off0 == off1) return 0;
    else{
      FILE *fp;
      if(!(fp = fopen(ttf->fontfilename, "rb"))) return 0;
      else{
        TTFGlyfHead glyfHead;
        int w = ttf->head.indexToLocFormat ? 1 : 2;
        fseek(fp, ttf->glyfOffset + w * off0, SEEK_SET);
        if(Object.create((Obj *)&glyfHead, &_TTFGlyfHead))
          if(Object.restore((Obj *)&glyfHead, fp)){
            Object.dump((Obj *)&glyfHead, 0, 0);
            if(glyfHead.numContours < 0)
              Object.debug(0, "multi-glyph is not supported\n");
            else result = getGlyfSingle(cp, epoc, numepoc, ttf, &glyfHead, fp);
          }
        fclose(fp);
      }
    }
  }
  return result;
}

static int drawGlyph(DrawInfo *di, int *w, int *h, int ch)
{
  CurvePoint *cp;
  BYTE *epoc;
  int numepoc, i, j;
  int numflags = di->ttf->fontfilename ? \
    getGlyph(&cp, &epoc, &numepoc, di->ttf, ch) : \
    getGlyphData(&cp, &epoc, &numepoc, ch);
  *w = 80, *h = 160;
  if(numflags){
    BeginPath(di->hdc);
    SetBkMode(di->hdc, TRANSPARENT);
    for(j = 0; j < numepoc; ++j){
      CurvePoint prv;
      int k = j ? epoc[j - 1] + 1 : 0;
      for(i = k; i <= epoc[j]; ++i){
        CurvePoint cur = cp[i];
        di->state = i - k;
        if(i == k && !(cur.flg & 0x01)) MessageBox(NULL,
          "off curve first", di->ttf->fontfilename, MB_ICONEXCLAMATION|IDOK);
        if(i != k)
          if(curve(di, &prv, &cur, &cp[i == epoc[j] ? k : i + 1])) continue;
        prv = cur;
      }
      di->state = i - k;
      curve(di, &prv, &cp[k], &cp[k + 1]);
    }
    EndPath(di->hdc);
#if 1
#if 1
#if 1
    // StrokePath(di->hdc);
    FillPath(di->hdc);
#else
    // SelectObject(di->hdc, CreateHatchBrush(HS_DIAGCROSS, 0xFF));
    StrokeAndFillPath(di->hdc);
    // DeleteObject(SelectObject(di->hdc, (HBRUSH)GetStockObject(WHITE_BRUSH)));
#endif
#else
    FillRgn(di->hdc, PathToRegion(di->hdc), (HBRUSH)GetStockObject(BLACK_BRUSH));
#endif
#else
    {
      HDC hmdc = CreateCompatibleDC(di->hdc);
      HBITMAP hbmp = CreateCompatibleBitmap(di->hdc, di->szx, di->szy);
      HBITMAP hobmp = (HBITMAP)SelectObject(hmdc, hbmp);
#if 1
      SelectClipPath(di->hdc, RGN_COPY);
#else
      HRGN hrgn = PathToRegion(di->hdc);
      SelectClipRgn(di->hdc, hrgn);
#endif
      BitBlt(di->hdc, 0, 0, di->szx, di->szy, hmdc, 0, 0, SRCCOPY);
      DeleteObject(SelectObject(hmdc, hobmp));
      DeleteDC(hmdc);
    }
#endif
    if(di->show_mark){
      for(j = 0; j < numepoc; ++j){
        int k = j ? epoc[j - 1] + 1 : 0;
        for(i = k; i <= epoc[j]; ++i){
          CurvePoint cur = cp[i];
          int r = i == k ? 4 : 3;
          COLORREF col = cur.flg & 0x01 ? RGB(0, 0, 255) : RGB(255, 0, 0);
          HPEN hpen = CreatePen(PS_SOLID, 1, col);
          HPEN hopen = (HPEN)SelectObject(di->hdc, hpen);
          HBRUSH hbrush, hobrush;
          if(i == k) hbrush = CreateSolidBrush(RGB(0, 255, 0));
          else hbrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
          hobrush = (HBRUSH)SelectObject(di->hdc, hbrush);
          //MoveToEx(di->hdc, scalex(di, cur.x), scaley(di, cur.y), NULL);
          //AngleArc(di->hdc, scalex(di, cur.x), scaley(di, cur.y), r, 0, 360);
          Ellipse(di->hdc, scalex(di, cur.x) - r, scaley(di, cur.y) + r,
            scalex(di, cur.x) + r, scaley(di, cur.y) - r);
          SelectObject(di->hdc, hobrush);
          if(i == k) DeleteObject(hbrush);
          SelectObject(di->hdc, hopen);
          DeleteObject(hpen);
        }
      }
    }
  }
  if(cp) free(cp);
  if(epoc) free(epoc);
  return 0;
}

int drawStrokes(DrawInfo *di, COLORREF foreground_color, char *str)
{
  int ox = di->ox, oy = di->oy;
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
