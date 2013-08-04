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
/// This is the header for the module for parsing bowtie output files.
///
/// @verbatim
///
/// Output (obtained from running bowtie -h)
/// ----------------------------------------
///
/// The 'bowtie' aligner outputs each alignment on a separate line.  Each
/// line is a collection of 8 fields separated by tabs; from left to
/// right, the fields are:
///
///  1. Name of read that aligned
///
///  2. Orientation of read in the alignment, '-' for reverse complement,
///     '+' otherwise
///
///  3. Name of reference sequence where alignment occurs, or ordinal ID
///     if no name was provided
///
///  4. 0-based offset into the reference sequence where leftmost
///     character of the alignment occurs
///
///  5. Read sequence (reverse-complemented if orientation is '-')
///
///  6. Read qualities (reversed if orientation is '-')
///
///  7. Reserved
///
///  8. Comma-separated list of mismatch descriptors.  If there are no
///     mismatches in the alignment, this field is empty.  A single
///     descriptor has the format offset:reference-base>read-base.  The
///     offset is expressed as a 0-based offset from the high-quality
///     (5') end of the read. 
///
///  Example:
///
///  FC20B5TAA_50708:2:1:246:339  -  chr8  74005895  TAGATGTGTGGTATTATTTCTGAGGGC  IIIIIIIIIIIIIIIIIIIIIIIIIII  785    
///  FC20B5TAA_50708:2:1:246:339  -  chr16 80796190  TAGATGTGTGGTATTATTTCTGAGGGC  IIIIIIIIIIIIIIIIIIIIIIIIIII  785    
///  FC20B5TAA_50708:2:1:624:321  +  chr1  240849136 GGCTTAAAAGCTAGATGACGGGGTGAG  IIIIIIIIIIIIIIIIIIIIIIIIIII  0    9:C>G,26:T>G
///  FC20B5TAA_50708:2:1:624:321  -  chrX  98759270  CTCACCCCGTCATCTAGCTTTTAAGCC  IIIIIIIIIIIIIIIIIIIIIIIIIII  2    22:A>C,26:A>C
///  FC20B5TAA_50708:2:1:624:321  -  chr10 84291740  CTCACCCCGTCATCTAGCTTTTAAGCC  IIIIIIIIIIIIIIIIIIIIIIIIIII  2    22:A>C,26:A>C
///  FC20B5TAA_50708:2:1:624:321  -  chr4  93757480  CTCACCCCGTCATCTAGCTTTTAAGCC  IIIIIIIIIIIIIIIIIIIIIIIIIII  2    22:A>C,26:A>C
///  FC20B5TAA_50708:2:1:624:321  -  chr2  57057350  CTCACCCCGTCATCTAGCTTTTAAGCC  IIIIIIIIIIIIIIIIIIIIIIIIIII  0    22:A>C,24:T>C
///  FC20B5TAA_50708:2:1:624:321  +  chr2  178610898 GGCTTAAAAGCTAGATGACGGGGTGAG  IIIIIIIIIIIIIIIIIIIIIIIIIII  0    22:T>G,26:T>G
///  FC20B5TAA_50708:2:1:624:321  -  chr4  31274212  CTCACCCCGTCATCTAGCTTTTAAGCC  IIIIIIIIIIIIIIIIIIIIIIIIIII  0    9:G>C,26:A>C
///  FC20B5TAA_50708:2:1:624:321  -  chr20 35191648  CTCACCCCGTCATCTAGCTTTTAAGCC  IIIIIIIIIIIIIIIIIIIIIIIIIII  2    9:G>C,22:A>C
///
/// @endverbatim

#ifndef BIOS_BOWTIE_H__
#define BIOS_BOWTIE_H__

#include <vector>
#include <string>

#include "worditer.hh"
#include "linestream.hh"

namespace bios {

/// @struct BowtieMismatch
/// @brief Structure for representing a bowtie mismatch.
struct BowtieMismatch {
  int offset;
  char reference_base;
  char read_base;
};

/// @struct BowtieEntry
/// @brief Structure for representing a bowtie entry.
struct BowtieEntry {
  BowtieEntry();
  BowtieEntry(const BowtieEntry& src_entry);
  ~BowtieEntry();

  std::string chromosome() const { return chromosome_; }
  std::string sequence() const { return sequence_; }
  std::string quality() const { return quality_; }
  int position() const { return position_; }
  char strand() const { return strand_; }
  const std::vector<BowtieMismatch>& mismatches() const { return mismatches_; }

  void set_chromosome(std::string chromosome) { chromosome_ = chromosome; }
  void set_sequence(std::string sequence) { sequence_ = sequence; }
  void set_quality(std::string quality) { quality_ = quality; }
  void set_position(int position) { position_ = position; }
  void set_strand(char strand) { strand_ = strand; }

  void ProcessMismatches(char* token);

 private:
  std::string chromosome_;
  std::string sequence_;
  std::string quality_;
  int position_;
  char strand_;
  std::vector<BowtieMismatch> mismatches_;
};

/// @class BowtieQuery
/// @brief Class for representing a Bowtie query.
class BowtieQuery {
 public:
  BowtieQuery();
  BowtieQuery(const BowtieQuery& src_query);
  ~BowtieQuery();

  const std::vector<BowtieEntry>& entries() const { return entries_; }
  std::string sequence_name() const { return sequence_name_; }
  void set_sequence_name(std::string sequence_name) {
    sequence_name_ = sequence_name;
  }

  void ProcessLine(char* line);

 private:
  std::string sequence_name_;
  std::vector<BowtieEntry> entries_;
};

/// @class BowtieParser
/// @brief Class for parsing bowtie output files.
class BowtieParser {
 public:
  /// @brief Class constructor.
  ///
  /// Constructs a new BowtieParser object.
  BowtieParser();

  /// @brief Class destructor.
  /// Destroys  BowtieParser object.
  ~BowtieParser();

  /// @brief Initializes the BowtieParser from a file.
  ///
  /// This method initializes the 
  void InitFromFile(const char* filename);

  /// @brief Initializes the BowtieParser from a piped command.
  ///
  /// This method executes the given command and pipes the output to the
  /// BowtieParser.
  ///
  /// @param    command   The command to be executed.
  void InitFromPipe(const char* command);

  /// @brief Returns the next bowtie query from the file.
  ///
  /// This method parses and returns a pointer to a BowtieQuery object
  /// representing the next bowtie query from the file.
  ///
  /// @return   The next bowtie query from the file.
  BowtieQuery* NextQuery();

  /// @brief Returns a vector of all bowtie queries from the file.
  ///
  /// This method parses the entire file and returns an std::vector of
  /// BowtieQuery objects representing each of the bowtie queries parsed from 
  /// the file.
  ///
  /// @return   All bowtie queries from the file.
  std::vector<BowtieQuery> GetAllQueries();

 private:
  /// @brief Parses the next bowtie query from the file.
  ///
  /// This method takes the next line from the input file, parses the line,
  /// and returns a pointer to a BowtieQuery object representing the the parsed
  /// bowtie query.
  ///
  /// @return   The next bowtie query from the file.
  BowtieQuery* ProcessNextQuery();

 private:
  LineStream* stream_;
  BowtieQuery* bowtie_query_;
  std::string query_name_;
  std::string prev_query_name_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_BOWTIE_H__ */
