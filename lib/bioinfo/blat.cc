/** 
 *   \file blatParser.c Module to parse psl BLAT output
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#include "format.h"
#include "log.h"
#include "linestream.h"
#include "common.h"
#include "blat.h"

const int kPslHeaderLinesCount = 5;

BlatParser::BlatParser() {

}

BlatParser::~BlatParser() {
  ls_destroy(stream_);
  if (blat_query_ != NULL) {
    delete blat_query_;
  }
}

/**
 * Initialize the blatParser module from file.
 * @param[in] fileName File name, use "-" to denote stdin
 */
void BlatParser::InitFromFile(std::string filename) {
  stream_ = ls_createFromFile(filename.c_str());
  ls_bufferSet(stream_, 1);
  for (int i = 0; i < kPslHeaderLinesCount; ++i) {
    ls_nextLine(stream_);
  }
}

/**
 * Initialize the blatParser module from pipe.
 * @param[in] command Command to be executed
 */
void BlatParser::InitFromPipe(std::string command) {
  stream_ = ls_createFromPipe((char*) command.c_str());
  ls_bufferSet(stream_, 1);
  for (int i = 0; i < kPslHeaderLinesCount; ++i) {
    ls_nextLine(stream_);
  }
}

void BlatParser::ProcessCommaSeparatedList(std::vector<int> results, 
                                           char* str) {
  WordIter w = wordIterCreate(str, (char*) ",", 0);
  char *tok;
  while (tok = wordNext(w)) {
    if (tok[0] == '\0') {
      continue;
    }
    results.push_back(atoi(tok));
  }
  wordIterDestroy(w);
}

/**
 * Returns a pointer to next BlatQuery. 
 * @pre The module has been initialized using BlatParser::init().
 */
BlatQuery* BlatParser::NextQuery() {
  if (blat_query_ != NULL) {
    delete blat_query_;
    blat_query_ = NULL;
  }
  
  if (ls_isEof(stream_)) {
    return NULL;
  }

  blat_query_ = new BlatQuery;
  int first = 1;
  char* line = NULL;
  while (line = ls_nextLine(stream_)) {
    if (line[0] == '\0') {
      continue;
    }
    WordIter w = wordIterCreate(line, (char*) "\t", 0);
    int matches = atoi(wordNext(w));
    int mismatches = atoi(wordNext(w));
    int repmatches = atoi(wordNext(w));
    int n_count = atoi(wordNext(w));
    int q_num_insert = atoi(wordNext(w));
    int q_base_insert = atoi(wordNext(w));
    int t_num_insert = atoi(wordNext(w));
    int t_base_insert = atoi(wordNext(w));
    char strand = (wordNext(w))[0];
    std::string query_name(wordNext(w));
    query_name_ = query_name;
    if (first == 1 || prev_query_name_ == query_name_) {
      PslEntry psl_entry;
      psl_entry.matches = matches;
      psl_entry.mismatches = mismatches;
      psl_entry.repmatches = repmatches;
      psl_entry.n_count = n_count;
      psl_entry.q_num_insert = q_num_insert;
      psl_entry.q_base_insert = q_base_insert;
      psl_entry.t_num_insert = t_num_insert;
      psl_entry.t_base_insert = t_base_insert;
      psl_entry.strand = strand;
      psl_entry.q_size = atoi(wordNext(w));
      psl_entry.q_start = atoi(wordNext(w));
      psl_entry.q_end = atoi(wordNext(w));
      std::string name(wordNext(w));
      psl_entry.t_name = name;
      psl_entry.t_size = atoi(wordNext(w));
      psl_entry.t_start = atoi(wordNext(w));
      psl_entry.t_end = atoi(wordNext(w));
      psl_entry.block_count = atoi(wordNext(w));
      ProcessCommaSeparatedList(psl_entry.block_sizes, wordNext(w));
      ProcessCommaSeparatedList(psl_entry.q_starts, wordNext(w));
      ProcessCommaSeparatedList(psl_entry.t_starts, wordNext(w));
    } else {
      ls_back(stream_, 1);
      return blat_query_;
    }
    if (first == 1) {
      blat_query_->q_name = query_name;
      first = 0;
    }
    prev_query_name_ = query_name;
    wordIterDestroy(w);
  }
  if (first == 1) {
    return NULL;
  }
  return blat_query_;
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
