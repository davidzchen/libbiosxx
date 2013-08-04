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

/// @file geneontology.hh
/// @author Lukas Habegger <lukas.habegger@yale.edu>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the gene ontology (GO) module.
/// See http://www.geneontology.org/GO.format.obo-1_2.shtml for details.

#ifndef BIOS_GENEONTOLOGY_H__
#define BIOS_GENEONTOLOGY_H__

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <stdint.h>

#include <gsl/gsl_randist.h>

#include "worditer.hh"
#include "string.hh"
#include "linestream.hh"

namespace bios {

/// @struct GoTagValue
/// @brief Structure representing a Gene Ontology tag value.
struct GoTagValue {
  char* tag;
  char* value;
};

/// @struct GoTerm
/// @brief Structure representing a Gene Ontology term.
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

/// @struct GoNode
/// @brief structure for representing a Gene Ontology node.
struct GoNode {
  GoNode();
  ~GoNode();

  static bool CompareById(const GoNode& a, const GoNode& b) {
    return (strcmp(a.id.c_str(), b.id.c_str()) < 0);
  }

  bool operator==(const GoNode& other) {
    return id == other.id;
  }

 /// @brief Get the children of a GoNode at a specified hiearchy level. 
 ///
 /// @param[in] currGoNode Get the children of this GoNode at the specified 
 ///            hiearchy level.
 /// @param[in] level Specifies the level within the gene ontology relative to 
 ///            the currGoNode. 
 ///  The level of currGoNode is 0.
 /// Example: To get the grand-children of currGoNode the level would be 2.
 /// @return An Array of GoNode pointers.
 /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
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

/// @struct GoGeneAssociation
/// @brief Structure for representing a Gene Ontology association.
struct GoGeneAssociation {
  GoGeneAssociation() {}
  ~GoGeneAssociation() {}

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

/// @struct GoStatistic
/// @brief Structure for representing a Gene Ontology statistic.
struct GoStatistic {
  GoStatistic() {}
  ~GoStatistic() {}

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

/// @class GeneOntology
/// @brief Class for processing a Gene Ontology file.
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

  /// @brief Get the GoNodes that are part of the biological_process hierarchy.
  ///
  /// @return An Array of GoNode pointers
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  std::vector<GoNode*> GetBiologicalProcessGoNodes();

  /// @brief Get the GoNodes that are part of the molecular_function hierarchy.
  ///
  /// @return An Array of GoNode pointers
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  std::vector<GoNode*> GetMolecularFunctionGoNodes();

  /// @brief Get the GoNodes that are part of the cellular_component hierarchy.
  ///
  /// @return An Array of GoNode pointers
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  std::vector<GoNode*> GetCellularComponentGoNodes();
 
  /// @brief Get the GoNodes of the entire ontology.
  ///
  /// @return An Array of GoNode pointers
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  std::vector<GoNode*> GetAllGoNodes();

  /// @brief Get all generic slim nodes.
  ///
  /// @return a vector of GoNode pointers
  std::vector<GoNode*> GetGenericGoSlimNodes();

  /// Get the number of associated genes.
  ///
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  /// @pre The gene annotations have been mapped to the gene ontology. 
  /// See GeneOntology::mapGeneAnnotationsToGeneOntology()
  int GetNumberOfAssociatedGenes();

  /// @brief Get the number of genes of interest that have been mapped to the 
  /// gene ontology.
  /// Note: duplicates gene names were removed.
  ///
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  /// @pre The gene annotations have been mapped to the gene ontology. 
  /// See GeneOntology::mapGeneAnnotationsToGeneOntology()
  /// @pre The genes of interest have been mapped to the gene ontology. 
  /// See GeneOntology::mapGenesOfInterestToGeneOntology()
  int GetNumberOfGenesOfInterest();

  /// Resets (performs arrayClear()) the genes of interest for every GoNode.
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  /// @pre The gene annotations have been mapped to the gene ontology. 
  /// See GeneOntology::mapGeneAnnotationsToGeneOntology()
  /// @pre The genes of interest have been mapped to the gene ontology. 
  /// See GeneOntology::mapGenesOfInterestToGeneOntology()
  /// @post A new set of genes of interest can be mapped to the gene ontology 
  /// by using GeneOntology::mapGenesOfInterestToGeneOntology()
  void ResetGenesOfInterest();

  /// @brief Find a GoNode.
  ///
  /// @param[in] id A GO term identifier. Example: GO:0065003
  /// @return A pointer to the GoNode, NULL if the GO term was not found
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  GoNode* FindGoNode(const char* id);

  /// @brief Find a GoGeneAssociation.
  /// @param[in] geneName A geneName or geneSymbol as specified in the 
  ///            geneAssociation file. See 
  ///            http://www.geneontology.org/GO.format.annotation.shtml for 
  ///            details. This geneName refers to the third column labeled as 
  ///            DB_Object_Symbol.
  /// @return A pointer to the GoGeneAssociation, NULL if GoGeneAssociation was 
  ///         not found 
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  /// @pre The gene annotations have been mapped to the gene ontology. 
  /// See GeneOntology::mapGeneAnnotationsToGeneOntology()
  GoGeneAssociation* FindGoGeneAssociation(const char* gene_name);

  /// @brief Map genes of interest to the gene ontology.
  ///
  /// @param[in] genesNamesOfInterest Texta that contains the gene names of the 
  ///            genes of interest. The gene names must be consistent with the 
  ///            genes names found in the gene annotation file.  
  /// @return Texta of invalid gene names. If the number of invalid gene names is
  ///         zero, then an empty Texta is returned. 
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  /// @pre The gene annotations have been mapped to the gene ontology. 
  /// See GeneOntology::mapGeneAnnotationsToGeneOntology()
  std::vector<std::string> MapGenesOfInterestToGeneOntology(
      std::vector<std::string>& genes_of_interest);

  /// Map gene annotations to the gene ontology.
  /// @param[in] goGeneAssociationFileName File that maps the genes of a specific 
  ///            organism to the gene ontology. 
  /// This file has to be in GO Annotation format. See 
  /// http://www.geneontology.org/GO.format.annotation.shtml for details.
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  void MapGeneAnnotationsToGeneOntology(
      const char* go_gene_association_file_Name);

  /// Calculate the gene enrichment for every GoNode in goNodePointers.
  /// @param[in] goNodePointers Array of goNode pointers. Gene enrichment is 
  ///            calculated for every goNode in this Array.
  /// @param[in] multipleTestingCorrectionMethod Specifies the multiple testing 
  ///            correction method. Use either 
  ///            kMultipleTestingCorrectionBenjaminiHochberg or 
  ///            kMultipleTestingCorrectionBonferroni.
  /// @return Array of type GoStatistic. This array is sorted by pvalue in 
  ///         ascending order.
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  /// @pre The gene annotations have been mapped to the gene ontology. 
  /// See GeneOntology::mapGeneAnnotationsToGeneOntology()
  /// @pre The genes of interest have been mapped to the gene ontology. 
  /// See GeneOntology::mapGenesOfInterestToGeneOntology()
  std::vector<GoStatistic> CalculateGeneEnrichment(
      std::vector<GoNode*> go_node_pointers, 
      int multiple_testing_correction_method);

  /// Calculate the gene depletion for every GoNode in goNodePointers.
  /// @param[in] goNodePointers Array of goNode pointers. Gene depletion is 
  ///            calculated for every goNode in this Array.
  /// @param[in] multipleTestingCorrectionMethod Specifies the multiple testing 
  ///            correction method. Use either 
  ///            kMultipleTestingCorrectionBenjaminiHochberg or 
  ///            kMultipleTestingCorrectionBonferroni.
  /// @return Array of type GoStatistic. This array is sorted by pvalue in 
  ///         ascending order.
  /// @pre The geneOntolgy module has been initialized. See GeneOntology::init()
  /// @pre The gene annotations have been mapped to the gene ontology. 
  /// See GeneOntology::mapGeneAnnotationsToGeneOntology()
  /// @pre The genes of interest have been mapped to the gene ontology. 
  /// See GeneOntology::mapGenesOfInterestToGeneOntology()
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

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_GENEONTOLOGY_H__ */
