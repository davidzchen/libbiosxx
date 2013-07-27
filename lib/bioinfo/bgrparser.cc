/** 
 *   \file bgrParser.c Module to parse BedGraphs files
 *   \author Andrea Sboner (andrea.sboner@yale.edu)
 */

#include "format.h"
#include "log.h"
#include "linestream.h"
#include "common.h"
#include "bgrparser.h"

BedGraphParser::BedGraphParser() {
  stream_ = NULL;
}

BedGraphParser::~BedGraphParser() {
  ls_destroy(stream_);
}

/**
 * Initialize the bgrParser module from file.
 * @param[in] fileName File name, use "-" to denote stdin
 */
void BedGraphParser::InitFromFile(std::string filename) {  
  stream_ = ls_createFromFile(filename.c_str());
  ls_bufferSet(stream_, 1);
}

/**
 * Initialize the bgrParser module from pipe.
 * @param[in] command Command to be executed
 */
void BedGraphParser::InitFromPipe(std::string command) {
  stream_ = ls_createFromPipe((char*) command.c_str());
  ls_bufferSet(stream_, 1);
}

/**
 * Retrieve the next entry in the bedGraph file.
 */
BedGraph* BedGraphParser::NextEntry() {
  while (!ls_isEof(stream_)) {
    char* line = ls_nextLine(stream_);
    if (!strStartsWithC(line, "track")) {
      BedGraph* bed_graph = new BedGraph();
      WordIter w = wordIterCreate(line, (char*) "\t", 1);
      std::string chromosome(wordNext(w));
      bed_graph->set_chromosome(chromosome);
      bed_graph->set_start(atoi(wordNext(w)));
      bed_graph->set_end(atoi(wordNext(w)));
      bed_graph->set_value(atof(wordNext(w)));
      wordIterDestroy(w);
      return bed_graph;
    }
  }
  return NULL;
}

/**
 * Retrieve all entries from a bedGraph file.
 */
std::vector<BedGraph> BedGraphParser::GetAllEntries() {
  std::vector<BedGraph> bed_graphs = std::vector<BedGraph>();
  BedGraph* bed_graph;
  int i = 0;
  while (bed_graph = NextEntry()) {
    bed_graphs.push_back(*bed_graph);
    delete bed_graph;
    ++i;
  }
  return bed_graphs;
}

/**
 * Get BedGraph values for a specified region.
 */
std::vector<double> BedGraphParser::GetValuesForRegion(
    std::vector<BedGraph>& bed_graphs, std::string chromosome, int start, 
    int end) {
  std::vector<double> entries = std::vector<double>();
  std::vector<BedGraph*> bed_graph_ptrs = std::vector<BedGraph*>();
  
  BedGraph test_bed_graph;
  test_bed_graph.set_chromosome(chromosome);
  test_bed_graph.set_start(start);
  test_bed_graph.set_end(end);
  std::vector<BedGraph>::iterator it =
    std::find(bed_graphs.begin(), bed_graphs.end(), test_bed_graph);
  int index = std::distance(bed_graphs.begin(), it);
  int i = index;
  while (i >= 0) {
    BedGraph* bed_graph = &bed_graphs[i];
    if (chromosome != bed_graph->chromosome() || bed_graph->end() < start) {
      break;
    }
    bed_graph_ptrs.push_back(bed_graph);
    --i;
  }
  i = index + 1;
  while (i < bed_graphs.size()) {
    BedGraph* bed_graph = &bed_graphs[i];
    if (chromosome != bed_graph->chromosome() || bed_graph->start() > end) {
      break;
    }
    bed_graph_ptrs.push_back(bed_graph);
    i++;
  }
  for (int i = start; i < end; i++) {
    int num_occurances = 0;
    for (int j = 0; j < bed_graph_ptrs.size(); ++j) {
      BedGraph* bed_graph = bed_graph_ptrs[i];
      if (bed_graph->start() <= i && i < bed_graph->end()) {
        ++num_occurances;
        entries.push_back(bed_graph->value());
      } 
    }
    if (num_occurances > 1) {
      die((char*) "Expected only one BedGraph overlap per position");
    }
  }
  return entries;
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
