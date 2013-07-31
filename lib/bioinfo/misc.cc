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

#define GETLINE_MIN 80
#define GETLINE_INC 1024

/**
 * Read an arbitrary long line from stream, functionality analog to gets(3S).
   \verbatim
   usage example:
     char *line = NULL ;
     int buflen ;
     while (getLine(stdin, &line, &buflen)) { 
       printf("%s", line) ;
     }
     hlr_free(line) ;
   \endverbatim
 * @param[in] stream
 * @param[in] buffer Pointer to a string
 * @param[in] buflen Current length of buffer, for interal use of getLine()
 * @param[out] buffer Might be re-alloated
 * @param[out] buflen Adjusted
 * @return Number of chars put into buffer, including trailing \n (except for the last line), 0 if EOF encountered
 * @post Next call to getLine gives next line
 */
int getLine(FILE *stream, char **buffer, int *buflen) { 
  int buffree ;
  char *startp ;
  char *bufp ;
  int c ;

  if (!buffer) {
    return 0;
  }
  if (!*buffer) {
    *buflen = GETLINE_MIN ;
    *buffer = (char*) malloc(*buflen) ;
  }
  buffree = (*buflen) -1 ;
  bufp = *buffer ;
  startp = bufp ;

  while ((c = getc(stream)) != EOF) {
    if (! --buffree) {
      *buflen = *buflen + GETLINE_INC ;
      buffree += GETLINE_INC ;
      *buffer = (char*) realloc(*buffer, *buflen) ;
      if (! *buffer) {
        return 0;
      }
      bufp = *buffer + (bufp - startp) ;  /* adjust to new location */
      startp = *buffer ;
    }

    if (!c) {
      fprintf(stderr, "getLine: read a NULL character") ; /* warn on binary data */
    }
    *(bufp++) = c ;          /* append char to buffer */
    if (c == '\n') {
      break ;
    }
  }

  *bufp = '\0' ;
  return bufp - startp ;
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
