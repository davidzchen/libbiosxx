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
/// This is the header for the module for parsing eland_result.txt files.

#include "eland.hh"

namespace bios {

ElandParser::ElandParser(const char* filename) {
  stream_ = new FileLineStream(filename);
}

ElandParser::~ElandParser() {
  delete stream_;
}

ElandQuery* ElandParser::NextQuery() {
  for (std::string line; stream_->GetLine(line); ) {
    if (line.empty()) {
      continue;
    }
    WordIter w = WordIter(line, "\t", false);
    ElandQuery* query = new ElandQuery;
    // remove the '>' character at beginning of the line
    query->sequence_name = w.Next() + 1;
    query->sequence = w.Next();
    query->match_code = w.Next();
    if (query->match_code == "QC") {
      return query;
    }
    query->exact_matches = atoi(w.Next());
    query->one_error_matches = atoi(w.Next());
    query->two_error_matches = atoi(w.Next());
    char* token = w.Next();
    if (token == NULL) {
      return query;
    }
    char* pos;
    if (!(pos = strchr(token, '.'))) {
      std::cerr << "Expected '.' in chromosome name: " << token << std::endl;
      return NULL;
    }
    *pos = '\0';
    query->chromosome = pos + 1;
    query->position = atoi(w.Next());
    token = w.Next();
    if (token[0] == 'F') {
      query->strand = '+'; 
    } else if (token[0] == 'R') {
      query->strand = '-'; 
    } 
    return query;
  }
  return NULL;
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
