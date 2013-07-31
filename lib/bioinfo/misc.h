#ifndef BIOINFO_MISC_H__
#define BIOINFO_MISC_H__

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>

#include "linestream.h"
#include "worditer.h"

#define NUMELE(a) ((int)sizeof(a)/sizeof(a[0]))

void reverse_bytes(char* bytes, long length);

void* clone_mem(void *pt, size_t size);

/* --------- getLine(): like gets(), but with arbitrary length lines ------
  usage:
    static char *line = 0 ;
    static int buflen ;
    int len ;
    while (getLine(stdin, &line, &buflen)) { 
      len = stripNlGetLength(line) ;
      printf("%d %s\n", len, line) ;
    }
    hlr_free(line) ;
  notes: 
   - 'buflen' is not the length of the returned line
   - getLine() does not strip the trailing newline
   - 'line' and 'buflen' belong tightly together
*/

int getLine(FILE *stream, char **buffer, int *buflen);
int stripNlGetLength(char *line);

struct TableRow {
  std::vector<std::string> columns;
};

std::vector<std::string> read_list(const char* filename);

std::vector<TableRow> read_table(const char* filename, const char* delimiter);

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_MISC_H__ */
