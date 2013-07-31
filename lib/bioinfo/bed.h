/**
 *    \file bedParser.h
 *    \author Andrea Sboner (ans2077@med.cornell.edu)
 */

#ifndef BIOINFO_BED_H__
#define BIOINFO_BED_H__

#include <string>
#include <sstream>
#include <vector>
#include <stdint.h>

#include "string.h"
#include "worditer.h"
#include "linestream.h"

struct SubBlock {
  uint32_t size;
  uint32_t start; // relative to Bed->start
};

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
  void set_thick_end(uint32_t thick_end) { thick_end_ = thick_end_; }
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
  
class BedParser {
 public:
  BedParser();
  ~BedParser();
  void InitFromFile(const char* filename);
  void InitFromCommand(const char* command);

  Bed* NextEntry();
  std::vector<Bed> GetAllEntries();

 private:
  LineStream* stream_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif // BIOINFO_BED_H__
