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

/// @file eland.cc
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// Header for module to parse *_export.txt files, i.e. output of Illumina 
/// GERALD/ELAND platform.

#include "exportpe.hh"

namespace bios {

std::string SingleEnd::ToString() {
  std::stringstream string_buffer;
  string_buffer << machine.c_str() << "\t";
  string_buffer << run_number << "\t";
  string_buffer << lane << "\t";
  string_buffer << tile << "\t";
  string_buffer << x_coord << "\t";
  string_buffer << y_coord << "\t";
  string_buffer << index.c_str() << "\t";
  string_buffer << read_number << "\t";
  string_buffer << sequence.c_str() << "\t";
  string_buffer << quality.c_str() << "\t";
  string_buffer << chromosome.c_str() << "\t";
  string_buffer << contig.c_str() << "\t";
  if (position != 0 || strand != '\0') {
    string_buffer << position;
  }
  string_buffer << "\t";

  if (strand == '\0') {
    string_buffer << " " << "\t";
  } else {
    string_buffer << strand << "\t";
  }
  string_buffer << match_descriptor.c_str() << "\t";
  if (single_score != 0 || strand != '\0') {
    string_buffer << single_score;
  }
  string_buffer << "\t";

  if (paired_score != 0 || strand != '\0') {
    string_buffer << paired_score;
  }
  string_buffer << "\t";

  string_buffer << partner_chromosome.c_str() << "\t";
  string_buffer << partner_contig.c_str() << "\t";
  if (partner_offset != 0 || strand != '\0') {
    string_buffer << partner_offset;
  }
  string_buffer << "\t";

  if (partner_strand == '\0') {
    string_buffer << " " << "\t";
  } else {
    string_buffer << partner_strand << "\t";
  }
  string_buffer << filter;

  return string_buffer.str();
}

ExportPE::ExportPE() {
  end1 = NULL;
  end2 = NULL;
}

ExportPE::~ExportPE() {
  if (end1 != NULL) {
    delete end1;
  }
  if (end2 != NULL) {
    delete end2; 
  }
}

ExportPEParser::ExportPEParser() 
    : stream1_(NULL),
      stream2_(NULL) {
}

ExportPEParser::~ExportPEParser() {
  delete stream1_;
  delete stream2_;
}

void ExportPEParser::InitFromFile(const char* filename1, 
                                  const char* filename2) {
  stream1_ = new FileLineStream(filename1);
  stream2_ = new FileLineStream(filename2);
}

void ExportPEParser::InitFromPipe(const char* cmd1, const char* cmd2) {
  stream1_ = new PipeLineStream(cmd1);
  stream2_ = new PipeLineStream(cmd2);
}

int ExportPEParser::ProcessSingleEndEntry(ExportPE* entry, int read_number) {
  std::string line;
  bool ret;
  if (read_number == 1) {
    ret = stream1_->GetLine(line);
  } else {
    ret = stream2_->GetLine(line);
  }
  if (ret == false || stream1_->IsEof() || stream2_->IsEof()) {
    return 0;  // no more entries
  }
  WordIter w(line, "\t", false);
  SingleEnd* end = new SingleEnd;
  end->machine = w.Next();
  end->run_number = atoi(w.Next());
  end->lane = atoi(w.Next());
  end->tile = atoi(w.Next());
  end->x_coord = atoi(w.Next());
  end->y_coord = atoi(w.Next());
  end->index = w.Next();
  end->read_number =  atoi(w.Next());
  end->sequence = w.Next();
  end->quality = w.Next();
  end->chromosome =  w.Next();
  end->contig = w.Next();
  end->position = atoi(w.Next());
  end->strand =  w.Next()[0] ;
  end->match_descriptor = w.Next();
  end->single_score = atoi(w.Next());
  end->paired_score = atoi(w.Next());
  end->partner_chromosome = w.Next();
  end->partner_contig = w.Next();
  end->partner_offset = atoi(w.Next());
  end->partner_strand = w.Next()[0];
  end->filter = w.Next()[0];
  if (read_number == 1) {
    entry->end1 = end;
  } else {
    entry->end2 = end;
  }
  return 1; // still more entries
}

ExportPE* ExportPEParser::ProcessNextEntry() {
  int more_entries = 0;
  ExportPE* entry = new ExportPE;
  more_entries += ProcessSingleEndEntry(entry, 1);
  more_entries += ProcessSingleEndEntry(entry, 2);
  if (more_entries > 0) {
    SingleEnd* end1 = entry->end1;
    SingleEnd* end2 = entry->end2;
    std::stringstream id1;
    std::stringstream id2;
    id1 << end1->machine << ":" << end1->run_number << ":" << end1->lane << ":"
        << end1->tile << ":" << end1->x_coord << ":" << end1->y_coord << "#"
        << end1->index.c_str();
    id2 << end2->machine << ":" << end2->run_number << ":" << end2->lane << ":"
        << end2->tile << ":" << end2->x_coord << ":" << end2->y_coord << "#"
        << end2->index.c_str();
    if (id1.str() != id2.str()) {
      std::cerr << "The IDs of the two entries do not match" << std::endl
                << end1->ToString().c_str() << std::endl
                << end2->ToString().c_str() << std::endl;
      return NULL;
    }
    return entry;
  } else {
    if (more_entries == 0) {
      return NULL;
    } else {
      std::cerr << "The export files do not have the same length" << std::endl;
      return NULL;
    }
  }
}

ExportPE* ExportPEParser::NextEntry() {
  return ExportPEParser::ProcessNextEntry();
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
