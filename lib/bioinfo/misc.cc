#include "misc.h"

/** 
 * Reverse the order of the bytes. 
 */
void reverse_bytes(char* bytes, long length) {
  long half_len = (length >> 1);
  char* end = bytes + length;
  char c;
  while (--half_len >= 0) {
    c = *bytes;
    *bytes++ = *--end;
    *end = c;
  }
}

/** 
 * Allocate a new buffer of given size, and copy pt to it. 
 */
void* clone_mem(void* pt, size_t size) {
  void *new_pt = malloc(size);
  memcpy(new_pt, pt, size);
  return new_pt;
}
/* vim: set ai ts=2 sts=2 sw=2 et: */
