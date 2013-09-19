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

/// @file blat.hh
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing PSL BLAT output.

#ifndef BIOS_BLAT_H__
#define BIOS_BLAT_H__

#include <vector>
#include <string>

#include "worditer.hh"
#include "linestream.hh"

namespace bios {

/// @struct PslEntry
/// @brief A structure representing a PSL entry.
struct PslEntry {
  int matches;       // Number of bases that match that aren't repeats
  int mismatches;    // Number of bases that don't match
  int repmatches;    // Number of bases that match but are part of repeats
  int n_count;       // Number of 'N' bases
  int q_num_insert;  // Number of inserts in query
  int q_base_insert; // Number of bases inserted in query
  int t_num_insert;  // Number of inserts in target
  int t_base_insert; // Number of bases inserted in target
  char strand;       // + or - for query strand, optionally followed by + or  for target strand
  int q_size;        // Query sequence size
  int q_start;       // Alignment start position in query
  int q_end;         // Alignment end position in query
  std::string t_name;   // Target sequence name
  int t_size;        // Target sequence size
  int t_start;       // Alignment start position in target
  int t_end;         // Alignment end position in target
  int block_count;   // Number of blocks in alignment. A block contains no gaps.
  std::vector<int> block_sizes; // Size of each block in a comma separated list
  std::vector<int> q_starts;    // Start of each block in query in a comma separated list
  std::vector<int> t_starts;    // Start of each block in target in a comma separated list
};

/// @struct BlatQuery
/// @brief Struct representing a BLAT query.
struct BlatQuery {
  std::string q_name;
  std::vector<PslEntry> entries;
};

class BlatParser {
 public:
  /// @brief Class constructor
  ///
  /// Constructs a BlatParser object.
  BlatParser();

  /// @brief Class destructor.
  ///
  /// Destroys a BlatParser object.
  ~BlatParser();

  /// @brief Initializes the BlatParser from a file.
  ///
  /// This method initializes the BlatParser from the given file or from
  /// standard input.
  ///
  /// @param    filename  The name of the file or '-' for stdin.
  void InitFromFile(const char* filename);
  
  /// @brief Initializes the BlatParser from a command.
  ///
  /// This method initializes the BlatParser from a command whose output is
  /// piped into the parser.
  ///
  /// @param    command  The command to be executed.
  void InitFromPipe(const char* command);

  /// @brief Returns the next BLAT query from the file.
  ///
  /// This method returns a pointer to a BlatQuery object representing the next
  /// BLAT query from the file.
  ///
  /// @return   A pointer to a BlatQuery object for the next BLAT query.
  BlatQuery* NextQuery();

 private:
  /// @brief Processes a line from the PSL BLAT file.
  ///
  /// This method tokenizes a comma-separated line from the PSL BLAT file and
  /// pushes all tokens into a vector of ints.
  ///
  /// @param    results   The vector to store the resulting ints.
  /// @param    str       The string to process.
  void ProcessCommaSeparatedList(std::vector<int>& results, std::string& str);

 private:
  LineStream* stream_;
  BlatQuery* blat_query_;
  std::string query_name_;
  std::string prev_query_name_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_BLAT_H__ */
