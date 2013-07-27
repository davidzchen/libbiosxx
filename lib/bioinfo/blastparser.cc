/** 
 *   \file blastParser.c Module to parse tab-delimited BLAST output
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#include "format.h"
#include "log.h"
#include "linestream.h"
#include "common.h"
#include "blastparser.h"

BlastQuery::BlastQuery() {
}

BlastQuery::~BlastQuery() {
}

void BlastQuery::ProcessLine(char* line) {
  BlastEntry entry;
  WordIter w = wordIterCreate(line, (char*) "\t", 0);
  std::string t_name(wordNext(w));
  entry.t_name = t_name;
  entry.percent_identity = atof(wordNext(w));
  entry.alignment_length = atoi(wordNext(w));
  entry.mis_matches = atoi(wordNext(w));
  entry.gap_openings = atoi(wordNext(w));
  entry.q_start = atoi(wordNext(w));
  entry.q_end = atoi(wordNext(w));
  entry.t_start = atoi(wordNext(w));
  entry.t_end = atoi(wordNext(w));
  entry.evalue = atof(wordNext(w));
  entry.bit_score = atof(wordNext(w));
  wordIterDestroy (w);
  entries.push_back(entry);
}

BlastEntry::BlastEntry() {
}

BlastEntry::~BlastEntry() {
}

BlastParser::BlastParser() {
  stream_ = NULL;
}

BlastParser::~BlastParser() {
  ls_destroy(stream_);
}

/**
 * Initialize the blastParser module from file.
 * @param[in] fileName File name, use "-" to denote stdin
 */
void BlastParser::InitFromFile(std::string filename) {
  stream_ = ls_createFromFile(filename.c_str());
  ls_bufferSet(stream_, 1);
}

/**
 * Initialize the blastParser module from pipe.
 * @param[in] command Command to be executed
 */
void BlastParser::InitFromPipe(std::string command) {
  stream_ = ls_createFromPipe((char*) command.c_str());
  ls_bufferSet(stream_, 1);
}

/**
 * Get the next BlastQuery.
 * @pre The module has been initialized using BlastParser::init().
 */
BlastQuery* BlastParser::NextQuery() {
  if (blast_query_ != NULL) {
    delete blast_query_;
    blast_query_ = NULL;
  }

  if (!ls_isEof(stream_)) {
    return NULL;
  }

  blast_query_ = new BlastQuery;
  int first = 1;
  char* line;
  while (line = ls_nextLine(stream_)) {
    if (line[0] == '\0') {
      continue;
    }
    char* pos = strchr(line, '\t');
    *pos = '\0';
    query_name_ = line;
    if (first == 1 || prev_query_name_ == query_name_) {
      blast_query_->ProcessLine(pos + 1);
    } else {
      ls_back(stream_, 1);
      return blast_query_;
    }
    if (first == 1) {
      blast_query_->q_name = query_name_;
      first = 0;
    }
    prev_query_name_ = query_name_;
  }
  if (first == 1) {
    return NULL;
  } else {
    return blast_query_;
  }
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
