/*
  ParseCommandLine.h
*/

#ifndef __ParseCommandLine_H__
#define __ParseCommandLine_H__

#include <windows.h>

LPTSTR *parseCommandLine(LPTSTR targs, int *argc);
VOID freeArgv(LPTSTR *targv);

#endif // __ParseCommandLine_H__
