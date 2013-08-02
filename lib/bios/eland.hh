/**
 *   \file elandParser.h
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOINFO_ELAND_H__
#define BIOINFO_ELAND_H__

#include <vector>
#include <string>
#include <iostream>

#include "worditer.hh"
#include "linestream.hh"

namespace bios {

struct ElandQuery {
  std::string sequence_name;
  std::string sequence;
  std::string match_code;
  int exact_matches;
  int one_error_matches;
  int two_error_matches;
  std::string chromosome;
  int position;
  char strand;
};

class ElandParser {
 public:
  ElandParser(const char* filename);
  ~ElandParser();

  ElandQuery* NextQuery();

 private:
  LineStream* stream_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_ELAND_H__ */
