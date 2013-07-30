/**
 *   \file geneOntology.h
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOINFO_GENEONTOLOGY_H__
#define BIOINFO_GENEONTOLOGY_H__

#include <vector>
#include <string>
#include <algorithm>

#include <gsl/gsl_randist.h>

#include "log.h"
#include "format.h"
#include "linestream.h"

struct GoTagValue {
  char* tag;
  char* value;
};

struct GoTerm {
  std::string id;
  std::string name;
  std::string name_space;
  std::vector<std::string> alt_ids;
  std::string definition;
  std::vector<std::string> synonyms;
  std::vector<std::string> subsets;
  bool is_generic_go_slim;
  std::string comment;
  bool is_obsolete;
  std::vector<std::string> considers;
  std::vector<GoTagValue> xrefs;
  std::vector<GoTagValue> relationships;
  std::vector<std::string> parents;
};

struct GoNode {
  GoNode();
  ~GoNode();

  static bool CompareById(const GoNode& a, const GoNode& b) {
    return (strcmp(a.id.c_str(), b.id.c_str()) < 0);
  }

  bool operator==(const GoNode& other) {
    return id == other.id;
  }

  std::vector<GoNode*> GetChildrenAtSpecifiedHierarchyLevel(int level);
  void GetChildrenAtHierarchyLevel(std::vector<GoNode*>& result_nodes,
                                   int current_level, int specified_level);

 public:
  std::string id;
  GoTerm* go_term;
  std::vector<GoNode*> parents;
  std::vector<GoNode*> children;
  std::vector<std::string> associated_genes;
  std::vector<std::string> genes_of_interest;
};

struct GoGeneAssociation {
  GoGeneAssociation();
  ~GoGeneAssociation();

  static bool CompareByGeneName(const GoGeneAssociation& a, 
                                const GoGeneAssociation& b) {
    return a.gene_name < b.gene_name;
  }

  bool operator==(const GoGeneAssociation& other) {
    return gene_name == other.gene_name;
  }

  std::string db;
  std::string db_gene_name;
  std::string gene_name;
  std::vector<std::string> go_ids;
};

struct GoStatistic {
  GoStatistic();
  ~GoStatistic();

  static bool CompareByPvalue(const GoStatistic& a, const GoStatistic& b) {
    return a.pvalue < b.pvalue;
  }

  GoNode* go_node;
  // Names of the genes of interest of goNode and its children
  std::vector<std::string> genes_of_interest; 
  int number_of_annotated_genes;
  int number_of_genes_of_interest;
  double pvalue;
  double pvalue_corrected;
};

class GeneOntology {
 public:
  GeneOntology(const char* go_filename);
  ~GeneOntology();

  GoNode* biological_process_root() const { return biological_process_root_; }
  GoNode* molecular_function_root() const { return molecular_function_root_; }
  GoNode* cellular_component_root() const { return cellular_component_root_; }

  enum {
    kMultipleTestingCorrectionBenjaminiHochberg = 1,
    kMultipleTestingCorrectionBonferroni = 2
  };

  std::vector<GoNode*> GetBiologicalProcessGoNodes();
  std::vector<GoNode*> GetMolecularFunctionGoNodes();
  std::vector<GoNode*> GetCellularComponentGoNodes();
  std::vector<GoNode*> GetGenericGoSlimNodes();
  std::vector<GoNode*> GetAllGoNodes();
  int GetNumberOfAssociatedGenes();
  int GetNumberOfGenesOfInterest();
  void ResetGenesOfInterest();
  GoNode* FindGoNode(const char* id);
  GoGeneAssociation* FindGoGeneAssociation(const char* gene_name);
  std::vector<std::string> MapGenesOfInterestToGeneOntology(
      std::vector<std::string>& genes_of_interest);
  void MapGeneAnnotationsToGeneOntology(
      const char* go_gene_association_file_Name);
  std::vector<GoStatistic> CalculateGeneEnrichment(
      std::vector<GoNode*> go_node_pointers, 
      int multiple_testing_correction_method);
  std::vector<GoStatistic> CalculateGeneDepletion(
      std::vector<GoNode*> go_node_pointers, 
      int multiple_testing_correction_method);
  
 private:
  struct GoGeneAssociationEntry {
    static bool CompareByGeneName(const GoGeneAssociationEntry& a, 
                                  const GoGeneAssociationEntry& b) {
      return a.gene_name < b.gene_name;
    }

    std::string db;
    std::string db_gene_name;
    std::string gene_name;
    std::string go_id;
  };

  enum {
    kAnalysisModeGeneEnrichment = 1,
    kAnalysisModeGeneDepletion = 2
  };

  std::vector<GoNode*> GetGoNodesByNameSpace(std::string name_space);
  void ReadGoOntology(const char* obo_filename); 
  void AddGoNode(std::vector<GoNode*>& go_nodes, GoNode* go_node);
  void ConvertGoTermsToGoNodes();
  void ReadGoAnnotations(const char* go_annotation_filename);
  void MapAnnotatedGenesToGoOntology();
  void CountGenes(GoNode* n, std::vector<std::string>& go_node_annotated_genes,
                  std::vector<std::string>& go_node_genes_of_interest);

  static double CalculatePvalueForEnrichment(int k, int n1, int n2, int t);
  static double CalculatePvalueForDepletion(int k, int n1, int n2, int t);

  std::vector<std::string> CalculateGeneEnrichmentOrDepletionForGoTerm(
      GoNode* go_node, int* number_of_annotated_genes,
      int* number_of_genes_of_interest, double* pvalue, int analysis_mode);
  std::vector<GoStatistic> CalculateGeneEnrichmentOrDepletion(
      std::vector<GoNode*> go_node_pointers, 
      int multiple_testing_correction_method, int analysis_mode);

 private:
  std::vector<GoTerm> go_terms_;
  std::vector<GoNode> go_nodes_;
  std::vector<GoNode*> generic_go_slim_nodes_;
  std::vector<GoGeneAssociation> go_gene_associations_;
  std::vector<std::string> genes_of_interest_;
  GoNode* biological_process_root_;
  GoNode* molecular_function_root_;
  GoNode* cellular_component_root_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_GENEONTOLOGY_H__ */
