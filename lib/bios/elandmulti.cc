/** 
 *   \file elandMultiParser.c Module to parse eland_multi.txt files
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#include "elandmulti.hh"

namespace bios {

ElandMultiQuery::ElandMultiQuery() {
}

ElandMultiQuery::~ElandMultiQuery() {
  entries.clear();
}

/**
 * Initialize the elandMultiParser module.
 * @param[in] fileName File name, use "-" to denote stdin
 */
ElandMultiParser::ElandMultiParser(const char* filename) {
  stream_ = LineStream::FromFile(filename);
}

/**
 * Deinitialize the elandMultiParser module.
 */
ElandMultiParser::~ElandMultiParser() {
  delete stream_;
}

/**
 * Returns a pointer to next ElandMultiQuery. 
 * @pre The module has been initialized using elandMultiParser_init().
 * Parse entries of the following format:
   \verbatim
   >FC30M30_111308:1:1:1713:829/1	AATAAACTCTCCTACTGATGATAGATGTTTTTTCT	NM
   >FC30M30_111308:1:1:1713:1605/1	ATATGAACAACGCCATGTTCTTGCAGAAAACGCTT	1:2:0	chr1.fa:16762278R0,21622953R1,143540857F1
   >FC30M30_111308:1:1:1666:671/1	ATCTACACCACCTCAATCACACTACTCCCCCTACC	0:0:1	chrM.fa:5359F2
   \endverbatim
 */
ElandMultiQuery* ElandMultiParser::NextQuery() {
  char* line;
  while (line = stream_->GetLine()) {
    if (line[0] == '\0') {
      continue;
    }
    WordIter* w1 = new WordIter(line, "\t", false);
    ElandMultiQuery* query = new ElandMultiQuery;
    // remove the '>' character at beginning of the line
    std::string sequence_name(w1->Next() + 1);
    std::string sequence(w1->Next());
    query->sequence_name = sequence_name; 
    query->sequence = sequence;
    char* token = w1->Next();
    if (strcmp(token, "NM") == 0 || strcmp(token, "QC") == 0 || 
        strcmp(token, "RM") == 0) {
      delete w1;
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
    token = w1->Next();
    if (token == NULL) {
      delete w1;
      return query;
    }
    WordIter* w2 = new WordIter(token, ",", false);
    while (token = w2->Next()) {
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
    delete w1;
    delete w2;
    return query;
  }
  return NULL;
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
