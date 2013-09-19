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
/// This is the header for the module for parsing bowtie output files.

#include "bowtie.hh"

namespace bios {

BowtieEntry::BowtieEntry() {
  mismatches_ = std::vector<BowtieMismatch>();
}

BowtieEntry::BowtieEntry(const BowtieEntry& src_entry) {
  const std::vector<BowtieMismatch>& src_mismatches = src_entry.mismatches();
  for (std::vector<BowtieMismatch>::const_iterator it = src_mismatches.begin();
      it != src_mismatches.end(); ++it) {
    const BowtieMismatch& src_mismatch = *it;
    BowtieMismatch dest_mismatch;
    dest_mismatch.offset = src_mismatch.offset;
    dest_mismatch.reference_base = src_mismatch.reference_base;
    dest_mismatch.read_base = src_mismatch.read_base;
    mismatches_.push_back(dest_mismatch);
  }
  chromosome_ = src_entry.chromosome();
  sequence_ = src_entry.sequence();
  quality_ = src_entry.quality();
  position_ = src_entry.position();
  strand_ = src_entry.strand();
}

BowtieEntry::~BowtieEntry() {
  mismatches_.clear();
}

BowtieQuery::BowtieQuery() {
  entries_ = std::vector<BowtieEntry>();
}

BowtieQuery::BowtieQuery(const BowtieQuery& src_query) {
  sequence_name_ = src_query.sequence_name();
  const std::vector<BowtieEntry>& src_entries = src_query.entries();
  for (std::vector<BowtieEntry>::const_iterator it = src_entries.begin();
      it != src_entries.end(); ++it) {
    const BowtieEntry& src_entry = *it;
    BowtieEntry dest_entry(src_entry);
    entries_.push_back(dest_entry);
  }
}

BowtieQuery::~BowtieQuery() {
  entries_.clear();
}

BowtieParser::BowtieParser()
    : stream_(NULL) {
}

BowtieParser::~BowtieParser() {
  delete stream_;
}

/**
 * Initialize the bowtieParser module from file.
 * @param[in] fileName File name, use "-" to denote stdin
 */
void BowtieParser::InitFromFile(const char* filename) {
  stream_ = new FileLineStream(filename);
  stream_->SetBuffer(1);
}

void BowtieParser::InitFromPipe(const char* command) {
  stream_ = new PipeLineStream(command);
  stream_->SetBuffer(1);
}

void BowtieEntry::ProcessMismatches(char* token) {
  if (token[0] == '\0') {
    return;
  }
  WordIter w(token, ",", false);
  for (char* item; (item = w.Next()) != NULL; ) {
    BowtieMismatch mismatch;
    char* pos = strchr(item, ':');
    *pos = '\0';
    mismatch.offset = atoi(item);
    mismatch.reference_base = *(pos + 1);
    mismatch.read_base = *(pos + 3);
    mismatches_.push_back(mismatch);
  }
}

void BowtieQuery::ProcessLine(std::string& line) {
  BowtieEntry entry;
  WordIter w(line, "\t", false);
  entry.set_strand((w.Next())[0]);
  std::string chromosome(w.Next());
  entry.set_chromosome(chromosome);
  entry.set_position(atoi(w.Next()));
  std::string sequence(w.Next());
  entry.set_sequence(sequence);
  std::string quality(w.Next());
  entry.set_quality(quality);
  w.Next();
  entry.ProcessMismatches(w.Next());
  entries_.push_back(entry);
}

BowtieQuery* BowtieParser::ProcessNextQuery() {
  if (bowtie_query_ != NULL) {
    delete bowtie_query_;
    bowtie_query_ = NULL;
  }

  if (stream_->IsEof()) {
    return NULL;
  }

  bowtie_query_ = new BowtieQuery;
  int first = 1;
  for (std::string line; stream_->GetLine(line); ) {
    if (line.empty()) {
      continue;
    }
    size_t pos = line.find('\t');
    query_name_.replace(0, pos + 1, line);
    if (first == 1 || prev_query_name_ == query_name_) {
      std::string query_line = line.substr(pos + 1, line.size() - pos);
      bowtie_query_->ProcessLine(query_line);
    } else {
      stream_->Back(line);
      return bowtie_query_;
    }
    if (first == 1) {
      bowtie_query_->set_sequence_name(query_name_);
      first = 0;
    }
    prev_query_name_ = query_name_;
  }
  if (first == 1) {
    return NULL;
  } else {
    return bowtie_query_;
  }
}

BowtieQuery* BowtieParser::NextQuery() {
  return ProcessNextQuery();
}

std::vector<BowtieQuery> BowtieParser::GetAllQueries() {
  std::vector<BowtieQuery> queries;
  BowtieQuery* query;
  while ((query = ProcessNextQuery()) != NULL) {
    queries.push_back(*query);
    delete query;
  }
  return queries;
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
