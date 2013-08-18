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

/// @file bedgraph.cc
/// @author David Z. Chen <d.zhekai.chen@gmail.com>
/// @version 
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for parsing BedGraphs files.

#include "bedgraph.hh"

namespace bios {

BedGraphParser::BedGraphParser() 
    : stream_(NULL) {
}

BedGraphParser::~BedGraphParser() {
  delete stream_;
}

void BedGraphParser::InitFromFile(const char* filename) {  
  stream_ = new FileLineStream(filename);
  stream_->SetBuffer(1);
}

void BedGraphParser::InitFromPipe(const char* command) {
  stream_ = new PipeLineStream(command);
  stream_->SetBuffer(1);
}

BedGraph* BedGraphParser::NextEntry() {
  for (std::string line; stream_->GetLine(line); ) {
    if (!string::starts_with(line, "track")) {
      BedGraph* bed_graph = new BedGraph();
      WordIter  w(line, "\t", true);
      std::string chromosome(w.Next());
      bed_graph->set_chromosome(chromosome);
      bed_graph->set_start(atoi(w.Next()));
      bed_graph->set_end(atoi(w.Next()));
      bed_graph->set_value(atof(w.Next()));
      return bed_graph;
    }
  }
  return NULL;
}

std::vector<BedGraph> BedGraphParser::GetAllEntries() {
  std::vector<BedGraph> bed_graphs = std::vector<BedGraph>();
  BedGraph* bed_graph;
  int i = 0;
  while ((bed_graph = NextEntry()) != NULL) {
    bed_graphs.push_back(*bed_graph);
    delete bed_graph;
    ++i;
  }
  return bed_graphs;
}

std::vector<double> BedGraphParser::GetValuesForRegion(
    std::vector<BedGraph>& bed_graphs, std::string chromosome, 
    uint32_t start, uint32_t end) {
  std::vector<double> entries = std::vector<double>();
  std::vector<BedGraph*> bed_graph_ptrs = std::vector<BedGraph*>();
  
  BedGraph test_bed_graph;
  test_bed_graph.set_chromosome(chromosome);
  test_bed_graph.set_start(start);
  test_bed_graph.set_end(end);
  std::vector<BedGraph>::iterator it = std::find(bed_graphs.begin(), 
                                                 bed_graphs.end(), 
                                                 test_bed_graph);
  uint32_t index = std::distance(bed_graphs.begin(), it);
  uint32_t i = index;
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
  for (uint32_t i = start; i < end; i++) {
    int num_occurances = 0;
    for (uint32_t j = 0; j < bed_graph_ptrs.size(); ++j) {
      BedGraph* bed_graph = bed_graph_ptrs[i];
      if (bed_graph->start() <= i && i < bed_graph->end()) {
        ++num_occurances;
        entries.push_back(bed_graph->value());
      } 
    }
    if (num_occurances > 1) {
      std::cerr << "Expected only one BedGraph overlap per position" 
                << std::endl;
      return entries;
    }
  }
  return entries;
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
