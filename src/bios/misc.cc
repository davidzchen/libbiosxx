#include "misc.hh"

namespace bios {

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

/**
 * Strip the trailing \n character off line, if present and return the length 
 * of the resulting line.
 * This function is typically used in conjunction with getLine().
 * @param[in] line
 * @param[out] line
 * @return Length of output line
 */
int stripNlGetLength(char *line) { 
  /*
  implementation note: since strlen() is a slow function if
    lines are long, I try to use it only when unavoidable.
    Since stripNlGetLength() has to find the end of the string
    I pass its finding back, so programs using this function
    would not need to call strlen() again.
  */
  int len = strlen(line) ;
  if (!len) {
    return 0 ;
  }
  --len ;
  if (line[len] == '\n') {
    line[len] = '\0' ;
  } else {
    ++len ;
  }
  return len ;
}

/**
 * Read a list from a file. 
 * @note Empty lines are skipped.
 */
std::vector<std::string> read_list(const char* filename) {
  std::vector<std::string> list;
  LineStream* ls = LineStream::FromFile(filename);
  char *line;
  while ((line = ls->GetLine()) != NULL) {
    if (line[0] == '\0') {
      continue;
    }
    list.push_back(line);
  }
  delete ls;
  return list;
}

/**
 * Read a table from a file.
 */
std::vector<TableRow> read_table(const char* filename, const char* delimiter) {
  std::vector<TableRow> table_rows;
  LineStream* ls = LineStream::FromFile(filename); 
  char* line = NULL;
  while ((line = ls->GetLine()) != NULL) {
    if (line[0] == '\0') {
      continue;
    }
    TableRow row;
    WordIter* w = new WordIter(line, delimiter, false);
    char* token = NULL;
    while ((token = w->Next()) != NULL) {
      row.columns.push_back(token);
    }
    table_rows.push_back(row);
    delete w;
  }
  delete ls;
  return table_rows;
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
