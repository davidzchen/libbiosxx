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

int getLine(FILE *stream, char **buffer, int *buflen);

struct TableRow {
  std::vector<std::string> columns;
};

std::vector<std::string> read_list(const char* filename);

std::vector<TableRow> read_table(const char* filename, const char* delimiter);

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_MISC_H__ */
