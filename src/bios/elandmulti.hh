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
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing eland_multi.txt files.

#ifndef BIOINFO_ELANDMULTI_H__
#define BIOINFO_ELANDMULTI_H__

#include <vector>
#include <string>
#include <iostream>

#include "worditer.hh"
#include "linestream.hh"

namespace bios {

struct ElandMultiEntry {
  std::string chromosome;
  int position;
  char strand;
  int num_errors;
};

struct ElandMultiQuery {
  ElandMultiQuery();
  ~ElandMultiQuery();

  std::string sequence_name;
  std::string sequence;
  int exact_matches;
  int one_error_matches;
  int two_error_matches;
  std::vector<ElandMultiEntry> entries;
};

class ElandMultiParser {
 public:
  /// @brief Class constructor
  ElandMultiParser(const char* filename);

  /// @brief Class destructor
  ~ElandMultiParser();

  /// @brief Returns a pointer to next ElandMultiQuery. 
  /// 
  /// Parse entries of the following format:
  /// 
  /// @verbatim
  /// >FC30M30_111308:1:1:1713:829/1	AATAAACTCTCCTACTGATGATAGATGTTTTTTCT	NM
  /// >FC30M30_111308:1:1:1713:1605/1	ATATGAACAACGCCATGTTCTTGCAGAAAACGCTT	1:2:0	chr1.fa:16762278R0,21622953R1,143540857F1
  /// >FC30M30_111308:1:1:1666:671/1	ATCTACACCACCTCAATCACACTACTCCCCCTACC	0:0:1	chrM.fa:5359F2
  /// @endverbatim
  ElandMultiQuery* NextQuery();

 private:
  LineStream* stream_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_ELANDMULTI_H__ */
