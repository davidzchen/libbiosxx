/**
 *    \file bgrParser.h
 *    \author Andrea Sboner (andrea.sboner@yale.edu)
 */
#ifndef BIOINFO_BEDGRAPH_H__
#define BIOINFO_BEDGRAPH_H__

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

#include "format.h"
#include "log.h"
#include "linestream.h"

class BedGraph {
 public:
  BedGraph();
  ~BedGraph();

  std::string chromosome() const { return chromosome_; }
  int start() const { return start_; }
  int end() const { return end_; }
  double value() const { return value_; }

  void set_chromosome(std::string chromosome) { chromosome_ = chromosome; }
  void set_start(int start) { start_ = start; }
  void set_end(int end) { end_ = end; }
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
  int start_;
  int end_;
  double value_;
};

class BedGraphParser {
 public:
  BedGraphParser();
  ~BedGraphParser();

  void InitFromFile(std::string filename);
  void InitFromPipe(std::string command);

  BedGraph* NextEntry();
  std::vector<BedGraph> GetAllEntries();

  static std::vector<double> GetValuesForRegion(
      std::vector<BedGraph>& bed_graphs, std::string chromosome, int start, 
      int end);

 private:
  LineStream stream_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif // BIOINFO_BEDGRAPH_H__
