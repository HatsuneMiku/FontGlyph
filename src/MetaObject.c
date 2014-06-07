/*
  MetaObject.c

  cl MetaObject.c
*/

#include "MetaObject.h"

#include <string.h>
#include <stdarg.h>

static char *isa(Obj *obj)
{
  static char buf[4096];
  sprintf(buf, "<%08x#%08x:%s>", obj, obj->meta, obj->meta->classname);
  return buf;
}

static short b2h(BYTE *b)
{
  return (short)((b[0] << 8) | b[1]);
}

static long b2l(BYTE *b)
{
  return (long)((b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3]);
}

static long long b2q(BYTE *b)
{
  unsigned long long q0, q1;
  q0 = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
  q1 = (b[4] << 24) | (b[5] << 16) | (b[6] << 8) | b[7];
  return (long long)(q0 * 0x100000000LL | q1); // not use ( << 32 )
}

static int calcLocation(Fields_DEF *fields_def)
{
  int loc = 0;
  Fields_DEF *p;
  if(!fields_def) return 0;
  for(p = fields_def; p->typ != NULL; ++p){
    int e = 0, n = 1, len = strlen(p->typ);
    char *s = p->typ, *t = &p->typ[len - 1];
    p->location = loc;
    switch(*t){
    case 'Q': case 'q': e = 8; break;
    case 'L': case 'l': e = 4; break;
    case 'H': case 'h': e = 2; break;
    case 'B': case 'b': e = 1; break;
    case 'C': case 'c': e = 1; break;
    case 'S': case 's': e = 1; break;
    default: e = 0; break;
    }
    if(*s == '>' || *s == '<') ++s;
    if(s != t){
      char u = *t;
      *t = '\0';
      sscanf(s, "%d", &n);
      *t = u;
    }
    loc += (p->elemLen = e) * (p->numElems = n);
  }
  return loc; // total size
}

static int create(Obj *obj, struct _MetaObject *meta)
{
  if(!meta) meta = &Object;
  else if(meta != &Object && !meta->super) meta->super = &Object;
  if(meta != &Object){
    size_t offset = (BYTE *)&meta->super->isa - (BYTE *)meta->super;
    int i, (**func)(), (**over)(), (**ref)();
    func = (int (**)())((BYTE *)meta + offset);
    over = (int (**)())((BYTE *)meta + sizeof(MetaObject));
    ref = (int (**)())((BYTE *)meta->super + offset);
    for(i = 0; i < over - func; ++i) if(!func[i]) func[i] = ref[i];
  }
  obj->meta = meta;
  obj->sz = calcLocation(meta->fields_def);
  obj->fields = (BYTE *)obj + sizeof(Obj);
  memset(obj->fields, 0, obj->sz);
  return obj->sz;
}

static int set(Obj *obj, char *attr, void *v)
{
  return 1;
}

static int get(Obj *obj, char *attr, void *r)
{
  return 1;
}

static int method(Obj *obj, char *attr, void *v, void *r)
{
  return 1;
}

static int adjustEndian(Obj *obj)
{
  Fields_DEF *p;
  if(!obj->meta->fields_def) return 1;
  for(p = obj->meta->fields_def; p->typ != NULL; ++p){
    char t = p->typ[strlen(p->typ) - 1];
    if(t == 'Q' || t == 'q' || t == 'L' || t == 'l' || t == 'H' || t == 'h'){
      BYTE *src = obj->fields + p->location;
      int n;
      for(n = 0; n < p->numElems; ++n, src += p->elemLen)
        switch(t){
        case 'Q': case 'q': *(long long *)src = b2q(src); break;
        case 'L': case 'l': *(long *)src = b2l(src); break;
        case 'H': case 'h': *(short *)src = b2h(src); break;
        default: break;
        }
    }
  }
  return 1;
}

static int restore(Obj *obj, FILE *fp)
{
  if(fread(obj->fields, 1, obj->sz, fp) != obj->sz) return 0;
  return adjustEndian(obj);
}

// mode 0: single object, 1: object array (all fields in an object / line)
// mode 0: idx = elements / line, 1: idx = No. of object in array
static int dump(Obj *obj, int mode, int idx)
{
  int i;
  Object.debug(1, NULL);
  if(!mode) Object.debug(2, "%s\n", Object.isa(obj));
  else Object.debug(2, " %02d: %s", idx, Object.isa(obj));
#if 0
  for(i = 0; i < obj->sz; ++i) Object.debug(2, " %02x", obj->fields[i]);
  Object.debug(2, "\n");
#else
  if(obj->meta->fields_def){
    Fields_DEF *p;
    for(p = obj->meta->fields_def; p->typ != NULL; ++p){
      char b[4096];
      int s = 0, w = idx ? 32 / idx : 1;
      char t = p->typ[strlen(p->typ) - 1];
      BYTE *src = obj->fields + p->location;
      if(!mode) Object.debug(2, " %s: ", p->fieldname);
      else Object.debug(2, " ");
      if(!mode && idx) Object.debug(2, "\n");
      b[0] = '\0';
      for(i = 0; i < p->numElems; ++i, src += p->elemLen){
        if(!mode && idx && !(i % w)) s+=sprintf(b+s, "%08x: ", i);
        switch(t){
        case 'Q': case 'q': s+=sprintf(b+s, "%08lx", *(LONGLONG *)src); break;
        case 'L': case 'l': s+=sprintf(b+s, "%08x", *(DWORD *)src); break;
        case 'H': case 'h': s+=sprintf(b+s, "%04x", *(WORD *)src); break;
        case 'B': case 'b': s+=sprintf(b+s, "%02x", *(BYTE *)src); break;
        case 'C': case 'c': // through down
        case 'S': case 's': s+=sprintf(b+s, "%c", *(char *)src); break;
        default: break;
        }
        if(!mode && idx)
          if((i + 1) % w) s+=sprintf(b+s, " ");
          else{ Object.debug(2, "%s\n", b); b[0] = '\0'; s = 0; }
      }
      if(!mode && (!idx || i % w)) s+=sprintf(b+s, "\n");
      Object.debug(2, b);
    }
    if(mode) Object.debug(2, "\n");
  }
#endif
  Object.debug(3, NULL);
  return 1;
}

// mode 0: auto open / close, 1: open, 2: append, 3: close
static int debug(int mode, char *fmt, ...)
{
  static int flag = 0;
  static char *logfile = "MetaObject.log";
  static FILE *fp = NULL;
  int result = 1;
  va_list args;
  va_start(args, fmt);
  if(!flag) if(fp = fopen(logfile, "wb")){ fclose(fp); flag = 1; }
  if(mode <= 1 && !(fp = fopen(logfile, "ab"))) result = 0;
  if((!mode || mode == 2) && fp && fmt) vfprintf(fp, fmt, args);
  if((!mode || mode == 3) && fp){ fclose(fp); fp = NULL; }
  va_end(args);
  return result;
}

MetaObject Object = {NULL, "MetaObject", NULL, NULL, isa, b2h, b2l, b2q,
  calcLocation, create, set, get, method, adjustEndian, restore, dump, debug};
