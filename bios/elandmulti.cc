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

/// @file elandmulti.cc
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing eland_multi.txt files.

#include "elandmulti.hh"

namespace bios {

ElandMultiQuery::ElandMultiQuery() {
}

ElandMultiQuery::~ElandMultiQuery() {
  entries.clear();
}

ElandMultiParser::ElandMultiParser(const char* filename) {
  stream_ = new FileLineStream(filename);
}

ElandMultiParser::~ElandMultiParser() {
  delete stream_;
}

ElandMultiQuery* ElandMultiParser::NextQuery() {
  for (std::string line; stream_->GetLine(line); ) {
    if (line.empty()) {
      continue;
    }
    WordIter w1(line, "\t", false);
    ElandMultiQuery* query = new ElandMultiQuery;
    // remove the '>' character at beginning of the line
    std::string sequence_name(w1.Next() + 1);
    std::string sequence(w1.Next());
    query->sequence_name = sequence_name; 
    query->sequence = sequence;
    char* token = w1.Next();
    if (strcmp(token, "NM") == 0 || strcmp(token, "QC") == 0 || 
        strcmp(token, "RM") == 0) {
      return query;
    }
    char* first_colon = strchr(token, ':');
    char* last_colon = strrchr(token, ':');
    if (first_colon == NULL || last_colon == NULL) {
      std::cerr << "Expected the following format: x:y:z" << std::endl;
      return NULL;
    }
    *first_colon = '\0';
    *last_colon = '\0';
    query->exact_matches = atoi(token);
    query->one_error_matches = atoi(first_colon + 1);
    query->two_error_matches = atoi(last_colon + 1);
    token = w1.Next();
    if (token == NULL) {
      return query;
    }
    WordIter w2(token, ",", false);
    while ((token = w2.Next()) != NULL) {
      ElandMultiEntry entry;
      int token_length = strlen(token);
      if (token[token_length - 2] == 'F') {
        entry.strand = '+';
      } else if (token[token_length - 2] == 'R') {
        entry.strand = '-';
      } else {
        std::cerr << "Unexpected strand: " << token << std::endl;
        return NULL;
      }
      entry.num_errors = atoi(token + token_length - 1);
      token[token_length - 2] = '\0';
      char* pos1 = strchr(token, ':');
      std::string chromosome = "";
      if (pos1) {
        char* pos2 = strchr (token,'.');
        *pos2 = '\0';
        chromosome = token;
        token = pos1 + 1;
      }
      entry.position = atoi(token);
      entry.chromosome = chromosome;
      query->entries.push_back(entry);
    }
    return query;
  }
  return NULL;
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
