// This file is free software; you can redistribute it and/or 
// modify it under the terms of the GNU Lesser General Public 
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
//
// This file is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
// Lesser General Public License for more details.
//
// To obtain a copy of the GNU Lesser General Public License, 
// please write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
// or visit the WWW site http://www.gnu.org/copyleft/lesser.txt

/// @file blast.hh
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing tab-delimited BLAST output.

#include "blat.hh"

namespace bios {

const int kPslHeaderLinesCount = 5;

BlatParser::BlatParser() 
    : stream_(NULL) {
}

BlatParser::~BlatParser() {
  delete stream_;
  if (blat_query_ != NULL) {
    delete blat_query_;
  }
}

void BlatParser::InitFromFile(const char* filename) {
  stream_ = LineStream::FromFile(filename);
  stream_->SetBuffer(1);
  for (int i = 0; i < kPslHeaderLinesCount; ++i) {
    stream_->GetLine();
  }
}

void BlatParser::InitFromPipe(const char* command) {
  stream_ = LineStream::FromPipe(command);
  stream_->SetBuffer(1);
  for (int i = 0; i < kPslHeaderLinesCount; ++i) {
    stream_->GetLine();
  }
}

void BlatParser::ProcessCommaSeparatedList(std::vector<int>& results, 
                                           char* str) {
  WordIter* w = new WordIter(str, ",", false);
  char *tok;
  while ((tok = w->Next()) != NULL) {
    if (tok[0] == '\0') {
      continue;
    }
    results.push_back(atoi(tok));
  }
  delete w;
}

BlatQuery* BlatParser::NextQuery() {
  if (blat_query_ != NULL) {
    delete blat_query_;
    blat_query_ = NULL;
  }
 
  if (stream_->IsEof()) {
    return NULL;
  }

  blat_query_ = new BlatQuery;
  int first = 1;
  char* line = NULL;
  while ((line = stream_->GetLine()) != NULL) {
    if (line[0] == '\0') {
      continue;
    }
    WordIter* w = new WordIter(line, "\t", false);
    int matches = atoi(w->Next());
    int mismatches = atoi(w->Next());
    int repmatches = atoi(w->Next());
    int n_count = atoi(w->Next());
    int q_num_insert = atoi(w->Next());
    int q_base_insert = atoi(w->Next());
    int t_num_insert = atoi(w->Next());
    int t_base_insert = atoi(w->Next());
    char strand = (w->Next())[0];
    std::string query_name(w->Next());
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
      psl_entry.q_size = atoi(w->Next());
      psl_entry.q_start = atoi(w->Next());
      psl_entry.q_end = atoi(w->Next());
      std::string name(w->Next());
      psl_entry.t_name = name;
      psl_entry.t_size = atoi(w->Next());
      psl_entry.t_start = atoi(w->Next());
      psl_entry.t_end = atoi(w->Next());
      psl_entry.block_count = atoi(w->Next());
      ProcessCommaSeparatedList(psl_entry.block_sizes, w->Next());
      ProcessCommaSeparatedList(psl_entry.q_starts, w->Next());
      ProcessCommaSeparatedList(psl_entry.t_starts, w->Next());
    } else {
      stream_->Back(1);
      return blat_query_;
    }
    if (first == 1) {
      blat_query_->q_name = query_name;
      first = 0;
    }
    prev_query_name_ = query_name;
    delete w;
  }
  if (first == 1) {
    return NULL;
  }
  return blat_query_;
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
