/*
 * Commonly used routines.
 *
 * This file is copyright 2002 Jim Kent, but license is hereby
 * granted for all use - public, private or commercial. 
 */

/** 
 *   \file common.c Commonly used routines.
 *   \author Adapted by Lukas Habegger (lukas.habegger@yale.edu)
 */

#include "common.h"

/****************************************************************************************
*  Memory functions
****************************************************************************************/

/* 128*8*1024*1024 == 1073741824 == 2^30 on 32 bit machines,size_t == 4 bytes*/
/* on 64 bit machines, size_t = 8 bytes, 2^30 * 2 * 2 = 2^32 == 4 Gb */
static size_t maxAlloc = 128*8*1024*1024*(sizeof(size_t)/4)*(sizeof(size_t)/4);

/** 
 * Request a block of memory.
 * Need mem calls abort if the memory allocation fails. The memory is initialized to zero.
 */
void *needMem (size_t size)
{
  void *pt;
  pt = hlr_malloc (size);
  memset(pt, 0, size);
  return pt;
}

/** 
 * Request a large block of memory.
 * This calls abort if the memory allocation fails. The memory is not initialized to zero. 
 */
void *needLargeMem (size_t size)
{
  void *pt;
  pt = hlr_malloc (size);
  return pt;
}

/**
 * Request a large block of memory and zero it. 
 */
void *needLargeZeroedMem (size_t size)
{
  void *v;
  v = needLargeMem(size);
  memset(v, 0, size);
  return v;
}

/** 
 * Adjust memory size on a block, possibly relocating it.  If vp is NULL,
 * a new memory block is allocated.  Memory not initted. 
*/
void *needLargeMemResize (void* vp, size_t size)
{
  void *pt;
  if (size == 0 || size >= maxAlloc)
    die ((char*)"needLargeMemResize: trying to allocate %llu bytes (limit: %llu)",
         (unsigned long long)size, (unsigned long long)maxAlloc);
  if ((pt = hlr_realloc(vp, size)) == NULL)
    die ((char*)"needLargeMemResize: Out of memory - request size %llu bytes\n",
	 (unsigned long long)size);
  return pt;
}

/** 
 * Adjust memory size on a block, possibly relocating it.  If vp is NULL, a
 * new memory block is allocated.  If block is grown, new memory is zeroed. 
 */
void *needLargeZeroedMemResize (void* vp, size_t oldSize, size_t newSize)
{
  void *v = needLargeMemResize(vp, newSize);
  if (newSize > oldSize)
    memset(((char*)v)+oldSize, 0, newSize-oldSize);
  return v;
}

/** 
 * Allocate a new buffer of given size, and copy pt to it. 
 */
void *cloneMem (void *pt, size_t size)
{
  void *newPt = needLargeMem(size);
  memcpy(newPt, pt, size);
  return newPt;
}

/**
 * Free memory will check for null before freeing. 
 */
void freeMem (void *pt)
{
  if (pt != NULL) 
    {
      hlr_free(pt);
    }
}

/**
 * Free pointer and set it to NULL.
 * s = needMem(1024); ... freez(&s);
 */
void freez (void *vpt)
{
  void **ppt = (void **)vpt;
  void *pt = *ppt;
  *ppt = NULL;
  freeMem(pt);
}


/****************************************************************************************
* Other Functions
****************************************************************************************/
/**
 * Fill a specified area of memory with zeroes. 
 */
void zeroBytes (void *vpt, int count)
{
  char *pt = (char*)vpt;
  while (--count>=0)
    *pt++=0;
}

/** 
 * Reverse the order of the bytes. 
 */
void reverseBytes (char *bytes, long length)
{
  long halfLen = (length>>1);
  char *end = bytes+length;
  char c;
  while (--halfLen >= 0)
    {
      c = *bytes;
      *bytes++ = *--end;
      *end = c;
    }
}

