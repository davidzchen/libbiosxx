/** 
 *   \file bedParser.c Module to parse BED files
 *   \author Andrea Sboner (ans2077@med.cornell.edu)
 */

#include "format.h"
#include "log.h"
#include "linestream.h"
#include "common.h"
#include "bedparser.h"

Bed::Bed() {
  sub_blocks_ = std::vector<SubBlock>();
}

Bed::~Bed() {
  sub_blocks_.clear();
}

std::string Bed::ToString() {
  std::stringstream string_buffer;
  SubBlock* sub_block;
  if (extended_) {
    string_buffer << chromosome_.c_str() << "\t";
    string_buffer << start_ << "\t";
    string_buffer << end_ << "\t";
    string_buffer << name_.c_str() << "\t";
    string_buffer << score_ << "\t";
    string_buffer << strand_ << "\t";
    string_buffer << thick_start_ << "\t";
    string_buffer << thick_end_ << "\t";
    string_buffer << item_rgb_.c_str() << "\t";
    string_buffer << block_count_ << "\t";
    for (int i = 0; i < sub_blocks_.size(); ++i) {
      string_buffer << sub_blocks_[i].size;
      string_buffer << (i < sub_blocks_.size() - 1) ? "," : "\t";
    }
    for (int i = 0; i < sub_blocks_.size(); ++i) {
      string_buffer << sub_blocks_[i].start;
      string_buffer << (i < sub_blocks_.size() - 1) ? "," : "";
    }
  } else {
    string_buffer << chromosome_.c_str() << "\t";
    string_buffer << start_ << "\t";
    string_buffer << end_;
  }
  return string_buffer.str();
}

BedParser::BedParser() {
  stream_ = NULL;
}

BedParser::~BedParser() {
  ls_destroy(stream_);
}

/**
 * Initialize the bedParser module from file.
 * @param[in] fileName File name, use "-" to denote stdin
 */
void BedParser::InitFromFile(std::string filename) {
  stream_ = ls_createFromFile((char*) filename.c_str());
  ls_bufferSet(stream_, 1);
}

/**
 * Initialize the bedParser module from pipe.
 * @param[in] command Command to be executed
 */
void BedParser::InitFromCommand(std::string command) {
  stream_ = ls_createFromPipe((char*) command.c_str());
  ls_bufferSet(stream_, 1);
}

/**
 * Retrieve the next entry in the BED file.
 */
Bed* BedParser::NextEntry(void) {
  char* line = NULL;
  while (ls_isEof(stream_)) {
    char *line = ls_nextLine(stream_);
    if (strStartsWithC(line, "track") || strStartsWithC(line, "browser")) {
      continue;
    }
    Bed* bed = new Bed();
    WordIter w = wordIterCreate(line, (char*) "\t", 1);
    std::string chromosome(wordNext(w));
    bed->set_chromosome(chromosome);
    bed->set_start(atoi(wordNext(w)));
    bed->set_end(atoi(wordNext(w)));
    char *next = wordNext(w);
    if (next != NULL) {
      std::string name(next);
      bed->set_name(name);
      bed->set_extended(true);
      bed->set_score(atoi(wordNext(w)));
      bed->set_strand(wordNext(w)[0]);
      bed->set_thick_start(atoi(wordNext(w)));
      bed->set_thick_end(atoi(wordNext(w)));
      std::string item_rgb(wordNext(w));
      bed->set_item_rgb(item_rgb);
      bed->set_block_count(atoi(wordNext(w)));
      WordIter wsizes = wordIterCreate(wordNext(w), (char*) ",", 1);
      WordIter wstarts = wordIterCreate(wordNext(w), (char*) ",", 1); 
      for (int i = 0; i < bed->block_count(); ++i) {
        SubBlock sub_block;
        sub_block.size = atoi(wordNext(wsizes));
        sub_block.start = atoi(wordNext(wstarts));
        bed->AddSubBlock(sub_block);
      }
      wordIterDestroy(wsizes);
      wordIterDestroy(wstarts);
    } else {
      bed->set_extended(false);
    }
    wordIterDestroy(w);
    return bed;
  }
  return NULL;
}

/**
 * Retrieve all entries from a Bed file.
 */
std::vector<Bed> BedParser::GetAllEntries() {
  std::vector<Bed> beds = std::vector<Bed>();
  Bed* current_bed;
  int i = 0;
  while (current_bed = NextEntry()) {
    beds.push_back(*current_bed);
    delete current_bed;
    ++i;
  }
  return beds;
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
