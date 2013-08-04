/** 
 *   \file fasta.c Module to handle FASTA sequences
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#include "fasta.hh"

namespace bios {

const int kCharactersPerLine = 60;

FastaParser::FastaParser()
    : stream_(NULL) {
}

FastaParser::~FastaParser() {
  delete stream_;
}

/**
 * Initialize the FASTA module using a file name.
 * @note Use "-" to denote stdin.
 * @post FastaParser::nextSequence(), FastaParser::readAllSequences() can be called.
 */
void FastaParser::InitFromFile(const char* filename) {
  stream_ = LineStream::FromFile(filename);
  stream_->SetBuffer(1);
}

/**
 * Initialize the FASTA module using a pipe.
 * @post FastaParser::nextSequence(), FastaParser::readAllSequences() can be called.
 */
void FastaParser::InitFromPipe(const char* command) {
  stream_ = LineStream::FromPipe(command);
  stream_->SetBuffer(1);
}

Seq* FastaParser::ProcessNextSequence(bool truncate_name) {
  if (stream_->IsEof()) {
    return NULL;
  }

  Seq* seq = new Seq;
  int count = 0;
  char* line;
  std::stringstream string_buffer;
  while ((line = stream_->GetLine()) != NULL) {
    if (line[0] == '\0') {
      continue;
    }
    if (line[0] == '>') {
      ++count;
      if (count == 1) {
        seq->name = strdup(line + 1);
        if (truncate_name) {
          seq->name = str::firstWordInLine(str::skipLeadingSpaces(seq->name));
        }
        continue;
      } else if (count == 2) {
        seq->sequence = strdup((char*) string_buffer.str().c_str());
        seq->size = string_buffer.str().size();
        stream_->Back(1);
        return seq;
      }
    }
    string_buffer << line;
  }
  seq->sequence = strdup((char*) string_buffer.str().c_str());
  seq->size = string_buffer.str().size();
  return seq;
} 

/**
 * Returns a pointer to the next FASTA sequence.
 * @param[in] truncate_name If truncate_name > 0, leading spaces of the name 
 *            are skipped. Furthermore, the name is truncated after the first 
 *            white space. If truncate_name == 0, the name is stored as is.
 * @note The memory belongs to this routine.
 */
Seq* FastaParser::NextSequence(bool truncate_name) {
  return ProcessNextSequence(truncate_name);
}

/**
 * Returns an Array of FASTA sequences.
 * @param[in] truncate_name If truncate_name > 0, leading spaces of the name 
 *            are skipped. Furthermore, the name is truncated after the first 
 *            white space. If truncate_name == 0, the name is stored as is.
 * @note The memory belongs to this routine.
 */
std::vector<Seq> FastaParser::ReadAllSequences(bool truncate_name) {
  std::vector<Seq> seqs;
  Seq* seq = NULL;
  while ((seq = ProcessNextSequence(truncate_name)) != NULL) {
    seqs.push_back(*seq);
  }
  return seqs;
}

/**
 * Prints currSeq to stdout.
 */
void FastaParser::PrintSequence(Seq& seq) {
  char* str = str::insertWordEveryNthPosition(seq.sequence, (char*) "\n",
                                              kCharactersPerLine);
  printf(">%s\n%s\n", seq.name, str);
}

/**
 * Prints seqs to stdout.
 */
void FastaParser::PrintAllSequences(std::vector<Seq>& seqs) {
  for (std::vector<Seq>::iterator it = seqs.begin(); it != seqs.end(); ++it) {
    FastaParser::PrintSequence(*it); 
  }
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
