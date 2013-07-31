/** 
 *   \file elandParser.c Module to parse eland_result.txt files
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#include "eland.h"

ElandParser::ElandParser(const char* filename) {
  stream_ = LineStream::FromFile(filename);
}

ElandParser::~ElandParser() {
  delete stream_;
}

/**
 * Returns a pointer to next ElandQuery. 
 * @pre The module has been initialized using elandParser_init().
 * Parse entries of the following format:
   \verbatim
   >FC30H5TAA_100308:2:1:1647:1161	GCTTACATTTTTCCTCTCTACATTATC	U0	1	0	0	chr17.fa	8466296	F	..
   >FC30H5TAA_100308:2:1:1588:122	GAGTTAGCCTTGGGACCCCTACTTCTT	U0	1	0	0	chr3.fa	61525628	F	..
   >FC30H5TAA_100308:2:1:1642:123	GGTGAGAGCCGCGACGGGCTTTAGGCG	NM	0	0	0
   >FC30H5TAA_100308:2:1:1630:119	CCGCCATTGCCAGCCCCCAGCTGACGG	R2	0	0	2
   >FC30H5TAA_100308:2:1:1603:120	GCAAGATGAAGTGAAAGGTAAAGAATC	U1	0	1	1	chrM.fa	15277	R	..	26A
   \endverbatim
 */
ElandQuery* ElandParser::NextQuery() {
  char *line;
  while (line = stream_->GetLine()){
    if (line[0] == '\0') {
      continue;
    }
    WordIter* w = new WordIter(line, "\t", false);
    ElandQuery* query = new ElandQuery;
    // remove the '>' character at beginning of the line
    query->sequence_name = w->Next() + 1;
    query->sequence = w->Next();
    query->match_code = w->Next();
    if (query->match_code == "QC") {
      delete w;
      return query;
    }
    query->exact_matches = atoi(w->Next());
    query->one_error_matches = atoi(w->Next());
    query->two_error_matches = atoi(w->Next());
    char* token = w->Next();
    if (token == NULL) {
      delete w;
      return query;
    }
    char* pos;
    if (!(pos = strchr(token, '.'))) {
      die((char*) "Expected '.' in chromosome name: %s", token);
    }
    *pos = '\0';
    query->chromosome = pos + 1;
    query->position = atoi(w->Next());
    token = w->Next();
    if (token[0] == 'F') {
      query->strand = '+'; 
    } else if (token[0] == 'R') {
      query->strand = '-'; 
    } 
    delete w;
    return query;
  }
  return NULL;
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
