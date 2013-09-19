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
/// Module to efficiently find intervals that overlap with a query interval.

#include "interval.hh"

namespace bios {

Interval::Interval() {
}

Interval::Interval(std::string& line, int source) {
  WordIter w(line, "\t", false);
  source = source;
  name = w.Next();
  chromosome = w.Next();
  strand = w.Next()[0];
  start = atoi(w.Next());
  end = atoi(w.Next());
  sub_interval_count = atoi(w.Next());
  std::vector<int> starts;
  std::vector<int> ends;
  std::string starts_str = w.Next();
  std::string ends_str = w.Next();
  IntervalFind::ProcessCommaSeparatedList(starts, starts_str);
  IntervalFind::ProcessCommaSeparatedList(ends, ends_str);
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
}

Interval::~Interval() {
  sub_intervals.clear();
}

std::string Interval::ToString() {
  std::stringstream string_buffer;
  string_buffer << name << "\t";
  string_buffer << chromosome << "\t";
  string_buffer << strand << "\t";
  string_buffer << start << "\t";
  string_buffer << end << "\t";
  string_buffer << sub_interval_count << "\t";
  for (uint32_t i = 0; i < sub_intervals.size(); ++i) {
    string_buffer << sub_intervals[i].start;
    string_buffer << ((i < sub_intervals.size() - 1) ? "," : "\t");
  }
  for (uint32_t i = 0; i < sub_intervals.size(); ++i) {
    string_buffer << sub_intervals[i].end;
    string_buffer << ((i < sub_intervals.size() - 1) ? "," : "\t");
  }
  return string_buffer.str();
}

uint32_t Interval::GetSize() {
  uint32_t size = 0;
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

int IntervalFind::GetNumberOfIntervals() {
  return intervals_.size();
}

std::vector<Interval> IntervalFind::GetAllIntervals() {
  return std::vector<Interval>(intervals_);
}

std::vector<Interval*> IntervalFind::GetIntervalPointers() {
  std::vector<Interval*> intervals;
  for (std::vector<Interval>::iterator it = intervals_.begin();
      it != intervals_.end(); ++it) {
    intervals.push_back(&(*it));
  }
  return intervals;
}

void IntervalFind::ProcessCommaSeparatedList(std::vector<int>& results, 
                                             std::string& str) {
  WordIter w(str, ",", false);
  for (char* tok = NULL; (tok = w.Next()) != NULL; ) {
    if (tok[0] == '\0') {
      continue;
    }
    results.push_back(atoi(tok));
  }
}

void IntervalFind::ParseFileContent(std::vector<Interval>& intervals, 
                                    const char* filename, int source) {
  FileLineStream ls(filename);
  for (std::string line; ls.GetLine(line); ) {
    if (line.empty()) {
      continue;
    }
    Interval interval(line, source);
    intervals.push_back(interval);
  }
}

void IntervalFind::AddIntervalsToSearchSpace(const char* filename, 
                                             int source) {
  ParseFileContent(intervals_, filename, source);
}

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
  uint32_t i = 0;
  while (i < intervals_.size()) {
    Interval& interval = intervals_[i];
    SuperInterval super_interval;
    super_interval.chromosome = interval.chromosome;
    super_interval.start = interval.start; 
    super_interval.end = interval.end;
    super_interval.sublist.push_back(&interval);
    uint32_t j = i + 1;
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
  uint32_t i = index;
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

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
