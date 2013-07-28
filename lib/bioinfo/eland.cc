/** 
 *   \file elandParser.c Module to parse eland_result.txt files
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#include "format.h"
#include "log.h"
#include "linestream.h"
#include "common.h"
#include "eland.h"

ElandParser::ElandParser(std::string filename) {
  stream_ = ls_createFromFile(filename.c_str());
}

ElandParser::~ElandParser() {
  ls_destroy(stream_);
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
  while (line = ls_nextLine(stream_)) {
    if (line[0] == '\0') {
      continue;
    }
    WordIter w = wordIterCreate(line, (char*) "\t", 0);
    ElandQuery* query = new ElandQuery;
    // remove the '>' character at beginning of the line
    query->sequence_name = wordNext(w) + 1;
    query->sequence = wordNext(w);
    query->match_code = wordNext(w);
    if (query->match_code == "QC") {
      wordIterDestroy(w);
      return query;
    }
    query->exact_matches = atoi(wordNext(w));
    query->one_error_matches = atoi(wordNext(w));
    query->two_error_matches = atoi(wordNext(w));
    char* token = wordNext(w);
    if (token == NULL) {
      wordIterDestroy(w);
      return query;
    }
    char* pos;
    if (!(pos = strchr(token, '.'))) {
      die ((char*) "Expected '.' in chromosome name: %s", token);
    }
    *pos = '\0';
    query->chromosome = pos + 1;
    query->position = atoi(wordNext(w));
    token = wordNext(w);
    if (token[0] == 'F') {
      query->strand = '+'; 
    }
    else if (token[0] == 'R') {
      query->strand = '-'; 
    } 
    wordIterDestroy(w);
    return query;
  }
  return NULL;
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
