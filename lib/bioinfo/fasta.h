/** 
 *   \file fasta.h
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOINFO_FASTA_H__
#define BIOINFO_FASTA_H__

#include <string>
#include <vector>

#include "seq.h"
#include "format.h"
#include "linestream.h"
#include "string.h"

class FastaParser {
 public:
  FastaParser();
  ~FastaParser();

  void InitFromFile(std::string filename);
  void InitFromPipe(std::string command);

  Seq* NextSequence(bool truncate_name);
  std::vector<Seq> ReadAllSequences(bool truncate_name);
  void PrintSequence(Seq& seq);
  void PrintAllSequences(std::vector<Seq>& seqs);

 private:
  Seq* ProcessNextSequence(bool truncate_name);

 private:
  LineStream stream_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_FASTA_H__ */
