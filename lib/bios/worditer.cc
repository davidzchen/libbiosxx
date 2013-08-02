#include "worditer.hh"

namespace bios {

WordIter::WordIter(char* str, const char* seps, bool collapse_separators) {
  if (str == NULL || seps == NULL) {
    return;
  }
  seps_ = seps;
  position_ = str;
  collapse_separators_ = collapse_separators;
  at_end_ = false;
}

WordIter::~WordIter() {
}

char* WordIter::Next(int* index) {
  if (index != NULL) {
    *index = 0;
  }

  if (at_end_ == true) {
    return NULL;
  }
  char* position = position_;
  if (collapse_separators_) {
    --position;
    while (*++position && strchr(seps_, *position))
      ;
  } else {
    if (!*position) {
      at_end_ = true;
      return position;
    }
    if (strchr(seps_, *position)) {
      ++position_;
      *position = '\0';
      return position;
    }
  }
  if (!*position) {
    at_end_ = true;
    return NULL;
  }

  // Here holds: we are on the beginning of a word, on a non-separator char.
  // Now run until end of this word.
  char* word = position;
  --position;
  while (*++position && !strchr(seps_, *position))
    ;
  if (index != NULL) {
    *index = position - word;
  }

  if (!*position) {
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
