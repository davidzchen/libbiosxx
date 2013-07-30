/**
 *   \file elandParser.h
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOINFO_ELAND_H__
#define BIOINFO_ELAND_H__

#include <vector>
#include <string>

#include "format.h"
#include "log.h"
#include "linestream.h"
#include "common.h"

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
  ElandParser(std::string filename);
  ~ElandParser();

  ElandQuery* NextQuery();

 private:
  LineStream stream_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_ELAND_H__ */
