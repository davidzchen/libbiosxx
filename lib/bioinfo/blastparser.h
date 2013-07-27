/**
 *   \file blastParser.h
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOINFO_BLASTPARSER_H__
#define BIOINFO_BLASTPARSER_H__

#include <vector>
#include <string>

struct BlastEntry {
  BlastEntry();
  ~BlastEntry();

  std::string t_name;
  double percent_identity;
  int alignment_length;
  int mis_matches;
  int gap_openings;
  int q_start;
  int q_end;
  int t_start;
  int t_end;
  double evalue;
  double bit_score;
};

struct BlastQuery {
  BlastQuery();
  ~BlastQuery();
  void ProcessLine(char* line);

  std::string q_name;
  std::vector<BlastEntry> entries;
};

class BlastParser {
 public:
  BlastParser();
  ~BlastParser();

  void InitFromFile(std::string filename);
  void InitFromPipe(std::string command);
  BlastQuery* NextQuery();

 private:
  LineStream stream_;
  std::string query_name_;
  std::string prev_query_name_;
  BlastQuery* blast_query_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_BLASTPARSER_H__ */
