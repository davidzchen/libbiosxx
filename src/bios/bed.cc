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

/// @file bed.cc
/// @author Andrea Sboner <ans2077@med.cornell.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing BED files.

#include "bed.hh"

namespace bios {

Bed::Bed() {
  sub_blocks_ = std::vector<SubBlock>();
}

Bed::~Bed() {
  sub_blocks_.clear();
}

std::string Bed::ToString() {
  std::stringstream string_buffer;
  SubBlock* sub_block;
  if (extended_) {
    string_buffer << chromosome_.c_str() << "\t";
    string_buffer << start_ << "\t";
    string_buffer << end_ << "\t";
    string_buffer << name_.c_str() << "\t";
    string_buffer << score_ << "\t";
    string_buffer << strand_ << "\t";
    string_buffer << thick_start_ << "\t";
    string_buffer << thick_end_ << "\t";
    string_buffer << item_rgb_.c_str() << "\t";
    string_buffer << block_count_ << "\t";
    for (int i = 0; i < sub_blocks_.size(); ++i) {
      string_buffer << sub_blocks_[i].size;
      string_buffer << (i < sub_blocks_.size() - 1) ? "," : "\t";
    }
    for (int i = 0; i < sub_blocks_.size(); ++i) {
      string_buffer << sub_blocks_[i].start;
      string_buffer << (i < sub_blocks_.size() - 1) ? "," : "";
    }
  } else {
    string_buffer << chromosome_.c_str() << "\t";
    string_buffer << start_ << "\t";
    string_buffer << end_;
  }
  return string_buffer.str();
}

BedParser::BedParser()
    : stream_(NULL) {
}

BedParser::~BedParser() {
  delete stream_;
}

void BedParser::InitFromFile(const char* filename) {
  stream_ = LineStream::FromFile(filename);
  stream_->SetBuffer(1);
}

void BedParser::InitFromCommand(const char* command) {
  stream_ = LineStream::FromPipe(command);
  stream_->SetBuffer(1);
}

Bed* BedParser::NextEntry(void) {
  char* line = NULL;
  while (!stream_->IsEof()) {
    char* line = stream_->GetLine();
    if (str::strStartsWithC(line, "track") || 
        str::strStartsWithC(line, "browser")) {
      continue;
    }
    Bed* bed = new Bed();
    WordIter* w = new WordIter(line, "\t", true);
    std::string chromosome(w->Next());
    bed->set_chromosome(chromosome);
    bed->set_start(atoi(w->Next()));
    bed->set_end(atoi(w->Next()));
    char *next = w->Next();
    if (next != NULL) {
      std::string name(next);
      bed->set_name(name);
      bed->set_extended(true);
      bed->set_score(atoi(w->Next()));
      bed->set_strand(w->Next()[0]);
      bed->set_thick_start(atoi(w->Next()));
      bed->set_thick_end(atoi(w->Next()));
      std::string item_rgb(w->Next());
      bed->set_item_rgb(item_rgb);
      bed->set_block_count(atoi(w->Next()));
      WordIter* wsizes = new WordIter(w->Next(), ",", true);
      WordIter* wstarts = new WordIter(w->Next(), ",", true); 
      for (int i = 0; i < bed->block_count(); ++i) {
        SubBlock sub_block;
        sub_block.size = atoi(wsizes->Next());
        sub_block.start = atoi(wstarts->Next());
        bed->AddSubBlock(sub_block);
      }
      delete wsizes;
      delete wstarts;
    } else {
      bed->set_extended(false);
    }
    delete w;
    return bed;
  }
  return NULL;
}

std::vector<Bed> BedParser::GetAllEntries() {
  std::vector<Bed> beds = std::vector<Bed>();
  Bed* current_bed;
  int i = 0;
  while (current_bed = NextEntry()) {
    beds.push_back(*current_bed);
    delete current_bed;
    ++i;
  }
  return beds;
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
