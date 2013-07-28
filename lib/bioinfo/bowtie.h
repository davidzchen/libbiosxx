/**
 *   \file bowtieParser.h
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOINFO_BOWTIE_H__
#define BIOINFO_BOWTIE_H__

#include <vector>
#include <string>

struct BowtieMismatch {
  int offset;
  char reference_base;
  char read_base;
};

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

class BowtieParser {
 public:
  BowtieParser();
  ~BowtieParser();

  void InitFromFile(std::string filename);
  void InitFromPipe(std::string command);

  BowtieQuery* NextQuery();
  std::vector<BowtieQuery> GetAllQueries();

 private:
  BowtieQuery* ProcessNextQuery();

 private:
  LineStream stream_;
  BowtieQuery* bowtie_query_;
  std::string query_name_;
  std::string prev_query_name_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_BOWTIE_H__ */
