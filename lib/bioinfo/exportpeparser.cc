/** 
 *   \file exportPEParser.c Module to parse *_export.txt files, i.e. 
 *   output of Illumina GERALD/ELAND platform.
 *   \author Andrea Sboner (andrea.sboner@yale.edu)
 */

#include <stdlib.h>

#include "format.h"
#include "log.h"
#include "linestream.h"
#include "common.h"
#include "exportpeparser.h"

/** 
 * Write an export entry;
 * @param [in] currEntry: a pointer to the single end entry
 * @return string formatted as an export file
 */
std::string singleEnd::ToString() {
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

ExportPEParser::ExportPEParser() {
  stream1_ = NULL;
  stream2_ = NULL;
}

ExportPEParser::~ExportPEParser() {
  ls_destroy(stream1_);
  ls_destroy(stream2_);
}

/**
 * Initialize the exportPEParser module from a file.
 * @param[in] fileName1 First-end file name
 * @param[in] fileName1 Second-end file name
 */
void ExportPEParser::InitFromFile(std::string filename1, 
                                  std::string filename2) {
  stream1_ = ls_createFromFile(filename1.c_str());
  stream2_ = ls_createFromFile(filename2.c_str());
}

/**
 * Initialize the exportPEParser module from a command/
 * @param[in] cmd1 command to be executed for the first end
 * @param[in] cmd2 command to be executed for the second end
 */
void ExportPEParser::InitFromPipe(std::string cmd1, std::string cmd2) {
  stream1_ = ls_createFromPipe((char*) cmd1.c_str());
  stream2_ = ls_createFromPipe((char*) cmd2.c_str());
}

int ExportPEParser::ProcessSingleEndEntry(ExportPE* entry, int read_number) {
  char* line;
  if (read_number == 1) {
    line = ls_nextLine(stream1_);
  } else {
    line = ls_nextLine(stream2_);
  }
  if (ls_isEof(stream1_) || ls_isEof(stream2_)) {
    return 0;  // no more entries
  }
  WordIter w = wordIterCreate(line, (char*) "\t", 0);
  singleEnd* end = new singleEnd;
  end->machine = wordNext(w);
  end->run_number = atoi(wordNext(w));
  end->lane = atoi(wordNext(w));
  end->tile = atoi(wordNext(w));
  end->x_coord = atoi(wordNext(w));
  end->y_coord = atoi(wordNext(w));
  end->index = wordNext(w);
  end->read_number =  atoi(wordNext(w));
  end->sequence = wordNext(w);
  end->quality = wordNext(w);
  end->chromosome =  wordNext(w);
  end->contig = wordNext(w);
  end->position = atoi(wordNext(w));
  end->strand =  wordNext(w)[0] ;
  end->match_descriptor = wordNext(w);
  end->single_score = atoi(wordNext(w));
  end->paired_score = atoi(wordNext(w));
  end->partner_chromosome = wordNext(w);
  end->partner_contig = wordNext(w);
  end->partner_offset = atoi(wordNext(w));
  end->partner_strand = wordNext(w)[0];
  end->filter = wordNext(w)[0];
  if (read_number == 1) {
    entry->end1 = end;
  } else {
    entry->end2 = end;
  }
  wordIterDestroy(w);
  return 1; // still more entries
}

ExportPE* ExportPEParser::ProcessNextEntry() {
  int more_entries = 0;
  ExportPE* entry = new ExportPE;
  more_entries += ProcessSingleEndEntry(entry, 1);
  more_entries += ProcessSingleEndEntry(entry, 2);
  if (more_entries > 0) {
    singleEnd* end1 = entry->end1;
    singleEnd* end2 = entry->end2;
    std::stringstream id1;
    std::stringstream id2;
    id1 << end1->machine << ":" << end1->run_number << ":" << end1->lane << ":"
        << end1->tile << ":" << end1->x_coord << ":" << end1->y_coord << "#"
        << end1->index.c_str();
    id2 << end2->machine << ":" << end2->run_number << ":" << end2->lane << ":"
        << end2->tile << ":" << end2->x_coord << ":" << end2->y_coord << "#"
        << end2->index.c_str();
    if (id1.str() != id2.str()) {
      die((char*) "The IDs of the two entries do not match\n%s\n%s", 
          end1->ToString().c_str(), 
          end2->ToString().c_str());
    }
    return entry;
  } else {
    if (more_entries == 0) {
      return NULL;
    } else {
      die((char*) "The export files do not have the same length");
      return NULL;
    }
  }
}

/**
 * Read an entry in the export file;
 */
ExportPE* ExportPEParser::NextEntry() {
  return ExportPEParser::ProcessNextEntry();
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
