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

/// @file eland.hh
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing eland_result.txt files.

#ifndef BIOINFO_ELAND_H__
#define BIOINFO_ELAND_H__

#include <vector>
#include <string>
#include <iostream>

#include "worditer.hh"
#include "linestream.hh"

namespace bios {

struct ElandQuery {
  std::string sequence_name;
  std::string sequence;
  std::string match_code;
  int exact_matches;
  int one_error_matches;
  int two_error_matches;
  std::string chromosome;
  int position;
  char strand;
};

class ElandParser {
 public:
  /// @brief Class constructor
  ///
  /// @param filename
  ElandParser(const char* filename);

  /// @brief Class destructor
  ~ElandParser();

  /// @brief Returns a pointer to next ElandQuery. 
  /// @pre The module has been initialized using elandParser_init().
  /// Parse entries of the following format:
  /// @verbatim
  /// >FC30H5TAA_100308:2:1:1647:1161	GCTTACATTTTTCCTCTCTACATTATC	U0	1	0	0	chr17.fa	8466296	F	..
  /// >FC30H5TAA_100308:2:1:1588:122	GAGTTAGCCTTGGGACCCCTACTTCTT	U0	1	0	0	chr3.fa	61525628	F	..
  /// >FC30H5TAA_100308:2:1:1642:123	GGTGAGAGCCGCGACGGGCTTTAGGCG	NM	0	0	0
  /// >FC30H5TAA_100308:2:1:1630:119	CCGCCATTGCCAGCCCCCAGCTGACGG	R2	0	0	2
  /// >FC30H5TAA_100308:2:1:1603:120	GCAAGATGAAGTGAAAGGTAAAGAATC	U1	0	1	1	chrM.fa	15277	R	..	26A
  /// @endverbatim
  ElandQuery* NextQuery();

 private:
  LineStream* stream_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_ELAND_H__ */
