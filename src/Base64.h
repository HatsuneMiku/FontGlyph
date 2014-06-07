/*
  Base64.h
*/

#ifndef __Base64_H__
#define __Base64_H__

#include <windows.h>

int b64decode(BYTE *dst, size_t *dst_size, BYTE *src, int src_size);

#endif // __Base64_H__
