/** 
 *   \file fastq.h
 *   \author Andrea Sboner (andrea.sboner@yale.edu)
 */

#ifndef BIOINFO_FASTQ_H__
#define BIOINFO_FASTQ_H__

#include <vector>
#include <string>
#include <iostream>

#include "seq.hh"
#include "linestream.hh"
#include "string.hh"

struct Fastq {
  Fastq();
  ~Fastq();

  Seq* seq;
  char* quality;
};

class FastqParser {
 public:
  FastqParser();
  ~FastqParser();

  void InitFromFile(const char* filename);
  void InitFromPipe(const char* command);

  Fastq* NextSequence(bool truncate_name);
  std::vector<Fastq> ReadAllSequences(bool truncate_name);
  char* PrintSequence(Fastq& fq);
  void PrintAllSequences(std::vector<Fastq>& fqs);

 private:
  Fastq* ProcessNextSequence(bool truncate_name);

 private:
  enum {
    kCharactersPerLine = 60,
  };

  LineStream* stream_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_FASTQ_H__ */
