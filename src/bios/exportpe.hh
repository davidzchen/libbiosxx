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
/// @author Andrea Sboner <andrea.sboner@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing *_export.txt files.

#ifndef BIOINFO_EXPORTPE_H__
#define BIOINFO_EXPORTPE_H__

#include <string>
#include <sstream>
#include <cstdlib>
#include <iostream>

#include "worditer.hh"
#include "linestream.hh"

namespace bios {

struct SingleEnd {
  /// Write an export entry;
  /// @param [in] currEntry: a pointer to the single end entry
  /// @return string formatted as an export file
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

  SingleEnd* end1;
  SingleEnd* end2;
};

class ExportPEParser {
 public:
  ExportPEParser();
  ~ExportPEParser();

  /// @brief Initialize the exportPEParser module from a file.
  /// @param filename1 First-end file name
  /// @param filename1 Second-end file name
  void InitFromFile(const char* filename1, const char* filename2);

  /// Initialize the exportPEParser module from a command/
  /// @param cmd1 command to be executed for the first end
  /// @param cmd2 command to be executed for the second end
  void InitFromPipe(const char* cmd1, const char* cmd2);

  ExportPE* NextEntry();

 private:
  int ProcessSingleEndEntry(ExportPE* entry, int read_number);
  ExportPE* ProcessNextEntry();

 private:
  LineStream* stream1_;
  LineStream* stream2_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_EXPORTPE_H__ */
