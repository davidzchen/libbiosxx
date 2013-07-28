/** 
 *   \file fastq.c Module to handle FASTQ sequences
 *   \author Andrea Sboner (andrea.sboner@yale.edu)
 */


#include "log.h"
#include "format.h"
#include "linestream.h"
#include "stringUtil.h"
#include "common.h"
#include "fastq.h"

Fastq::Fastq() {
  seq = new Seq;
}

Fastq::~Fastq() {
  delete seq;
}

FastqParser::FastqParser() {
  stream_ = NULL;
}

FastqParser::~FastqParser() {
  ls_destroy(stream_);
}

/**
 * Initialize the FASTQ module using a file name.
 * @note Use "-" to denote stdin.
 * @post FastqParser::nextSequence(), FastqParser::readAllSequences() can be called.
 */
void FastqParser::InitFromFile(const char* filename) {
  stream_ = ls_createFromFile(filename);
  ls_bufferSet(stream_, 1);
}

/**
 * Initialize the FASTQ module using a pipe.
 * @post FastqParser::nextSequence(), FastqParser::readAllSequences() can be called.
 */
void FastqParser::InitFromPipe(const char* command) {
  stream_ = ls_createFromPipe((char*) command);
  ls_bufferSet(stream_, 1);
}

Fastq* FastqParser::ProcessNextSequence (bool truncate_name) {
  if (ls_isEof (stream_)) {
    return NULL;
  }

  Fastq* fq = NULL;
  char* line = NULL;
  while ((line = ls_nextLine(stream_))) {
    if (line[0] == '\0') {
      continue;
    }
    if (line[0] == '@') {      
      fq = new Fastq;
      Seq* seq = fq->seq;
      seq->name = hlr_strdup(line + 1);
      if (truncate_name) {
        seq->name = firstWordInLine(skipLeadingSpaces(seq->name));
      }
      line = ls_nextLine(stream_); // reading sequence
      seq->sequence = hlr_strdup(line);
      seq->size = strlen(seq->sequence);
      line = ls_nextLine(stream_); // reading quality ID
      if (line[0] != '+') {
        die((char*) "Expected quality ID: '+' or '+%s'", seq->name);
      }
      line = ls_nextLine(stream_); // reading quality
      fq->quality = hlr_strdup(line);
      break;
    }
  }   
  return fq;
}

/**
 * Returns a pointer to the next FASTQ sequence.
 * @param[in] truncate_name If truncate_name > 0, leading spaces of the name are skipped. Furthermore, the name is truncated after the first white space. If truncate_name == 0, the name is stored as is.
 * @note The memory belongs to this routine.
 */
Fastq* FastqParser::NextSequence(bool truncate_name) {
  return ProcessNextSequence(truncate_name);
}

/**
 * Returns an Array of FASTQ sequences.
 * @param[in] truncate_name If truncate_name > 0, leading spaces of the name are skipped. Furthermore, the name is truncated after the first white space. If truncate_name == 0, the name is stored as is.
 * @note The memory belongs to this routine.
 */
std::vector<Fastq> FastqParser::ReadAllSequences(bool truncate_name) {
  std::vector<Fastq> seqs;
  Fastq* fq = NULL;
  while (fq = ProcessNextSequence(truncate_name)) {
    seqs.push_back(*fq);
  }
  return seqs;
}

/**
 * Prints currSeq to char*.
 */
char* FastqParser::PrintSequence(Fastq& fq) {
  std::stringstream string_buffer;
  string_buffer << "@" << fq.seq->name << "\n" << fq.seq->sequence << "\n+\n"
                << fq.quality;
  return strdup(string_buffer.str().c_str());
}

/**
 * Prints seqs to stdout.
 */
void FastqParser::PrintAllSequences(std::vector<Fastq>& fqs) {
  for (std::vector<Fastq>::iterator it = fqs.begin(); it != fqs.end(); ++it) {
    FastqParser::PrintSequence(*it); 
  }
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
