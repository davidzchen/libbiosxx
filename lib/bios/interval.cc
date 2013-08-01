/**
 *   \file intervalFind.c Module to efficiently find intervals that overlap 
 *   with a query interval.
 *   The algorithm is based on containment sublists. 
     See Alekseyenko, A.V., Lee, C.J. (2007) Nested Containment List (NCList): 
     A new algorithm for accelerating interval query of genome alignment and 
     interval databases. Bioinformatics 23: 1386-1393.
     (http://bioinformatics.oxfordjournals.org/cgi/content/abstract/23/11/1386)
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 *   Note: The Interval format is zero-based and half-open.
 */

#include "interval.hh"

Interval::Interval() {
}

/**
 * Parse a line in the Interval format. 
 * @param[in] thisInterval Pointer to an Interval. Must be allocated and 
 *            deallocated externally.\n
 * @param[in] line Line in Interval format\n
 * @param[in] source An integer that specifies the source. This is useful when 
 *            multiple files are used.
 * See IntervalFind::addIntervalsToSearchSpace() for details.
 * @pre None.
*/
Interval::Interval(const char* line, int source) {
  WordIter* w = new WordIter((char*) line, "\t", 0);
  source = source;
  name = w->Next();
  chromosome = w->Next();
  strand = w->Next()[0];
  start = atoi(w->Next());
  end = atoi(w->Next());
  sub_interval_count = atoi(w->Next());
  std::vector<int> starts;
  std::vector<int> ends;
  IntervalFind::ProcessCommaSeparatedList(starts, w->Next());
  IntervalFind::ProcessCommaSeparatedList(ends, w->Next());
  if (starts.size() != ends.size()) {
    std::cerr << "Unequal number of subIntervalStarts and subIntervalEnds"
              << std::endl;
    return;
  }
  for (int i = 0; i < sub_interval_count; ++i) {
    SubInterval sub_interval;
    sub_interval.start = starts[i];
    sub_interval.end = ends[i];
    sub_intervals.push_back(sub_interval);
  }
  delete w;
}

Interval::~Interval() {
  sub_intervals.clear();
}

/**
 * Write an Interval to a string.
 * @param[in] currInterval Pointer to an Interval
 * @return A char* representing the Interval in tab-delimited format
 */
std::string Interval::ToString() {
  std::stringstream string_buffer;
  string_buffer << name << "\t";
  string_buffer << chromosome << "\t";
  string_buffer << strand << "\t";
  string_buffer << start << "\t";
  string_buffer << end << "\t";
  string_buffer << sub_interval_count << "\t";
  for (int i = 0; i < sub_intervals.size(); ++i) {
    string_buffer << sub_intervals[i].start;
    string_buffer << (i < sub_intervals.size() - 1) ? "," : "\t";
  }
  for (int i = 0; i < sub_intervals.size(); ++i) {
    string_buffer << sub_intervals[i].end;
    string_buffer << (i < sub_intervals.size() - 1) ? "," : "\t";
  }
  return string_buffer.str();
}

/**
 * Get size of an Interval. This is done by summing up the size of the 
 * subIntervals
 * @param[in] currInterval Pointer to an Interval
 * @return An integer representing the size
 */
int Interval::GetSize() {
  int size = 0;
  for (std::vector<SubInterval>::iterator it = sub_intervals.begin();
      it != sub_intervals.end(); ++it) {
    size += it->end - it->start;
  }
  return size;
}

IntervalFind::IntervalFind() {
}

IntervalFind::~IntervalFind() {
  intervals_.clear();
  super_intervals_.clear();
  source_to_file_.clear();
  file_to_source_.clear();
}

/**
 * Get the total number of intervals that have been added to the search space.
 * @return The total number of intervals in the search space.
 * @pre Intervals were added to the search space using 
 *      IntervalFind::addIntervalsToSearchSpace()
 */
int IntervalFind::GetNumberOfIntervals() {
  return intervals_.size();
}

/**
 * Retrieve all the intervals that have been added to the search space.
 * @return An Array of type Interval.
 * @note This function generates a copy of the intervals array. 
 * @pre Intervals were added to the search space using 
 *      IntervalFind::addIntervalsToSearchSpace().
 */
std::vector<Interval> IntervalFind::GetAllIntervals() {
  return std::vector<Interval>(intervals_);
}

/**
 * Get an array of Interval pointers from the intervals that have been added to
 * the search space.
 * @return An Array of type Interval pointers.
 * @note The user is not allowed to modify the content of the array. 
 * @pre Intervals were added to the search space using 
 *      IntervalFind::addIntervalsToSearchSpace().
 */
std::vector<Interval*> IntervalFind::GetIntervalPointers() {
  std::vector<Interval*> intervals;
  for (std::vector<Interval>::iterator it = intervals_.begin();
      it != intervals_.end(); ++it) {
    intervals.push_back(&(*it));
  }
  return intervals;
}

void IntervalFind::ProcessCommaSeparatedList(std::vector<int>& results, 
                                             const char* str) {
  WordIter* w = new WordIter((char*) str, ",", false);
  char* tok = NULL;
  while (tok = w->Next()) {
    if (tok[0] == '\0') {
      continue;
    }
    results.push_back(atoi(tok));
  }
  delete w;
}

void IntervalFind::ParseFileContent(std::vector<Interval>& intervals, 
                                    const char* filename, int source) {
  LineStream* ls = LineStream::FromFile(filename);
  char* line = NULL;
  while (line = ls->GetLine()) {
    if (line[0] == '\0') {
      continue;
    }
    Interval interval(line, source);
    intervals.push_back(interval);
  }
  delete ls;
}

/**
 * Add intervals to the search space. 
 * @param[in] fileName File name of the file that contains the interval and 
 * subintervals to search against. This tab-delimited file must have the 
 * following format:
   
   \verbatim
   Column:   Description:
   1         Name of the interval
   2         Chromosome 
   3         Strand
   4         Interval start
   5         Interval end
   6         Number of subintervals
   7         Subinterval starts (comma-delimited)
   8         Subinterval end (comma-delimited)
   \endverbatim

   This is an example:
   \verbatim
   uc001aaw.1      chr1    +       357521  358460  1       357521  358460
   uc001aax.1      chr1    +       410068  411702  3       410068,410854,411258    410159,411121,411702
   uc001aay.1      chr1    -       552622  554252  3       552622,553203,554161    553066,553466,554252
   uc001aaz.1      chr1    +       556324  557910  1       556324  557910
   uc001aba.1      chr1    +       558011  558705  1       558011  558705
   \endverbatim
   Note in this example the intervals represent a transcripts, while the subintervals denote exons.

 * @param[in] source An integer that specifies the source. This is useful when multiple files are used.
*/
void IntervalFind::AddIntervalsToSearchSpace(const char* filename, 
                                             int source) {
  ParseFileContent(intervals_, filename, source);
}

/**
 * Parse a file in the Interval format. 
 * @param[in] fileName File name of the file that contains the interval and 
 *            subintervals.\n
 * @param[in] source An integer that specifies the source. This is useful when 
 *            multiple files are used.
 * See IntervalFind::addIntervalsToSearchSpace() for details.
 * @return Array of intervals. The user is responsible to free up the memory. 
 *         The user can modify the returned Array. 
 * @pre None.
*/
std::vector<Interval> IntervalFind::ParseFile(const char* filename, 
                                              int source) {
  std::vector<Interval> intervals;
  ParseFileContent(intervals, filename, source);
  return intervals;
}

void IntervalFind::AssignSuperIntervals() {
  if (super_intervals_assigned_ == true) {
    return;
  }
  std::sort(intervals_.begin(), intervals_.end(), Interval::Compare);
  int i = 0;
  while (i < intervals_.size()) {
    Interval& interval = intervals_[i];
    SuperInterval super_interval;
    super_interval.chromosome = interval.chromosome;
    super_interval.start = interval.start; 
    super_interval.end = interval.end;
    super_interval.sublist.push_back(&interval);
    int j = i + 1;
    while (j < intervals_.size()) {
      Interval& next_interval = intervals_[j];
      if (interval.chromosome == next_interval.chromosome &&
          interval.start <= next_interval.start &&
          interval.end >= next_interval.end) {
        super_interval.sublist.push_back(&next_interval);
      } else {
        break;
      }
      ++j;
    }
    i = j;
    super_intervals_.push_back(super_interval);
  }
  std::sort(super_intervals_.begin(), super_intervals_.end(), 
            SuperInterval::Compare);
  super_intervals_assigned_ = true;
}

void IntervalFind::AddIntervals(std::vector<Interval*> matching_intervals, 
                                std::vector<Interval*> sublist, int start, 
                                int end) {
  for (std::vector<Interval*>::iterator it = sublist.begin(); 
      it != sublist.end(); ++it) {
    Interval* interval = *it;
    int intersection = num::range_intersection(interval->start, interval->end, 
                                               start, end);
    if (intersection >= 0) {
      matching_intervals.push_back(interval);
    }
  }
}

/**
 * Get the intervals that overlap with the query interval.
 * @param[in] chromosome Chromosome of the query interval
 * @param[in] start Start of the query interval
 * @param[in] end End of the query interval
 * @return An Array of Interval pointers. If no overlapping intervals are found, 
   then an empty Array is returned
 * @note The user is not allowed to modify the content of the array. 
 * @pre Intervals were added to the search space, see 
 *      IntervalFind::addIntervalsToSearchSpace()
 */
std::vector<Interval*> IntervalFind::GetOverlappingIntervals(
    char* chromosome, int start, int end) {
  if (super_interval_assigned_ == false) {
     AssignSuperIntervals();
  }
  std::vector<Interval*> matching_intervals;
  SuperInterval test_super_interval;
  test_super_interval.chromosome = chromosome;
  test_super_interval.start = start;
  test_super_interval.end = end;
  std::vector<SuperInterval>::iterator it = std::find(
      super_intervals_.begin(), super_intervals_.end(), test_super_interval);
  // Index points to the location where test_super_interval would be inserted
  int index = std::distance(super_intervals_.begin(), it);
  int i = index;
  while (i >= 0) {
    SuperInterval& super_interval = super_intervals_[i];
    if (super_interval.chromosome != chromosome || 
        super_interval.end < start) {
      break;
    }
    AddIntervals(matching_intervals, super_interval.sublist, start, end);
    --i;
  }
  i = index + 1;
  while (i < super_intervals_.size()) {
    SuperInterval& super_interval = super_intervals_[i];
    if (super_interval.chromosome != chromosome ||
        super_interval.start > end) {
      break;
    }
    AddIntervals(matching_intervals, super_interval.sublist, start, end); 
    i++;
  }
  return matching_intervals;
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
