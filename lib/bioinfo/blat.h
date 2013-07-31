/**
 *   \file blatParser.h
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOINFO_BLAT_H__
#define BIOINFO_BLAT_H__

#include <vector>
#include <string>

#include "worditer.h"
#include "linestream.h"

struct PslEntry {
  int matches;       // Number of bases that match that aren't repeats
  int mismatches;    // Number of bases that don't match
  int repmatches;    // Number of bases that match but are part of repeats
  int n_count;       // Number of 'N' bases
  int q_num_insert;  // Number of inserts in query
  int q_base_insert; // Number of bases inserted in query
  int t_num_insert;  // Number of inserts in target
  int t_base_insert; // Number of bases inserted in target
  char strand;       // + or - for query strand, optionally followed by + or  for target strand
  int q_size;        // Query sequence size
  int q_start;       // Alignment start position in query
  int q_end;         // Alignment end position in query
  std::string t_name;   // Target sequence name
  int t_size;        // Target sequence size
  int t_start;       // Alignment start position in target
  int t_end;         // Alignment end position in target
  int block_count;   // Number of blocks in alignment. A block contains no gaps.
  std::vector<int> block_sizes; // Size of each block in a comma separated list
  std::vector<int> q_starts;    // Start of each block in query in a comma separated list
  std::vector<int> t_starts;    // Start of each block in target in a comma separated list
};

struct BlatQuery {
  std::string q_name;
  std::vector<PslEntry> entries;
};

class BlatParser {
 public:
  BlatParser();
  ~BlatParser();

  void InitFromFile(const char* filename);
  void InitFromPipe(const char* command);
  BlatQuery* NextQuery();

 private:
  void ProcessCommaSeparatedList(std::vector<int> results, char* str);

 private:
  LineStream* stream_;
  BlatQuery* blat_query_;
  std::string query_name_;
  std::string prev_query_name_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_BLAT_H__ */
