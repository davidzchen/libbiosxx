/**
 *   \file intervalFind.h
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOINFO_INTERVAL_H__
#define BIOINFO_INTERVAL_H__

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "log.h"
#include "format.h"
#include "linestream.h"
#include "number.h"

struct SubInterval {
  int start;
  int end;
};

struct Interval {
  Interval();
  Interval(const char* line, int source);
  ~Interval();

  std::string ToString();
  int GetSize();

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

struct SuperInterval {
  SuperInterval();
  ~SuperInterval();

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

class IntervalFind {
 public:
  IntervalFind();
  ~IntervalFind();

  int GetNumberOfIntervals();
  std::vector<Interval> GetAllIntervals();
  std::vector<Interval*> GetIntervalPointers();
  void AddIntervalsToSearchSpace(const char* filename, int source);
  std::vector<Interval*> GetOverlappingIntervals(char* chromosome, int start,
                                                 int end);

  static std::vector<Interval> ParseFile(const char* filename, int source);
  static void ProcessCommaSeparatedList(std::vector<int>& results, const char* str);

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

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_INTERVAL_H__ */
