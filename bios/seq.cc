/// Sequence module.
///
/// Assumes that DNA is stored as a character.
/// The DNA it generates will include the bases
/// as lowercase tcag.  It will generally accept
/// uppercase as well, and also 'n' or 'N' or '-'
/// for unknown bases.
///
/// Amino acids are stored as single character upper case.
///
/// This file is copyright 2002 Jim Kent, but license is hereby
/// granted for all use - public, private or commercial.

/// @file seq.c Module to handle DNA and protein sequences
/// @author Adapted by Lukas Habegger (lukas.habegger@yale.edu)

#include "seq.hh"

namespace bios {

Seq::Seq() {
  sequence = NULL;
  mask = NULL;
}

Seq::~Seq() {
  if (sequence != NULL) {
    free(sequence);
  }
  if (mask != NULL) {
    free(mask);
  }
}

/**
 * Allocate a mask for sequence and fill it in based on sequence case.
 */
BitField* Seq::MaskFromUpperCase(Seq* seq) {
  int size = seq->size;
  char *poly = seq->sequence;
  BitField *b = new BitField(size);
  for (int i = 0; i < size; ++i) {
    if (isupper(poly[i])) {
      b->SetBit(i);
    }
  }
  return b;
}

Sequencer::CodonRow Sequencer::codon_table_[] = {
  {"ttt", 'F', 'F',},
  {"ttc", 'F', 'F',},
  {"tta", 'L', 'L',},
  {"ttg", 'L', 'L',},

  {"tct", 'S', 'S',},
  {"tcc", 'S', 'S',},
  {"tca", 'S', 'S',},
  {"tcg", 'S', 'S',},

  {"tat", 'Y', 'Y',},
  {"tac", 'Y', 'Y',},
  {"taa", 0, 0,},
  {"tag", 0, 0,},

  {"tgt", 'C', 'C',},
  {"tgc", 'C', 'C',},
  {"tga", 0, 'W',},
  {"tgg", 'W', 'W',},

  {"ctt", 'L', 'L',},
  {"ctc", 'L', 'L',},
  {"cta", 'L', 'L',},
  {"ctg", 'L', 'L',},

  {"cct", 'P', 'P',},
  {"ccc", 'P', 'P',},
  {"cca", 'P', 'P',},
  {"ccg", 'P', 'P',},

  {"cat", 'H', 'H',},
  {"cac", 'H', 'H',},
  {"caa", 'Q', 'Q',},
  {"cag", 'Q', 'Q',},

  {"cgt", 'R', 'R',},
  {"cgc", 'R', 'R',},
  {"cga", 'R', 'R',},
  {"cgg", 'R', 'R',},

  {"att", 'I', 'I',},
  {"atc", 'I', 'I',},
  {"ata", 'I', 'M',},
  {"atg", 'M', 'M',},

  {"act", 'T', 'T',},
  {"acc", 'T', 'T',},
  {"aca", 'T', 'T',},
  {"acg", 'T', 'T',},

  {"aat", 'N', 'N',},
  {"aac", 'N', 'N',},
  {"aaa", 'K', 'K',},
  {"aag", 'K', 'K',},

  {"agt", 'S', 'S',},
  {"agc", 'S', 'S',},
  {"aga", 'R', 0,},
  {"agg", 'R', 0,},

  {"gtt", 'V', 'V',},
  {"gtc", 'V', 'V',},
  {"gta", 'V', 'V',},
  {"gtg", 'V', 'V',},

  {"gct", 'A', 'A',},
  {"gcc", 'A', 'A',},
  {"gca", 'A', 'A',},
  {"gcg", 'A', 'A',},

  {"gat", 'D', 'D',},
  {"gac", 'D', 'D',},
  {"gaa", 'E', 'E',},
  {"gag", 'E', 'E',},

  {"ggt", 'G', 'G',},
  {"ggc", 'G', 'G',},
  {"gga", 'G', 'G',},
  {"ggg", 'G', 'G',},
};

void Sequencer::InitNucleotideValues() {
  for (uint32_t i = 0; i < NUMELE(nt_val_); ++i) {
    nt_val_upper_[i] = nt_val_lower_[i] = nt_val_[i] = -1;
    nt_val_no_n_[i] = T_BASE_VAL;
    if (isspace(i) || isdigit(i)) {
      nt_val_5_[i] = nt_val_masked_[i] = -1;
    } else {
      nt_val_5_[i] = N_BASE_VAL;
      nt_val_masked_[i] = (islower(i)
          ? (N_BASE_VAL|MASKED_BASE_BIT)
          : N_BASE_VAL);
    }
  }

  const unsigned char bases[] = "tucagn";
  uint32_t length = NUMELE(bases);
  for (uint32_t i = 0; i < length; ++i) {
    unsigned char lower = bases[i];
    unsigned char upper = toupper(lower);
    int base_val = 0;
    switch (lower) {
      case 't':
        base_val = T_BASE_VAL;
        break;
      case 'u':
        base_val = U_BASE_VAL;
        break;
      case 'c':
        base_val = C_BASE_VAL;
        break;
      case 'a':
        base_val = A_BASE_VAL;
        break;
      case 'g':
        base_val = G_BASE_VAL;
        break;
      default:
        base_val = N_BASE_VAL;
        break;
    }

    if (lower != 'n') {
      nt_val_5_[lower] = nt_val_5_[upper] = nt_val_no_n_[lower] =
          nt_val_no_n_[upper] = nt_val_[lower] = nt_val_[upper] =
          nt_val_lower_[lower] = nt_val_upper_[upper] = base_val;

      nt_val_masked_[upper] = base_val;
      nt_val_masked_[lower] = base_val | MASKED_BASE_BIT;
    }

    if (lower != 'u') {
      val_to_nt_[base_val] = val_to_nt_[base_val|MASKED_BASE_BIT] = lower;
      val_to_nt_masked_[base_val] = upper;
      val_to_nt_masked_[base_val | MASKED_BASE_BIT] = lower;
    }
  }
}

/**
 * Returns one letter code for protein, 0 for stop codon, or X for bad input.
 */
AA Sequencer::LookupCodon(DNA* dna) {
  int ix = 0;
  for (int i = 0; i < 3; ++i) {
    int bv = nt_val_[(int) dna[i]];
    if (bv < 0) {
      return 'X';
    }
    ix = (ix << 2) + bv;
  }
  char c = codon_table_[ix].protein_code;
  c = toupper(c);
  return c;
}

/**
 * Returns one letter code for protein, 0 for stop codon, or X for bad input.
 */
AA Sequencer::LookupMitochondrialCodon(DNA* dna) {
  int ix = 0;
  for (int i = 0; i < 3; ++i) {
    int bv = nt_val_[(int) dna[i]];
    if (bv < 0) {
      return 'X';
    }
    ix = (ix << 2) + bv;
  }
  char c = codon_table_[ix].mito_code;
  c = toupper(c);
  return c;
}

/**
 * Return value from 0-63 of codon starting at start. Returns -1 if not a codon.
 */
Codon Sequencer::CodonVal(DNA* start) {
  int v1, v2, v3;
  if ((v1 = nt_val_[(int) start[0]]) < 0) {
    return -1;
  }
  if ((v2 = nt_val_[(int) start[1]]) < 0) {
    return -1;
  }
  if ((v3 = nt_val_[(int) start[2]]) < 0) {
    return -1;
  }
  return ((v1 << 4) + (v2 << 2) + v3);
}

/**
 * Return codon corresponding to val (0-63)
 */
const DNA* Sequencer::ValToCodon(int val) {
  assert(val >= 0 && val < 64);
  return codon_table_[val].codon;
}

char* Sequencer::DnaTranslate(DNA* dna, bool terminate_at_stop_codon) {
  std::stringstream translation;
  int dna_size = strlen(dna);
  for (int i = 0; i < dna_size - 2; i += 3) {
    char aa = LookupCodon(dna + i);
    if (aa == 0) {
      if (terminate_at_stop_codon) {
        break;
      }
      aa = '*';
    }
    translation << aa;
  }
  return strdup((char*) translation.str().c_str());
}

void Sequencer::InitNucleotideChars() {
  memset(nt_chars_, 0, sizeof(nt_chars_));
  const unsigned char bases[] = "acgtnu";
  uint32_t length = NUMELE(bases);
  for (uint32_t i = 0; i < length; ++i) {
    unsigned char lower = bases[i];
    unsigned char upper = toupper(lower);
    nt_chars_[lower] = nt_chars_[upper] = lower;
  }
  nt_chars_[(unsigned char)'-'] = (unsigned char)'n';
}

void Sequencer::InitNucleotideMixedCaseChars() {
  memset(nt_mixed_case_chars_, 0, sizeof(nt_mixed_case_chars_));
  const unsigned char bases[] = "acgtnu";
  uint32_t length = NUMELE(bases);
  for (uint32_t i = 0; i < length; ++i) {
    unsigned char lower = bases[i];
    unsigned char upper = toupper(lower);
    nt_mixed_case_chars_[lower] = lower;
    nt_mixed_case_chars_[upper] = upper;
  }
  nt_mixed_case_chars_[(unsigned char)'-'] = (unsigned char)'n';
}

void Sequencer::InitNucleotideCompareTable() {
  memset(nt_comp_table_, 0, sizeof(nt_comp_table_));
  const unsigned char orig[] = " -=acgtun-.ACGTUNRYMKSWVHDBXryswmkvhdbx()";
  const unsigned char tran[] = " -=tgcaan-.TGCAANYRKMSWBDHVNyrswkmbdhvn)(";
  uint32_t length = NUMELE(orig);
  for (uint32_t i = 0; i < length; ++i) {
    nt_comp_table_[orig[i]] = tran[i];
  }
}

/**
 * Complement DNA (not reverse).
 */
void Sequencer::Complement(DNA* dna, long length) {
  for (int i = 0; i < length; ++i) {
    *dna = nt_comp_table_[(int) *dna];
    ++dna;
  }
}

/**
 * Reverse complement DNA.
 */
void Sequencer::ReverseComplement(DNA* dna, long length) {
  reverse_bytes(dna, length);
  Complement(dna, length);
}

/**
 * Return 1 if sequence is all lower case, 0 otherwise.
 */
bool Sequencer::SeqIsLower(Seq* seq) {
  int size = seq->size;
  char *poly = seq->sequence;
  for (int i = 0; i < size; ++i) {
    if (!islower(poly[i])) {
      return false;
    }
  }
  return true;
}

/**
 * Return a translated sequence.  Offset is position of first base to translate.
 * If size is 0 then use length of in_seq.
 */
aaSeq* Sequencer::TranslateSeqN(dnaSeq* in_seq, unsigned offset,
                                unsigned in_size, int stop) {
  assert(offset <= in_seq->size);
  if ((in_size == 0) || (in_size > (in_seq->size - offset))) {
    in_size = in_seq->size - offset;
  }
  int last_codon = offset + in_size - 3;

  aaSeq* seq = new aaSeq;
  DNA* dna = in_seq->sequence;
  uint32_t actual_size = 0;
  size_t size = in_size / 3 + 1;
  seq->sequence = (char*) malloc(size);
  memset(seq->sequence, 0, size);
  AA* pep = seq->sequence;
  for (int i = offset; i <= last_codon; i += 3) {
    AA aa = LookupCodon(dna + i);
    if (aa == 0) {
      if (stop) {
        break;
      } else {
        aa = 'Z';
      }
    }
    *pep++ = aa;
    ++actual_size;
  }
  *pep = 0;
  assert(actual_size <= in_size / 3 + 1);
  seq->size = actual_size;
  seq->name = std::string(in_seq->name);
  return seq;
}

/**
 * Return a translated sequence.  Offset is position of first base to
 * translate. If stop is 1 then stop at first stop codon.
 * (Otherwise represent stop codons as 'Z').
 */
aaSeq* Sequencer::TranslateSeq(dnaSeq* in_seq, unsigned offset, int stop) {
  return TranslateSeqN(in_seq, offset, 0, stop);
}

/**
 * Convert T's to U's.
 */
void Sequencer::ToRna(DNA* dna) {
  for (;;) {
    DNA c = *dna;
    if (c == 't') {
      *dna = 'u';
    } else if (c == 'T') {
      *dna = 'U';
    } else if (c == 0) {
      break;
    }
    ++dna;
  }
}

/* Run chars through filter. */
void Sequencer::DnaOrAaFilter(char* in, char* out, char filter[256]) {
  char c;
  while ((c = *in++) != 0) {
    if ((c = filter[(int)c]) != 0) {
      *out++ = c;
    }
  }
  *out++ = 0;
}

/**
 * Filter out non-DNA characters and change to lower case.
 */
void Sequencer::DnaFilter(char* in, DNA* out) {
  DnaOrAaFilter(in, out, nt_chars_);
}

/**
 * Filter out non-DNA characters but leave case intact.
 */
void Sequencer::DnaMixedCaseFilter(char* in, DNA* out) {
  DnaOrAaFilter(in, out, nt_mixed_case_chars_);
}

/**
 * Filter out non-aa characters and change to upper case.
 */
void Sequencer::AaFilter(char* in, DNA* out) {
  DnaOrAaFilter(in, out, aa_chars_);
}

/**
 * Count up frequency of occurance of each base and store results in histogram.
 */
void Sequencer::DnaBaseHistogram(DNA* dna, int dna_size, int histogram[4]) {
  int val;
  memset(histogram, 0, 4 * sizeof(int));
  while (--dna_size >= 0) {
    if ((val = nt_val_[(int) *dna++]) >= 0) {
      ++histogram[val];
    }
  }
}

/**
 * Given a gap in genome from intron_start to intron_end, return 1 for GT/AG intron between left and right,
 * -1 for CT/AC, 0 for no intron. Assumes DNA is lower cased.
 */
int Sequencer::IntronOrientation(DNA* intron_start, DNA* intron_end) {
  if (intron_end - intron_start < 32) {
    return 0;
  }
  if (intron_start[0] == 'g' && intron_start[1] == 't' &&
      intron_end[-2] == 'a' && intron_end[-1] == 'g') {
    return 1;
  } else if (intron_start[0] == 'c' && intron_start[1] == 't' &&
      intron_end[-2] == 'a' && intron_end[-1] == 'c') {
    return -1;
  } else {
    return 0;
  }
}

/**
 * Compare two sequences (without inserts or deletions) and score.
 */
int Sequencer::DnaOrAaScoreMatch(char* a, char* b, int size, int match_score,
                                 int mismatch_score, char ignore) {
  int score = 0;
  for (int i = 0; i < size; ++i) {
    char aa = a[i];
    char bb = b[i];
    if (aa == ignore || bb == ignore) {
      continue;
    }
    if (aa == bb) {
      score += match_score;
    } else {
      score += mismatch_score;
    }
  }
  return score;
}

Sequencer::AminoAcidRow Sequencer::amino_acid_table_[] = {
  {0, 'A', "ala", "alanine"},
  {1, 'C', "cys", "cysteine"},
  {2, 'D', "asp",  "aspartic acid"},
  {3, 'E', "glu",  "glutamic acid"},
  {4, 'F', "phe",  "phenylalanine"},
  {5, 'G', "gly",  "glycine"},
  {6, 'H', "his",  "histidine"},
  {7, 'I', "ile",  "isoleucine"},
  {8, 'K', "lys",  "lysine"},
  {9, 'L', "leu",  "leucine"},
  {10, 'M', "met", "methionine"},
  {11, 'N', "asn", "asparagine"},
  {12, 'P', "pro", "proline"},
  {13, 'Q', "gln", "glutamine"},
  {14, 'R', "arg", "arginine"},
  {15, 'S', "ser", "serine"},
  {16, 'T', "thr", "threonine"},
  {17, 'V', "val", "valine"},
  {18, 'W', "try", "tryptophan"},
  {19, 'Y', "tyr", "tyrosine"},
};

/* Initialize aaVal and valToAa tables. */
void Sequencer::InitAminoAcidValues() {
  for (uint32_t i = 0; i < NUMELE(aa_val_); ++i) {
    aa_val_[i] = -1;
  }
  for (uint32_t i = 0; i < NUMELE(amino_acid_table_); ++i) {
    char c = amino_acid_table_[i].letter;
    char lowc = tolower(c);
    aa_val_[(int) c] = aa_val_[(int) lowc] = i;
    aa_chars_[(int) c] = aa_chars_[(int) lowc] = c;
    val_to_aa_[i] = c;
  }
  aa_chars_[(unsigned char)'x'] = aa_chars_[(unsigned char)'X'] =
      (unsigned char)'X';
}

Sequencer::Sequencer() {
  InitNucleotideValues();
  InitAminoAcidValues();
  InitNucleotideChars();
  InitNucleotideMixedCaseChars();
  InitNucleotideCompareTable();
}

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
