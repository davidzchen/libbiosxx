/*
 * Sequence module. 
 *
 * Assumes that DNA is stored as a character.
 * The DNA it generates will include the bases 
 * as lowercase tcag.  It will generally accept
 * uppercase as well, and also 'n' or 'N' or '-'
 * for unknown bases. 
 *
 * Amino acids are stored as single character upper case. 
 *
 * This file is copyright 2002 Jim Kent, but license is hereby
 * granted for all use - public, private or commercial. 
 */

/** 
 *   \file seq.h
 *   \author Adapted by Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOINFO_SEQ_H__
#define BIOINFO_SEQ_H__

#include <string>
#include <sstream>

#include "common.h"
#include "format.h"
#include "number.h"
#include "log.h"
#include "bitfield.h"

/**
 * Seq.
 */
struct Seq {
  Seq();
  ~Seq();
  BitField* MaskFromUpperCase();

  char *name;           /* Name of sequence. */
  char *sequence;       /* Sequence base by base. */
  int size;             /* Size of sequence. */
  BitField* mask;           /* Repeat mask (optional) */
}; 

typedef Seq dnaSeq;	/* Preferred use if DNA */
typedef Seq aaSeq;	/* Preferred use if protein. */

#define MASKED_BASE_BIT 8

/* Numerical values for bases. */
#define T_BASE_VAL 0
#define U_BASE_VAL 0
#define C_BASE_VAL 1
#define A_BASE_VAL 2
#define G_BASE_VAL 3
#define N_BASE_VAL 4   /* Used in 1/2 byte representation. */

typedef char DNA;
typedef char AA;
typedef char Codon; 

class Sequencer {
 public:
  static Sequencer& GetInstance() {
    static Sequencer instance;
    return instance;
  }

  aaSeq* TranslateSeqN(dnaSeq* in_seq, unsigned offset, unsigned in_size,
                       int stop);
  aaSeq* TranslateSeq(dnaSeq* in_seq, unsigned offset, int stop);
  void ToRna(DNA* dna);

 private:
  void DnaOrAaFilter(char* in, char* out, char filter[256]);

 public:
  void DnaFilter(char* in, DNA* out);
  void DnaMixedCaseFilter(char* in, DNA* out);
  void AaFilter(char* in, DNA* out);
  void DnaBaseHistogram(DNA* dna, int dna_size, int histogram[4]);
  int IntronOrientation(DNA* i_start, DNA* i_end);
  int DnaOrAaScoreMatch(char* a, char* b, int size, int match_score,
                        int mismatch_score, char ignore);
  AA LookupCodon(DNA* dna);
  AA LookupMitochondrialCodon(DNA* dna);
  Codon CodonVal(DNA* start);
  const DNA* ValToCodon(int val);
  char* DnaTranslate(DNA* dna, bool terminate_at_stop_codon);
  void Complement(DNA* dna, long length);
  void ReverseComplement(DNA* dna, long length);
  bool SeqIsLower(Seq* seq);

 private:
  Sequencer();
  Sequencer(const Sequencer&);
  void operator=(const Sequencer&);

 private:
  void InitNucleotideValues();

 private:
  struct CodonRow {
    const DNA* codon;   // Lower case.
    AA protein_code;    // Upper case. The "Standard" code.
    AA mito_code;       // Upper case. Vertibrate mitochondrial translations.
  };

  static Sequencer::CodonRow codon_table_[];

  // A table that gives values:
  //    0 for t
  //    1 for c
  //    2 for a
  //    3 for g
  // (which is order aa's are in biochemistry codon tables) and gives -1 for
  // all others.
  int nt_val_[256];
  // Nucleotide values only for lower case.
  int nt_val_lower_[256];
  // Nucleotide values only for up9per case.
  int nt_val_upper_[256];
  // Like nt_val_ but with T_BASE_VAL in place of -1 for 'n', 'x', '-', etc.
  int nt_val_5_[256];
  // Like nt_val_, but with T_BASE_VAL in place of -1 for nonexistent ones.
  int nt_val_no_n_[256];
  // Inverse array - takes X_BASE_VAL int to a DNA char value.
  DNA val_to_nt_[(N_BASE_VAL | MASKED_BASE_BIT) + 1];
  // Arrays to convert between lower case indicating repeat masking, and a
  // 1/2 byte representation where the 4th bit indicates if the character
  // is masked. Uses N_BASE_VAL for `n', `x', etc.
  int nt_val_masked_[256];
  DNA val_to_nt_masked_[256];

 private:
  void InitNucleotideChars();
  void InitNucleotideMixedCaseChars();
  void InitNucleotideCompareTable();

 private:
  // A little array to help decide if a character is a nucleotide, and if so,
  // convert it to lower case. Contains zeroes for characters that are not used
  // in DNA sequence.
  char nt_chars_[256];
  // A similar array that doesn't convert to lower case.
  char nt_mixed_case_chars_[256];
  // Another array to help do complement of DNA.
  DNA nt_comp_table_[256];

 private:
  void InitAminoAcidValues();

 private:
  struct AminoAcidRow {
    int ix;
    char letter;
    char abbreviation[4];
    const char *name;
  };

  static Sequencer::AminoAcidRow amino_acid_table_[];

  // Tables to convert from 0-20 to ASCII single letter representation of
  // amino acids.
  int aa_val_[256];
  AA val_to_aa_[20];
  AA aa_chars_[256]; // 0 except for value aa characters. Converts to upper first.
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_SEQ_H__ */
