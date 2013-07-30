/**
 *   \file exportPEParser.h
 *   \author Andrea Sboner (andrea.sboner@yale.edu)
 */

#ifndef BIOINFO_EXPORTPE_H__
#define BIOINFO_EXPORTPE_H__

#include <string>
#include <sstream>
#include <cstdlib>

#include "format.h"
#include "log.h"
#include "linestream.h"
#include "common.h"

/**
 * ExportPEsingle
 */

struct singleEnd {
  std::string ToString();

  std::string machine;            // 1 machine
  int run_number;                 // 2 run number
  int lane;                       // 3 lane
  int tile;                       // 4 tile 
  int x_coord;                    // 5 x coordinate of cluster
  int y_coord;                    // 6 y coordinate of cluster
  std::string index;              // 7 index string
  int read_number;                // 8 read number
  std::string sequence;           // 9 read
  std::string quality;            // 10 quality
  std::string chromosome;         // 11 match chromosome
  std::string contig;             // 12 match contig
  int position;                   // 13 match position
  char strand;                    // 14 match strand
  std::string match_descriptor;   // 15 match descriptor
  int single_score;               // 16 single read alignment score
  int paired_score;               // 17 paired end alignment score
  std::string partner_chromosome; // 18 partner chromosome
  std::string partner_contig;     // 19 partner contig
  int partner_offset;             // 20 partner offset
  char partner_strand;            // 21 partner strand
  char filter;                    // 22 filter
};

struct ExportPE {
  ExportPE();
  ~ExportPE();

  singleEnd* end1;
  singleEnd* end2;
};

class ExportPEParser {
 public:
  ExportPEParser();
  ~ExportPEParser();

  void InitFromFile(std::string filename1, std::string filename2);
  void InitFromPipe(std::string cmd1, std::string cmd2);

  ExportPE* NextEntry();

 private:
  int ProcessSingleEndEntry(ExportPE* entry, int read_number);
  ExportPE* ProcessNextEntry();

 private:
  LineStream stream1_;
  LineStream stream2_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_EXPORTPE_H__ */
