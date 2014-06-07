/*
  ParseCommandLine.c
    not for UNICODE (LPTSTR GetCommandLine() returns mbcs)

  cl ParseCommandLine.c
  (link) shell32.lib
*/

#include "ParseCommandLine.h"
#include "ConvWCS.h"
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "shell32.lib")

LPTSTR *parseCommandLine(LPTSTR targs, int *argc)
{
  LPWSTR wargs, *wargv;
  *argc = 0;
#ifdef UNICODE
  if(targs[0] == L'\0') return NULL;
  wargs = targs;
#else
  if(targs[0] == '\0') return NULL;
  if(!(wargs = mbs2wcs(targs))) return NULL;
#endif
  wargv = CommandLineToArgvW(wargs, argc);
#ifdef UNICODE
  return wargv;
#else
  if(wargs) free(wargs);
  if(!wargv) return NULL;
  {
    char *p, *q, *argvs = NULL, **argv = NULL;
    int i, j, len = 0;
    for(i = 0; i < *argc; ++i){
      if(!(p = wcs2mbs(wargv[i]))){ len = 0; *argc = 0; break; }
      len += strlen(p) + 1;
      free(p);
    }
    while(len && (argvs = (char *)malloc(len)) != NULL){
      if(!(argv = (char **)malloc(*argc * sizeof(char *)))){
        free(argvs); *argc = 0; break;
      }
      for(i = j = 0, q = argvs; i < *argc; ++i){
        argv[i] = q + j;
        if(!(p = wcs2mbs(wargv[i]))){ *q++ = '\0'; ++j; continue; }
        strncpy(argv[i], p, len - j);
        j += strlen(p) + 1;
        free(p);
      }
      break;
    }
    LocalFree((HLOCAL)wargv);
    return argv;
  }
#endif
}

VOID freeArgv(LPTSTR *targv)
{
#ifdef UNICODE
  LocalFree((HLOCAL)targv);
#else
  if(targv) free(targv[0]); // targv[0] == argvs
  free(targv);
#endif
}
