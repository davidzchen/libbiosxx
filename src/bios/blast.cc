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

/// @file blast.cc
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing tab-delimited BLAST output.

#include "blast.hh"

namespace bios {

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

void BlastParser::InitFromFile(const char* filename) {
  stream_ = LineStream::FromFile(filename);
  stream_->SetBuffer(1);
}

void BlastParser::InitFromPipe(const char* command) {
  stream_ = LineStream::FromPipe(command);
  stream_->SetBuffer(1);
}

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
  while ((line = stream_->GetLine()) != NULL) {
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

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
