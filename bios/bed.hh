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

/// @file bed.hh
/// @author Andrea Sboner <ans2077@med.cornell.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing BED files.

#ifndef BIOS_BED_H__
#define BIOS_BED_H__

#include <iostream>

#include <string>
#include <sstream>
#include <vector>
#include <stdint.h>

#include "string.hh"
#include "worditer.hh"
#include "linestream.hh"

namespace bios {

/// @struct SubBlock
/// @brief  Struct representing a sub-block.
///
/// Something something.
struct SubBlock {
  uint32_t size;
  uint32_t start; // relative to Bed->start
};

/// @struct Bed
/// @brief  BED entry.
///
/// Something something.
class Bed {
 public:
  Bed();
  ~Bed();

  // Getters.
  std::string chromosome() const { return chromosome_; }
  uint32_t start() const { return start_; }
  uint32_t end() const { return end_; }
  bool extended() const { return extended_; }
  std::string name() const { return name_; }
  uint32_t score() const { return score_; }
  char strand() const { return strand_; }
  uint32_t thick_start() const { return thick_start_; }
  uint32_t thick_end() const { return thick_end_; }
  std::string item_rgb() const { return item_rgb_; }
  uint32_t block_count() const { return block_count_; }

  // Setters.
  void set_chromosome(std::string chromosome) { chromosome_ = chromosome; }
  void set_start(uint32_t start) { start_ = start; }
  void set_end(uint32_t end) { end_ = end; }
  void set_extended(bool extended) { extended_ = extended; }
  void set_name(std::string name) { name_ = name; }
  void set_score(uint32_t score) { score_ = score; }
  void set_strand(char strand) { strand_ = strand; }
  void set_thick_start(uint32_t thick_start) { thick_start_ = thick_start; }
  void set_thick_end(uint32_t thick_end) { thick_end_ = thick_end; }
  void set_item_rgb(std::string item_rgb) { item_rgb_ = item_rgb; }
  void set_block_count(uint32_t block_count) { block_count_ = block_count; }

  std::string ToString();
  void AddSubBlock(SubBlock& sub_block) {
    sub_blocks_.push_back(sub_block);
  }

  static bool Compare(Bed& a, Bed& b) {
    int diff = strcmp(a.chromosome().c_str(), b.chromosome().c_str());
    if (diff != 0) {
      return (diff < 0);
    }
    if (a.start() != b.start()) {
      return a.start() < b.start();
    }
    return a.end() < b.end();
  }

 private:
  std::string chromosome_;
  uint32_t start_;
  uint32_t end_;
  bool extended_; 

  // Optional
  std::string name_;
  uint32_t score_;  // [0,1000]
  char strand_; // '+' or '-'
  uint32_t thick_start_; 
  uint32_t thick_end_;
  std::string item_rgb_; // ex. 255,0,0
  uint32_t block_count_;
  std::vector<SubBlock> sub_blocks_;
};

/// @class BedParser
/// @brief Class for parsing BED files.
class BedParser {
 public:
  BedParser();
  ~BedParser();

  /// @brief Initialize the BedParser from a file.
  ///
  /// This method initializes the BedParser from a file. This method or
  /// InitFromCommand must be called before calling other parser methods.
  ///
  /// @param     filename     The name of the input file or '-' for stdin.
  void InitFromFile(const char* filename);

  /// @brief Initialize the BedParser from a command.
  ///
  /// This method initializes the BedParser from a command. This method or
  /// InitFromFile must be called before calling other parser methods.
  ///
  /// @param     filename     The command to be executed.
  void InitFromCommand(const char* command);

  /// @brief Retrieve the next entry in the BED file.
  ///
  /// This method retrieves the next entry in the BED file.
  ///
  /// @return    A pointer to the a Bed object representing the next entry in
  ///            the BED file.
  Bed* NextEntry();

  /// @brief Retrieves all entries from the BED file.
  ///
  /// This method parses the BED file and returns a vector containing all of
  /// entries from the BED file.
  ///
  /// @return    a vector containing Bed objects representing all entries in
  ///            the BED file.
  std::vector<Bed> GetAllEntries();

 private:
  LineStream* stream_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif // BIOS_BED_H__
