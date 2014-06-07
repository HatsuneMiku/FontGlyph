/*
  Glyph.h
*/

#ifndef __Glyph_H__
#define __Glyph_H__

#include <windows.h>
#include "MetaObject.h"

#pragma pack(push, 2)

typedef struct _TTFInfo {
  Obj obj;
  DWORD sfntVersion;
  WORD numTables, searchRange, entrySelector, rangeShift;
} TTFInfo;

typedef struct _TTFTag {
  Obj obj;
  char tag[4];
  DWORD checkSum, offset, length;
} TTFTag;

typedef struct _TTFCmap {
  Obj obj;
  WORD tableVersion, numEncodingTables;
} TTFCmap;

typedef struct _TTFCmapEncodingTable {
  Obj obj;
  WORD platformId, specificEncordingId;
  DWORD offset;
} TTFCmapEncodingTable;

typedef struct _TTFCmapSubTableFormat {
  Obj obj;
  WORD format, length, version;
} TTFCmapSubTableFormat;

typedef struct _TTFCmapSTF0 { // cet.specificEncodingId == 0
  Obj obj;
  BYTE glyphIdArray[256];
} TTFCmapSTF0;

typedef struct _TTFMaxp {
  Obj obj;
  DWORD tableVersion;
  WORD numGlyphs;
} TTFMaxp;

typedef struct _TTFMaxpAppendix {
  Obj obj;
  WORD maxPoints, maxContours, maxCompositePoints, maxCompositeContours;
  // WORD maxZones;
  WORD maxTwilightPoints, maxStorage, maxFunctionDefs, maxInstructionDefs;
  WORD maxStackElements, maxSizeOfInstructions;
  WORD maxComponentElements, maxComponentDepth;
} TTFMaxpAppendix;

typedef struct _TTFHead {
  Obj obj;
  DWORD tableVersion, fontRevision, checkSumAdjustment, magicNumber;
  WORD flags, unitsPerEm;
  LONGLONG created, modified;
  WORD xMin, yMin, xMax, yMax; // short
  WORD macStyle, lowestRecPPEM;
  short fontDirectionHint, indexToLocFormat, glyphDataFormat;
} TTFHead;

typedef struct _TTFLocaDynamic {
  Obj obj;
  union {
    DWORD dwGlyphOffsets[];
    WORD wGlyphOffsets[];
  } u;
} TTFLocaDynamic;

typedef struct _TTFGlyfHead {
  Obj obj;
  short numContours; // positive: single-glyph, negative: multi-glyph
  short xMin, yMin, xMax, yMax;
} TTFGlyfHead;

#pragma pack(pop)

typedef struct _TTFont {
  char *fontfilename;
  TTFInfo info;
  TTFTag tags[64];
  TTFCmap cmap;
  TTFCmapEncodingTable cet[8];
  TTFCmapSubTableFormat cstf[8];
  TTFCmapSTF0 fmt0;
  TTFMaxp maxp;
  TTFMaxpAppendix maxp_appendix;
  TTFHead head;
  TTFLocaDynamic *loca;
  DWORD glyfOffset;
} TTFont;

typedef struct _CurvePoint {
  BYTE flg;
  int x, y;
} CurvePoint;

typedef struct _DrawInfo {
  HDC hdc;
  int szx, szy, ox, oy, spx, spy, scale;
  TTFont *ttf;
  int show_mark, state;
} DrawInfo;

int initGlyph(TTFont *ttf, char *fontfilename);
int drawStrokes(DrawInfo *di, COLORREF foreground_color, char *str);

#endif // __Glyph_H__
