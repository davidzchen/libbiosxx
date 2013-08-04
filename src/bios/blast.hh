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

/// @file blast.hh
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing tab-delimited BLAST output.

#ifndef BIOS_BLAST_H__
#define BIOS_BLAST_H__

#include <vector>
#include <string>

#include "worditer.hh"
#include "linestream.hh"

namespace bios {

/// @struct BlastEntry
/// @brief Structure representing a singe BLAST entry.
struct BlastEntry {
  BlastEntry();
  ~BlastEntry();

  std::string t_name;
  double percent_identity;
  int alignment_length;
  int mis_matches;
  int gap_openings;
  int q_start;
  int q_end;
  int t_start;
  int t_end;
  double evalue;
  double bit_score;
};

/// @struct BlastQuery
/// @brief Structure representing a BLAST query.
struct BlastQuery {
  BlastQuery();
  ~BlastQuery();
  void ProcessLine(char* line);

  std::string q_name;
  std::vector<BlastEntry> entries;
};

/// @class BlastParser
/// @brief Class for parsing tab-delimited BLAST output.
class BlastParser {
 public:
  /// @brief Class constructor.
  ///
  /// Constructs a new BlastParser object.
  BlastParser();

  /// @brief Class destructor.
  ///
  /// Destroys the BlastParser object.
  ~BlastParser();

  /// @brief Initializes the BlastParser from a file.
  ///
  /// This method initializes the BlastParser from a file or standard input.
  /// This method or BlastParser::InitFromPipe must be called before any of
  /// the other parser methods are called.
  ///
  /// @param    filename  The name of the file to read from or '-' for stdin.
  void InitFromFile(const char* filename);
  
  /// @brief Initializes the BlastParser from a file.
  ///
  /// This method initializes the BlastParser from a file or standard input.
  /// This method or BlastParser::InitFromFile must be called before any of
  /// the other parser methods are called.
  ///
  /// @param    filename  The name of the file to read from or '-' for stdin.
  void InitFromPipe(const char* command);

  /// @brief Returns the next BLAST query from the file.
  ///
  /// This method returns a pointer to a BlastQuery representing the next
  /// BLAST query from the file.
  ///
  /// @return   A pointer to the next BlastQuery from the BLAST file.
  BlastQuery* NextQuery();

 private:
  LineStream* stream_;
  std::string query_name_;
  std::string prev_query_name_;
  BlastQuery* blast_query_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_BLAST_H__ */
