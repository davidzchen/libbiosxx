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

/// @file interval.hh
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the gene ontology (GO) module.
/// Module to efficiently find intervals that overlap with a query interval.
/// The algorithm is based on containment sublists. 
/// See Alekseyenko, A.V., Lee, C.J. (2007) Nested Containment List (NCList): 
/// A new algorithm for accelerating interval query of genome alignment and 
/// interval databases. Bioinformatics 23: 1386-1393.
/// (http://bioinformatics.oxfordjournals.org/cgi/content/abstract/23/11/1386)
/// Note: The Interval format is zero-based and half-open.
///
/// This tab-delimited file must have the 
/// following format:
/// 
/// @verbatim
/// Column:   Description:
/// 1         Name of the interval
/// 2         Chromosome 
/// 3         Strand
/// 4         Interval start
/// 5         Interval end
/// 6         Number of subintervals
/// 7         Subinterval starts (comma-delimited)
/// 8         Subinterval end (comma-delimited)
/// @endverbatim
///
/// This is an example:
/// @verbatim
/// uc001aaw.1  chr1  +  357521  358460  1  357521  358460
/// uc001aax.1  chr1  +  410068  411702  3  410068,410854,411258  410159,411121,411702
/// uc001aay.1  chr1  -  552622  554252  3  552622,553203,554161  553066,553466,554252
/// uc001aaz.1  chr1  +  556324  557910  1  556324  557910
/// uc001aba.1  chr1  +  558011  558705  1  558011  558705
/// @endverbatim
///
/// Note in this example the intervals represent a transcripts, while the 
/// subintervals denote exons.

#ifndef BIOS_INTERVAL_H__
#define BIOS_INTERVAL_H__

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <stdint.h>

#include "linestream.hh"
#include "worditer.hh"
#include "number.hh"

namespace bios {

/// @struct SubInterval
/// @brief Structure representing a subinterval.
struct SubInterval {
  int start;
  int end;
};

/// @struct Interval
/// @brief Structure representing an interval.
struct Interval {
  Interval();

  /// @brief Parse a line in the Interval format. 
  ///
  /// @param[in] thisInterval Pointer to an Interval. Must be allocated and 
  ///            deallocated externally.\n
  /// @param[in] line Line in Interval format\n
  /// @param[in] source An integer that specifies the source. This is useful when 
  ///            multiple files are used.
  /// See IntervalFind::addIntervalsToSearchSpace() for details.
  Interval(std::string& line, int source);
  ~Interval();

  /// @brief Write an Interval to a string.
  ///
  /// @param[in] currInterval Pointer to an Interval
  /// @return A char* representing the Interval in tab-delimited format
  std::string ToString();

  /// Get size of an Interval. This is done by summing up the size of the 
  /// subIntervals
  /// @param[in] currInterval Pointer to an Interval
  /// @return An integer representing the size
  uint32_t GetSize();

  static bool Compare(const Interval& a, const Interval& b) {
    int diff = strcmp(a.chromosome.c_str(), b.chromosome.c_str());
    if (diff != 0) {
      return diff < 0;
    }
    if (a.start != b.start) {
      return a.start < b.start;
    }
    return a.end < b.end;
  }

  int source;
  std::string name;
  std::string chromosome;
  char strand;
  int start;
  int end;
  int sub_interval_count;
  std::vector<SubInterval> sub_intervals; 
};

/// @struct SuperInterval
/// @brief Structure representing a superinterval.
struct SuperInterval {
  SuperInterval() {}
  ~SuperInterval() {}

  static bool Compare(const SuperInterval& a, const SuperInterval& b) {
    int diff = strcmp(a.chromosome.c_str(), b.chromosome.c_str());
    if (diff != 0) {
      return diff < 0;
    }
    if (a.start != b.start) {
      return a.start < b.start;
    }
    return a.end < b.end;
  }

  bool operator==(const SuperInterval& other) {
    return (strcmp(chromosome.c_str(), other.chromosome.c_str()) == 0 &&
        start == other.start &&
        end == other.end);
  }

  std::string chromosome;
  int start;
  int end;
  std::vector<Interval*> sublist;
};

/// @class IntervalFind
/// @brief Class for finding intervals.
class IntervalFind {
 public:
  IntervalFind();
  ~IntervalFind();

  /// Get the total number of intervals that have been added to the search space.
  /// @return The total number of intervals in the search space.
  /// @pre Intervals were added to the search space using 
  ///      IntervalFind::addIntervalsToSearchSpace()
  int GetNumberOfIntervals();

  /// Retrieve all the intervals that have been added to the search space.
  /// @return An Array of type Interval.
  /// @note This function generates a copy of the intervals array. 
  /// @pre Intervals were added to the search space using 
  ///      IntervalFind::addIntervalsToSearchSpace().
  std::vector<Interval> GetAllIntervals();

  /// Get an array of Interval pointers from the intervals that have been added to
  /// the search space.
  /// @return An Array of type Interval pointers.
  /// @note The user is not allowed to modify the content of the array. 
  /// @pre Intervals were added to the search space using 
  ///      IntervalFind::addIntervalsToSearchSpace().
  std::vector<Interval*> GetIntervalPointers();

  /// Add intervals to the search space. 
  /// @param[in] fileName File name of the file that contains the interval and 
  ///             subintervals to search against.
  /// @param[in] source An integer that specifies the source. This is useful when 
  ///            multiple files are used.
  void AddIntervalsToSearchSpace(const char* filename, int source);

  /// Get the intervals that overlap with the query interval.
  /// @param[in] chromosome Chromosome of the query interval
  /// @param[in] start Start of the query interval
  /// @param[in] end End of the query interval
  /// @return An Array of Interval pointers. If no overlapping intervals are found, 
  /// then an empty Array is returned
  /// @note The user is not allowed to modify the content of the array. 
  /// @pre Intervals were added to the search space, see 
  ///      IntervalFind::addIntervalsToSearchSpace()
  std::vector<Interval*> GetOverlappingIntervals(char* chromosome, int start,
                                                 int end);
 
  /// Parse a file in the Interval format. 
  /// @param[in] fileName File name of the file that contains the interval and 
  ///            subintervals.\n
  /// @param[in] source An integer that specifies the source. This is useful when 
  ///            multiple files are used.
  /// See IntervalFind::addIntervalsToSearchSpace() for details.
  /// @return Array of intervals. The user is responsible to free up the memory. 
  ///         The user can modify the returned Array. 
  /// @pre None.
  static std::vector<Interval> ParseFile(const char* filename, int source);

  /// Process a comma-separated list.
  ///
  /// @param results
  /// @param str the string to process.
  static void ProcessCommaSeparatedList(std::vector<int>& results, 
                                        std::string& str);

 private:
  void AssignSuperIntervals();
  void AddIntervals(std::vector<Interval*> matching_intervals,
                    std::vector<Interval*> sublist, int start, int end);

  static void ParseFileContent(std::vector<Interval>& intervals,
                               const char* filename, int source);

 private:
  std::vector<Interval> intervals_;
  bool super_intervals_assigned_;
  std::vector<SuperInterval> super_intervals_;
  bool super_interval_assigned_;
  std::map<int, std::string> source_to_file_;
  std::map<std::string, int> file_to_source_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_INTERVAL_H__ */
