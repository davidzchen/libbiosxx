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

/// @file bedgraph.hh
/// @author David Z. Chen <d.zhekai.chen@gmail.com>
/// @version 
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing BedGraphs files.

#ifndef BIOS_BEDGRAPH_H__
#define BIOS_BEDGRAPH_H__

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <stdint.h>

#include "worditer.hh"
#include "string.hh"
#include "linestream.hh"

namespace bios {

/// @class BedGraph
/// @brief Class representing a BedGraph file entry.
class BedGraph {
 public:
  BedGraph() {}
  ~BedGraph() {}

  std::string chromosome() const { return chromosome_; }
  uint32_t start() const { return start_; }
  uint32_t end() const { return end_; }
  double value() const { return value_; }

  void set_chromosome(std::string chromosome) { chromosome_ = chromosome; }
  void set_start(uint32_t start) { start_ = start; }
  void set_end(uint32_t end) { end_ = end; }
  void set_value(double value) { value_ = value; }

  static bool Compare(BedGraph& a, BedGraph& b) {
    int diff;
    diff = strcmp(a.chromosome().c_str(), b.chromosome().c_str());
    if (diff < 0) {
      return true;
    }
    if (a.start() < b.start()) {
      return true;
    }
    return a.end() < b.end();
  }

  bool operator==(const BedGraph& other) {
    return (chromosome_ == other.chromosome() &&
            start_ == other.start() &&
            end_ == other.end() &&
            value_ == other.value());
  }

 private:
  std::string chromosome_;
  uint32_t start_;
  uint32_t end_;
  double value_;
};

/// @class BedGraphParser
/// @brief Parser for parsing BedGraph files.
class BedGraphParser {
 public:
  BedGraphParser();
  ~BedGraphParser();

  /// @brief Initialize the BedGraphParser from a file.
  ///
  /// This method initializes the BedGraphParser from a file or standard
  /// input. This method or BedGraphParser::InitFromPipe must be called before
  /// any of the other parser methods.
  ///
  /// @param     filename    The name of the BedGraph file or '-' for stdin.
  void InitFromFile(const char* filename);
  
  /// @brief Initialize the BedGraphParser from a clommand.
  ///
  /// This method initializes the BedGraphParser from a command that is
  /// executed and whose output is piped as input. This method or 
  /// BedGraphParser::InitFromPipe must be called before any of the other 
  /// parser methods.
  ///
  /// @param     command     The command to be executed.
  void InitFromPipe(const char* command);

  /// @brief Retrieve the next entry from the BedGraph file.
  ///
  /// This method returns a pointer to a BedGraph object representing the next
  /// entry from the BedGraph file.
  ///
  /// @return    A BedGraph object representing the next entry from the
  ///            BedGraph file or NULL if the end of file is reached.
  BedGraph* NextEntry();

  /// @brief Retrieve all entries from the BedGraph file.
  ///
  /// This method returns an std::vector containing BedGraph objects
  /// representing all of the entries in the BedGraph file.
  ///
  /// @return    An std::vector containing all of the BedGraph entries in the
  ///            BedGraph file.
  std::vector<BedGraph> GetAllEntries();

  /// @brief Get the BedGraph values for all entries in the specified
  ///        chromosomal region.
  ///
  /// This method takes an array of BedGraph regions, a string describing a
  /// chromosome, and a start and end chromosomal coordinates and returns a
  /// vector of BedGraph values for that region.
  ///
  /// @return    An std::vector containing BedGraph values for the specified
  ///            region.
  static std::vector<double> GetValuesForRegion(
      std::vector<BedGraph>& bed_graphs, std::string chromosome, 
      uint32_t start, uint32_t end);

 private:
  LineStream* stream_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif // BIOS_BEDGRAPH_H__
