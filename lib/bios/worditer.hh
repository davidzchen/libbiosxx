#ifndef BIOINFO_WORDITER_H__
#define BIOINFO_WORDITER_H__

#include <cstdio>
#include <cstring>

class WordIter {
 public:
  WordIter(char* str, const char* seps, bool collapse_separators);
  ~WordIter();

  char* Next();
  char* Next(int* index);

 private:
  char* position_;
  const char* seps_;
  bool collapse_separators_;
  bool at_end_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_WORDITER_H__ */
