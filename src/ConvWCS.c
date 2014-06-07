/*
  ConvWCS.c

  cl ConvWCS.c
*/

#include "ConvWCS.h"
#include <stdlib.h>
#include <string.h>

wchar_t *mbs2wcs(char *mbs)
{
  size_t mblen = strlen(mbs);
  size_t wclen = MultiByteToWideChar(GetACP(), 0, mbs, mblen, NULL, 0);
  wchar_t *wcs = (wchar_t *)malloc((wclen + 1) * sizeof(wchar_t));
  wclen = MultiByteToWideChar(GetACP(), 0, mbs, mblen, wcs, wclen + 1); // + 1
  wcs[wclen] = L'\0';
  return wcs; // wcs *** must be free later ***
}

char *wcs2mbs(wchar_t *wcs)
{
  size_t mblen = WideCharToMultiByte(GetACP(), 0,
    (LPCWSTR)wcs, -1, NULL, 0, NULL, NULL);
  char *mbs = (char *)malloc((mblen + 1));
  mblen = WideCharToMultiByte(GetACP(), 0,
    (LPCWSTR)wcs, -1, mbs, mblen, NULL, NULL); // not + 1
  mbs[mblen] = '\0';
  return mbs; // locale mbs *** must be free later ***
}
