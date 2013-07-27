/** 
 *   \file bowtieParser.c Module to parse bowtie output files
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#include "format.h"
#include "log.h"
#include "linestream.h"
#include "common.h"
#include "bowtieparser.h"

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

// BowtieQuery

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

// BowtieParser

BowtieParser::BowtieParser() {
  stream_ = NULL;
}

BowtieParser::~BowtieParser() {
  ls_destroy(stream_);
}

/**
 * Initialize the bowtieParser module from file.
 * @param[in] fileName File name, use "-" to denote stdin
 */
void BowtieParser::InitFromFile(std::string filename) {
  stream_ = ls_createFromFile(filename.c_str());
  ls_bufferSet(stream_, 1);
}

/**
 * Initialize the bowtieParser module from pipe.
 * @param[in] command Command to be executed
 */
void BowtieParser::InitFromPipe(std::string command) {
  stream_ = ls_createFromPipe((char*) command.c_str());
  ls_bufferSet(stream_, 1);
}

void BowtieEntry::ProcessMismatches(char* token) {
  if (token[0] == '\0') {
    return;
  }
  WordIter w = wordIterCreate (token, (char*)",", 0);
  char* item;
  while (item = wordNext(w)) {
    BowtieMismatch mismatch;
    char* pos = strchr(item, ':');
    *pos = '\0';
    mismatch.offset = atoi(item);
    mismatch.reference_base = *(pos + 1);
    mismatch.read_base = *(pos + 3);
    mismatches_.push_back(mismatch);
  }
  wordIterDestroy(w);
}

void BowtieQuery::ProcessLine(char* line) {
  BowtieEntry entry;
  WordIter w = wordIterCreate(line, (char*) "\t", 0);
  entry.set_strand((wordNext(w))[0]);
  std::string chromosome(wordNext(w));
  entry.set_chromosome(chromosome);
  entry.set_position(atoi(wordNext(w)));
  std::string sequence(wordNext(w));
  entry.set_sequence(sequence);
  std::string quality(wordNext(w));
  entry.set_quality(quality);
  wordNext(w);
  entry.ProcessMismatches(wordNext(w));
  wordIterDestroy(w);
  entries_.push_back(entry);
}

BowtieQuery* BowtieParser::ProcessNextQuery() {
  if (bowtie_query_ != NULL) {
    delete bowtie_query_;
    bowtie_query_ = NULL;
  }

  if (!ls_isEof(stream_)) {
    return NULL;
  }

  bowtie_query_ = new BowtieQuery;
  int first = 1;
  char* line = NULL;
  while (line = ls_nextLine(stream_)) {
    if (line[0] == '\0') {
      continue;
    }
    char* pos = strchr(line, '\t');
    *pos = '\0';
    std::string query_name(line);
    query_name_ = query_name;
    if (first == 1 || prev_query_name_ == query_name_) {
      bowtie_query_->ProcessLine(pos + 1);
    } else {
      ls_back(stream_, 1);
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

/**
 * Returns a pointer to next BowtieQuery. 
 * @pre The module has been initialized using BowtieParser::init().
 * Parse entries of the following format:
   \verbatim
  
   Output (obtained from running bowtie -h)
   ----------------------------------------

   The 'bowtie' aligner outputs each alignment on a separate line.  Each
   line is a collection of 8 fields separated by tabs; from left to
   right, the fields are:

    1. Name of read that aligned

    2. Orientation of read in the alignment, '-' for reverse complement,
       '+' otherwise

    3. Name of reference sequence where alignment occurs, or ordinal ID
       if no name was provided

    4. 0-based offset into the reference sequence where leftmost
       character of the alignment occurs

    5. Read sequence (reverse-complemented if orientation is '-')

    6. Read qualities (reversed if orientation is '-')

    7. Reserved

    8. Comma-separated list of mismatch descriptors.  If there are no
       mismatches in the alignment, this field is empty.  A single
       descriptor has the format offset:reference-base>read-base.  The
       offset is expressed as a 0-based offset from the high-quality
       (5') end of the read. 


    Example:

    FC20B5TAA_50708:2:1:246:339         -        chr8        74005895        TAGATGTGTGGTATTATTTCTGAGGGC        IIIIIIIIIIIIIIIIIIIIIIIIIII        785        
    FC20B5TAA_50708:2:1:246:339         -        chr16        80796190        TAGATGTGTGGTATTATTTCTGAGGGC        IIIIIIIIIIIIIIIIIIIIIIIIIII        785        
    FC20B5TAA_50708:2:1:624:321         +        chr1        240849136        GGCTTAAAAGCTAGATGACGGGGTGAG        IIIIIIIIIIIIIIIIIIIIIIIIIII        0        9:C>G,26:T>G
    FC20B5TAA_50708:2:1:624:321         -        chrX        98759270        CTCACCCCGTCATCTAGCTTTTAAGCC        IIIIIIIIIIIIIIIIIIIIIIIIIII        2        22:A>C,26:A>C
    FC20B5TAA_50708:2:1:624:321         -        chr10        84291740        CTCACCCCGTCATCTAGCTTTTAAGCC        IIIIIIIIIIIIIIIIIIIIIIIIIII        2        22:A>C,26:A>C
    FC20B5TAA_50708:2:1:624:321         -        chr4        93757480        CTCACCCCGTCATCTAGCTTTTAAGCC        IIIIIIIIIIIIIIIIIIIIIIIIIII        2        22:A>C,26:A>C
    FC20B5TAA_50708:2:1:624:321         -        chr2        57057350        CTCACCCCGTCATCTAGCTTTTAAGCC        IIIIIIIIIIIIIIIIIIIIIIIIIII        0        22:A>C,24:T>C
    FC20B5TAA_50708:2:1:624:321         +        chr2        178610898        GGCTTAAAAGCTAGATGACGGGGTGAG        IIIIIIIIIIIIIIIIIIIIIIIIIII        0        22:T>G,26:T>G
    FC20B5TAA_50708:2:1:624:321         -        chr4        31274212        CTCACCCCGTCATCTAGCTTTTAAGCC        IIIIIIIIIIIIIIIIIIIIIIIIIII        0        9:G>C,26:A>C
    FC20B5TAA_50708:2:1:624:321         -        chr20        35191648        CTCACCCCGTCATCTAGCTTTTAAGCC        IIIIIIIIIIIIIIIIIIIIIIIIIII        2        9:G>C,22:A>C

   \endverbatim
 */
BowtieQuery* BowtieParser::NextQuery() {
  return ProcessNextQuery();
}

/**
 * Returns an Array of BowtieQueries.
 * @note The memory belongs to this routine.
 */
std::vector<BowtieQuery> BowtieParser::GetAllQueries() {
  std::vector<BowtieQuery> queries;
  BowtieQuery* query;
  while (query = ProcessNextQuery()) {
    queries.push_back(*query);
    delete query;
  }
  return queries;
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
