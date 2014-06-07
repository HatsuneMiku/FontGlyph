/*
  MetaObject.h
*/

#ifndef __MetaObject_H__
#define __MetaObject_H__

#include <windows.h>
#include <stdio.h>

#pragma pack(push, 2)

typedef struct _Fields_DEF {
  char *typ;
  char *fieldname;
  int location, elemLen, numElems;
} Fields_DEF;

typedef struct _Methods_DEF {
  int (*method)(void);
  char *methodname;
} Methods_DEF;

struct _MetaObject;

typedef struct _Obj {
  struct _MetaObject *meta;
  int sz;
  BYTE *fields;
} Obj;

typedef struct _MetaObject {
  struct _MetaObject *super;
  char *classname;
  Fields_DEF *fields_def;
  Methods_DEF *methods_def;
  char *(*isa)(Obj *obj); // This must be at first position of functions.
  short (*b2h)(BYTE *b);
  long (*b2l)(BYTE *b);
  long long (*b2q)(BYTE *b);
  int (*calcLocation)(Fields_DEF *fields_def);
  int (*create)(Obj *obj, struct _MetaObject *meta);
  int (*set)(Obj *obj, char *attr, void *v);
  int (*get)(Obj *obj, char *attr, void *r);
  int (*method)(Obj *obj, char *attr, void *v, void *r);
  int (*adjustEndian)(Obj *obj);
  int (*restore)(Obj *obj, FILE *fp);
  int (*dump)(Obj *obj, int mode, int idx);
  int (*debug)(int mode, char *fmt, ...);
} MetaObject;

#pragma pack(pop)

MetaObject Object;

#endif // __MetaObject_H__
