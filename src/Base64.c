/*
  Base64.c

  cl Base64.c
*/

#include "Base64.h"

int b64decode(BYTE *dst, size_t *dst_size, BYTE *src, int src_size)
{
  static BYTE b64o[80] = {
    62,  0, 62,  0, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,
     0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
     0,  0,  0, 62, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};
  size_t i, words = src_size/4, oversize = *dst_size+2, lasti = oversize/3-1;
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
