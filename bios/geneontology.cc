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

/// @file geneontology.cc
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the Gene Ontology module.

#include "geneontology.hh"

namespace bios {

GoNode::GoNode() {
}

GoNode::~GoNode() {
}

void GoNode::GetChildrenAtHierarchyLevel(std::vector<GoNode*>& result_nodes, 
                                         int current_level, 
                                         int specified_level) {
  if (current_level == specified_level) {
    uint32_t i = 0; 
    while (i < result_nodes.size()) {
      GoNode* node = result_nodes[i];
      if (node == this) {
        break;
      }
      ++i;
    }
    if (i == result_nodes.size()) {
      result_nodes.push_back(this);
    }
    return;
  }
  if (children.empty() == false) {
    for (std::vector<GoNode*>::iterator it = children.begin(); 
        it != children.end(); ++it) {
      GoNode* node = *it;
      node->GetChildrenAtHierarchyLevel(result_nodes, current_level + 1, 
                                        specified_level);
    }
  }
}

std::vector<GoNode*> GoNode::GetChildrenAtSpecifiedHierarchyLevel(int level) {
  std::vector<GoNode*> result_nodes;
  GetChildrenAtHierarchyLevel(result_nodes, 0, level);
  return result_nodes;
}

GeneOntology::GeneOntology(const char* go_filename) {
  ReadGoOntology(go_filename);
  ConvertGoTermsToGoNodes(); 
}

std::vector<GoNode*> GeneOntology::GetGoNodesByNameSpace(
    std::string name_space) {
  std::vector<GoNode*> subset_go_nodes;
  for (std::vector<GoNode>::iterator it = go_nodes_.begin();
      it != go_nodes_.end(); ++it) {
    GoNode& go_node = *it;
    if (go_node.go_term->name_space == name_space || name_space == "all") {
      subset_go_nodes.push_back(&go_node);
    }
  }
  return subset_go_nodes;
}

std::vector<GoNode*> GeneOntology::GetBiologicalProcessGoNodes() {
  return GetGoNodesByNameSpace("biological_process");
}

std::vector<GoNode*> GeneOntology::GetMolecularFunctionGoNodes() {
  return GetGoNodesByNameSpace("molecular_function");
}

std::vector<GoNode*> GeneOntology::GetCellularComponentGoNodes() {
  return GetGoNodesByNameSpace("cellular_component");
}

std::vector<GoNode*> GeneOntology::GetGenericGoSlimNodes() {
  return generic_go_slim_nodes_;
}

std::vector<GoNode*> GeneOntology::GetAllGoNodes() {
  return GetGoNodesByNameSpace("all");
}

int GeneOntology::GetNumberOfAssociatedGenes() {
  return go_gene_associations_.size();
}

int GeneOntology::GetNumberOfGenesOfInterest(){
  return genes_of_interest_.size();
}

void GeneOntology::ResetGenesOfInterest() {
  for (std::vector<GoNode>::iterator it = go_nodes_.begin();
      it != go_nodes_.end(); ++it) {
    GoNode& go_node = *it;
    go_node.genes_of_interest.clear();
  }
}

void GeneOntology::ReadGoOntology(const char* obo_filename) {
  FileLineStream ls(obo_filename);
  bool go_term_found = false;
  GoTerm* go_term = NULL;
  for (std::string line; ls.GetLine(line); ) {
    if (line == "[Term]") {
      if (go_term_found == true) {
        go_terms_.push_back(*go_term);
        delete go_term;
      }
      go_term_found = true;
      continue;
    } else if (line[0] == '\0') {
      if (go_term_found == true) {
        go_terms_.push_back(*go_term);
        delete go_term;
      }
      go_term_found = false;
      continue;
    }
    if (go_term_found == false) {
      continue;
    }

    go_term = new GoTerm;
    if (string::starts_with(line, "id:")) {
      size_t pos = line.find(' ');
      go_term->id = line.substr(pos + 1, line.size() - pos);
    } else if (string::starts_with(line, "name:")) {
      size_t pos = line.find(' ');
      go_term->name = line.substr(pos + 1, line.size() - pos);
    } else if (string::starts_with(line, "namespace:")) {
      size_t pos = line.find(' ');
      go_term->name_space = line.substr(pos + 1, line.size() - pos);
    } else if (string::starts_with(line, "alt_id:")) {
      size_t pos = line.find(' ');
      std::string alt_id = line.substr(pos + 1, line.size() - pos);
      go_term->alt_ids.push_back(alt_id);
    } else if (string::starts_with(line, "def:")) {
      size_t pos = line.find(" [");
      if (pos == std::string::npos) {
        std::cerr << "Expected ' [' in def line: " << line << std::endl;
        return;
      }
      std::string definition = line.substr(0, pos);
      pos = definition.find(' ');
      string::trim_chars(pos1, " \"", " \"");
      go_term->definition = pos1;
    } else if (string::starts_with(line, "subset:")) {
      size_t pos = line.find(' ');
      std::string subset = line.substr(pos + 1, line.size() - pos);
      go_term->subsets.push_back(subset);
      if (subset == "goslim_generic") {
        go_term->is_generic_go_slim = true;
      }
    } else if (string::starts_with(line, "comment:")) {
      size_t pos = line.find(' ');
      go_term->comment = line.substr(pos + 1, line.size() - pos);
    } else if (line == "is_obsolete: true") {
      go_term->is_obsolete = true;
    } else if (string::starts_with(line, "synonym:")) {
      size_t pos0 = line.find('"');
      size_t pos1 = line.rfind('"');
      std::string synonym = line.substr(pos0, pos1 - pos0 + 1);
      go_term->synonyms.push_back(synonym);
    } else if (string::starts_with(line, "consider:")) {
      size_t pos = line.find(' ');
      std::string consider = line.substr(pos, line.size() - pos);
      go_term->considers.push_back(consider);
    } else if (string::starts_with(line, "xref:")) {
      size_t pos0 = line.find(' ');
      size_t pos1 = line.find(':', pos0 + 1);
      GoTagValue tag_value;
      tag_value.tag = line.substr(pos0 + 1, pos1 - pos0 + 1);
      tag_value.value = line.substr(pos1 + 1, line.size() - pos1);
      go_term->xrefs.push_back(tag_value);
    } else if (string::starts_with(line, "is_a:")) {
      size_t pos = line.find('!');
      size_t bound = line.size();
      if (pos == std::string::npos) {
        bound = pos;
      }
      pos = line.find(' ');
      std::string parent = line.substr(pos + 1, bound - pos);
      go_term->parents.push_back(parent);
    } else if (string::starts_with(line, "relationship:")) {
      size_t pos0 = line.find('!');
      size_t bound = line.size();
      if (pos0 == std::string::npos) {
        bound = pos0;
      }
      pos0 = line.find(' ');
      size_t pos1 = line.find(' ', pos0 + 1);
      GoTagValue tag_value;
      tag_value.tag = line.substr(pos0 + 1, bound - pos0);
      tag_value.value = line.substr(pos1 + 1, line.size() - pos1);
      go_term->relationships.push_back(tag_value);
    } else if (string::starts_with(line, "replaced_by:")) {
      // Do nothing.
    } else if (string::starts_with(line, "disjoint_from:")) {
      // Do nothing.
    } else {
      std::cerr << "Unexpected line: " << line << std::endl;
    }
  }
}

GoNode* GeneOntology::FindGoNode(const char* id) {
  GoNode test_go_node;
  test_go_node.id = id;
  std::vector<GoNode>::iterator it = std::find(go_nodes_.begin(),
                                               go_nodes_.end(), test_go_node);
  if (it == go_nodes_.end()) {
    return NULL;
  }
  return &(*it);
}

void GeneOntology::AddGoNode(std::vector<GoNode*>& go_nodes, GoNode* go_node) {
  std::vector<GoNode*>::iterator it = std::find(go_nodes.begin(), 
                                                go_nodes.end(), go_node);
  if (it == go_nodes.end()) {
    return;
  }
  go_nodes.push_back(go_node);
}

void GeneOntology::ConvertGoTermsToGoNodes() {
  for (std::vector<GoTerm>::iterator it = go_terms_.begin(); 
      it != go_terms_.end(); ++it) {
    GoTerm& go_term = *it;
    if (go_term.is_obsolete == true) {
      continue;
    }
    GoNode go_node;
    go_node.go_term = &go_term;
    go_node.id = go_term.id;
    if (go_node.go_term->is_generic_go_slim == true) {
      generic_go_slim_nodes_.push_back(&go_node);
    }
    if (go_node.go_term->name == "biological_process") {
      biological_process_root_ = &go_node;
    } else if (go_node.go_term->name == "molecular_function") {
      molecular_function_root_ = &go_node;
    } else if (go_node.go_term->name == "cellular_component") {
      cellular_component_root_ = &go_node;
    }
    go_nodes_.push_back(go_node);
  }
  std::sort(go_nodes_.begin(), go_nodes_.end(), GoNode::CompareById);
  for (std::vector<GoNode>::iterator it = go_nodes_.begin();
      it != go_nodes_.end(); ++it) {
    GoNode& go_node = *it;
    if (go_node.go_term->parents.empty() == true) {
      continue;
    }
    for (uint32_t i = 0; i < go_node.go_term->parents.size(); i++) {
      GoNode* parent_go_node = FindGoNode(go_node.go_term->parents[i].c_str());
      if (parent_go_node == NULL) {
        std::cerr << "Expected to find %s in GO ontology: "
                  << go_node.go_term->parents[i].c_str() << std::endl;
        return;
      }
      AddGoNode(go_node.parents, parent_go_node);
      AddGoNode(parent_go_node->children, &go_node);
    }
  }
}

void GeneOntology::ReadGoAnnotations(const char* annotation_filename) {
  std::vector<GoGeneAssociationEntry> association_entries;
  FileLineStream ls(annotation_filename);
  for (std::string line; ls.GetLine(line); ) {
    if (line[0] == '\0' || line[0] == '!') {
      continue;
    }
    GoGeneAssociationEntry association_entry;
    WordIter w(line, "\t", false);
    association_entry.db = w.Next();
    association_entry.db_gene_name = w.Next();
    association_entry.gene_name = w.Next();
    // optional column [qualifier], not considered
    w.Next(); 
    association_entry.go_id = w.Next();
    association_entries.push_back(association_entry);
  }
  std::sort(association_entries.begin(), association_entries.end(),
            GoGeneAssociationEntry::CompareByGeneName);
  uint32_t i = 0;
  while (i < association_entries.size()) {
    GoGeneAssociationEntry& association_entry = association_entries[i];
    GoGeneAssociation association;
    association.db = association_entry.db;
    association.db_gene_name = association_entry.db_gene_name;
    association.gene_name = association_entry.gene_name;
    association.go_ids.push_back(association_entry.go_id);
    uint32_t j = i + 1;
    while (j < association_entries.size()) {
      GoGeneAssociationEntry next_association_entry = association_entries[j];
      if (association_entry.gene_name == next_association_entry.gene_name) {
        association.go_ids.push_back(next_association_entry.go_id);
      } else {
        break;
      }
      j++;
    }
    i = j;
    std::unique(association.go_ids.begin(), 
                association.go_ids.end());
    go_gene_associations_.push_back(association);
  }
  std::sort(go_gene_associations_.begin(), go_gene_associations_.end(),
            GoGeneAssociation::CompareByGeneName);
}

void GeneOntology::MapAnnotatedGenesToGoOntology() {
  for (std::vector<GoGeneAssociation>::iterator it = 
      go_gene_associations_.begin(); it != go_gene_associations_.end(); ++it) {
    GoGeneAssociation& go_gene_association = *it;
    std::vector<std::string>& go_ids = go_gene_association.go_ids;
    for (std::vector<std::string>::iterator it = go_ids.begin();
        it != go_ids.end(); ++it) {
      std::string& id = *it;
      GoNode* go_node = FindGoNode(id.c_str());
      if (go_node == NULL) {
        std::cerr << "Expected to find %s in GO ontology: "
                  << id.c_str() << std::endl;
        return;
      }
      go_node->associated_genes.push_back(go_gene_association.gene_name);
    }
  }
}

GoGeneAssociation* GeneOntology::FindGoGeneAssociation(const char* gene_name) {
  GoGeneAssociation test_go_gene_association;
  test_go_gene_association.gene_name = gene_name;
  std::vector<GoGeneAssociation>::iterator it = std::find(
      go_gene_associations_.begin(), go_gene_associations_.end(), 
      test_go_gene_association);
  if (it == go_gene_associations_.end()) {
    return NULL;
  }
  return &(*it);
}

std::vector<std::string> GeneOntology::MapGenesOfInterestToGeneOntology(
    std::vector<std::string>& gene_names_of_interest) {
  std::vector<std::string> invalid_gene_names;
  std::unique(gene_names_of_interest.begin(), gene_names_of_interest.end());
  for (uint32_t i = 0; i < gene_names_of_interest.size(); ++i) {
    GoGeneAssociation* go_gene_association = FindGoGeneAssociation(
        gene_names_of_interest[i].c_str());
    if (go_gene_association != NULL) {
      genes_of_interest_.push_back(gene_names_of_interest[i]);
      for (uint32_t j = 0; j < go_gene_association->go_ids.size(); ++j) {
        GoNode* go_node = FindGoNode(go_gene_association->go_ids[j].c_str());
        if (go_node == NULL) {
          std::cerr << "Expected to find "
                    << go_gene_association->go_ids[j].c_str()
                    << " in GO ontology: " << "????" << std::endl;
          return invalid_gene_names;
        }
        go_node->genes_of_interest.push_back(gene_names_of_interest[j]);
      }
    } else {
      invalid_gene_names.push_back(gene_names_of_interest[i]);
    }
  }
  return invalid_gene_names;
}

void GeneOntology::MapGeneAnnotationsToGeneOntology(
    const char* go_gene_association_filename) {
  ReadGoAnnotations(go_gene_association_filename);
  MapAnnotatedGenesToGoOntology();
}

void GeneOntology::CountGenes(GoNode* n, 
    std::vector<std::string>& go_node_annotated_genes, 
    std::vector<std::string>& go_node_genes_of_interest) {
  if (n->children.empty() == false) {
    for (std::vector<GoNode*>::iterator it = n->children.begin();
        it != n->children.end(); ++it) {
      GoNode* go_node = *it;
      CountGenes(go_node, go_node_annotated_genes, go_node_genes_of_interest);
    }
  }
  if (n->associated_genes.empty() == false) {
    for (std::vector<std::string>::iterator it = n->associated_genes.begin();
        it != n->associated_genes.end(); ++it) {
      go_node_annotated_genes.push_back(*it);
    }
  }
  if (n->genes_of_interest.empty() == false) {
    for (std::vector<std::string>::iterator it = n->genes_of_interest.begin();
        it != n->genes_of_interest.end(); ++it) {
      go_node_genes_of_interest.push_back(*it);
    }
  }
}

double GeneOntology::CalculatePvalueForEnrichment(int k, int n1, int n2, 
                                                  int t) {
  double pvalue = 0.0;
  for (int i = k; i <= n1; ++i) {
    pvalue += gsl_ran_hypergeometric_pdf(i, n1, n2, t);
  }
  return pvalue;
}

double GeneOntology::CalculatePvalueForDepletion(int k, int n1, int n2, 
                                                 int t) {
  double pvalue = 0.0;
  for (int i = 0; i <= k; ++i) {
    pvalue += gsl_ran_hypergeometric_pdf(i, n1, n2, t);
  }
  return pvalue;
}

std::vector<std::string> GeneOntology::CalculateGeneEnrichmentOrDepletionForGoTerm(
    GoNode* go_node, int* number_of_annotated_genes, 
    int* number_of_genes_of_interest, double* pvalue, int analysis_mode) { 

  std::vector<std::string> go_node_annotated_genes;
  std::vector<std::string> go_node_genes_of_interest;

  CountGenes(go_node, go_node_annotated_genes, go_node_genes_of_interest);
  std::sort(go_node_annotated_genes.begin(), go_node_annotated_genes.end());
  std::sort(go_node_genes_of_interest.begin(), 
            go_node_genes_of_interest.end());
  std::unique(go_node_annotated_genes.begin(), go_node_annotated_genes.end());
  std::unique(go_node_genes_of_interest.begin(), 
              go_node_genes_of_interest.end());
  *number_of_annotated_genes = go_node_annotated_genes.size();
  *number_of_genes_of_interest = go_node_genes_of_interest.size();

  if (analysis_mode == kAnalysisModeGeneEnrichment) {
    *pvalue = CalculatePvalueForEnrichment(
        *number_of_genes_of_interest, *number_of_annotated_genes,
        go_gene_associations_.size() - (*number_of_annotated_genes),
        genes_of_interest_.size());
  } else if (analysis_mode == kAnalysisModeGeneDepletion) {
    *pvalue = CalculatePvalueForDepletion(
        *number_of_genes_of_interest, *number_of_annotated_genes,
        go_gene_associations_.size() - (*number_of_annotated_genes),
        genes_of_interest_.size());
  } else {
    std::cerr << "Unknown analysis mode: " << analysis_mode << std::endl;
    return go_node_genes_of_interest;
  }
  return go_node_genes_of_interest;
}

std::vector<GoStatistic> GeneOntology::CalculateGeneEnrichmentOrDepletion(
   std::vector<GoNode*> go_node_pointers, 
   int multiple_testing_correction_method, int analysis_mode) {
  std::vector<GoStatistic> go_statistics;
  for (std::vector<GoNode*>::iterator it = go_node_pointers.begin();
      it != go_node_pointers.end(); ++it) {
    GoNode *go_node = *it;
    double pvalue = 0.0;
    int number_of_annotated_genes = 0;
    int number_of_genes_of_interest = 0;
    std::vector<std::string> genes_of_interest;
    genes_of_interest = CalculateGeneEnrichmentOrDepletionForGoTerm(
        go_node, &number_of_annotated_genes, &number_of_genes_of_interest,
        &pvalue, analysis_mode);
    GoStatistic go_statistic;
    go_statistic.go_node = go_node;
    go_statistic.number_of_annotated_genes = number_of_annotated_genes;
    go_statistic.number_of_genes_of_interest = number_of_genes_of_interest;
    go_statistic.pvalue = pvalue;
    go_statistic.genes_of_interest = genes_of_interest;
    go_statistics.push_back(go_statistic);
  }
  std::sort(go_statistics.begin(), go_statistics.end(), 
            GoStatistic::CompareByPvalue);
  for (uint32_t i = 0; i < go_statistics.size(); ++i) {
    GoStatistic& go_statistic = go_statistics[i];
    if (multiple_testing_correction_method == 
        kMultipleTestingCorrectionBenjaminiHochberg) {
      go_statistic.pvalue_corrected = std::min<double>(
          go_statistic.pvalue * go_statistics.size() / (i + 1), 1.0); 
    } else if (multiple_testing_correction_method == 
        kMultipleTestingCorrectionBonferroni) {
      go_statistic.pvalue_corrected =  std::min<double>(
          go_statistic.pvalue * go_statistics.size(), 1.0);
    } else {
      std::cerr << "Unknown multiple testing correction method: "
                << multiple_testing_correction_method << std::endl;
      return go_statistics;
    }
  }
  return go_statistics;
}

std::vector<GoStatistic> GeneOntology::CalculateGeneEnrichment(
    std::vector<GoNode*> go_node_pointers, 
    int multiple_testing_correction_method) {
  return CalculateGeneEnrichmentOrDepletion(go_node_pointers, 
      multiple_testing_correction_method, kAnalysisModeGeneEnrichment);
}

std::vector<GoStatistic> GeneOntology::CalculateGeneDepletion(
    std::vector<GoNode*> go_node_pointers, 
    int multiple_testing_correction_method) {
  return CalculateGeneEnrichmentOrDepletion(go_node_pointers,
      multiple_testing_correction_method, kAnalysisModeGeneDepletion);
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
