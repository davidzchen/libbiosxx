#include "worditer.hh"

namespace bios {

WordIter::WordIter(const char* str, const char* seps, 
                   bool collapse_separators) {
  if (str == NULL || seps == NULL) {
    return;
  }
  str_ = strdup(str);
  seps_ = seps;
  position_ = str_;
  collapse_separators_ = collapse_separators;
  at_end_ = false;
}

WordIter::~WordIter() {
  if (str_ != NULL) {
    free(str_);
  }
}

char* WordIter::Next(int* index) {
  if (index != NULL) {
    *index = 0;
  }

  if (at_end_ == true) {
    return NULL;
  }
  char* position = position_;
  if (collapse_separators_ != false) {
    --position;
    while (*++position != '\0' && strchr(seps_, *position) != NULL)
      ;
  } else {
    if (*position == '\0') {
      at_end_ = true;
      return NULL;
    }
    if (strchr(seps_, *position) != NULL) {
      ++position_;
      *position = '\0';
      return position;
    }
  }
  if (*position == '\0') {
    at_end_ = true;
    return NULL;
  }

  // Here holds: we are on the beginning of a word, on a non-separator char.
  // Now run until end of this word.
  char* word = position;
  --position;
  while (*++position != '\0' && strchr(seps_, *position) == NULL)
    ;
  if (index != NULL) {
    *index = position - word;
  }

  if (*position == '\0') {
    at_end_ = true;
  } else {
    *position = '\0';
    position_ = position + 1;
  }
  return word;
}

char* WordIter::Next() {
  return Next(NULL); 
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
