/** 
 *   \file blastParser.c Module to parse tab-delimited BLAST output
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#include "blast.hh"

BlastQuery::BlastQuery() {
}

BlastQuery::~BlastQuery() {
}

void BlastQuery::ProcessLine(char* line) {
  BlastEntry entry;
  WordIter* w = new WordIter(line, "\t", false);
  std::string t_name(w->Next());
  entry.t_name = t_name;
  entry.percent_identity = atof(w->Next());
  entry.alignment_length = atoi(w->Next());
  entry.mis_matches = atoi(w->Next());
  entry.gap_openings = atoi(w->Next());
  entry.q_start = atoi(w->Next());
  entry.q_end = atoi(w->Next());
  entry.t_start = atoi(w->Next());
  entry.t_end = atoi(w->Next());
  entry.evalue = atof(w->Next());
  entry.bit_score = atof(w->Next());
  delete w;
  entries.push_back(entry);
}

BlastEntry::BlastEntry() {
}

BlastEntry::~BlastEntry() {
}

BlastParser::BlastParser()
    : stream_(NULL) {
}

BlastParser::~BlastParser() {
  delete stream_;
}

/**
 * Initialize the blastParser module from file.
 * @param[in] fileName File name, use "-" to denote stdin
 */
void BlastParser::InitFromFile(const char* filename) {
  stream_ = LineStream::FromFile(filename);
  stream_->SetBuffer(1);
}

/**
 * Initialize the blastParser module from pipe.
 * @param[in] command Command to be executed
 */
void BlastParser::InitFromPipe(const char* command) {
  stream_ = LineStream::FromPipe(command);
  stream_->SetBuffer(1);
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

  if (stream_->IsEof()) {
    return NULL;
  }

  blast_query_ = new BlastQuery;
  int first = 1;
  char* line;
  while (line = stream_->GetLine()) {
    if (line[0] == '\0') {
      continue;
    }
    char* pos = strchr(line, '\t');
    *pos = '\0';
    query_name_ = line;
    if (first == 1 || prev_query_name_ == query_name_) {
      blast_query_->ProcessLine(pos + 1);
    } else {
      stream_->Back(1);
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
