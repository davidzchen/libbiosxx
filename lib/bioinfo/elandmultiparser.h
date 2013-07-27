/**
 *   \file elandMultiParser.h
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOINFO_ELANDMULTIPARSER_H__
#define BIOINFO_ELANDMULTIPARSER_H__

#include <vector>
#include <string>

struct ElandMultiEntry {
  std::string chromosome;
  int position;
  char strand;
  int num_errors;
};

struct ElandMultiQuery {
  ElandMultiQuery();
  ~ElandMultiQuery();

  std::string sequence_name;
  std::string sequence;
  int exact_matches;
  int one_error_matches;
  int two_error_matches;
  std::vector<ElandMultiEntry> entries;
};

class ElandMultiParser {
 public:
  ElandMultiParser(std::string filename);
  ~ElandMultiParser();

  ElandMultiQuery* NextQuery();

 private:
  LineStream stream_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_ELANDMULTIPARSER_H__ */
