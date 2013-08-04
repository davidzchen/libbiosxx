#ifndef BIOS_WORDITER_H__
#define BIOS_WORDITER_H__

#include <cstdio>
#include <cstring>

namespace bios {

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

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_WORDITER_H__ */
