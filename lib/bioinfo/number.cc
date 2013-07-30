/** 
 *   \file numUtil.c Numeric utilities
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#include "number.h"

namespace num {

/** 
 * Returns rounded a*p/q 
 */
int rounding_scale(int a, int p, int q) {
  if (a > 100000 || p > 100000) {
    double x = a;
    x *= p;
    x /= q;
    return round(x);
  } else {
    return (a * p + q / 2) / q;
  }
}

/** 
 * Return amount of bases two ranges intersect over, 0 or negative if no intersection. 
 */
int range_intersection(int start1, int end1, int start2, int end2) {
  int s = MAX(start1,start2);
  int e = MIN(end1,end2);
  return e - s;
}

/** 
 * Return number of bases in intersection of two ranges, or zero if they don't intersect. 
 */
int positive_range_intersection(int start1, int end1, int start2, int end2) {
  int ret = range_intersection(start1, end1, start2, end2);
  if (ret < 0) {
    ret = 0;
  }
  return ret;
}

/** 
 * Return byte-swapped version of a. 
 */
unsigned byteswap32(unsigned a) {
  union {
    unsigned whole; 
    unsigned char bytes[4];
  } u, v;
  u.whole = a;
  v.bytes[0] = u.bytes[3];
  v.bytes[1] = u.bytes[2];
  v.bytes[2] = u.bytes[1];
  v.bytes[3] = u.bytes[0];
  return v.whole;
}

/**
 * Return base two number of digits. 
 */
int digits_base_two(unsigned long x) {
  int digits = 0;
  while (x) {
    digits += 1;
    x >>= 1;
  }
  return digits;
}

/**
 * Return number of digits base 10. 
 */
int digits_base_ten(int x) {
  int digits = 1;
  if (x < 0) {
    digits = 2;
    x = -x;
  }
  while (x >= 10) {
    digits += 1;
    x /= 10;
  }
  return digits;
}

static double calculate_sum_term(std::vector<int>& ties) {
  double sum_term = 0.0;
  for (int i = 0; i < ties.size(); ++i) {
    sum_term += (pow(ties[i], 3) - ties[i]);
  }
  return sum_term;
}

static double calculate_c1(std::vector<int>& ties1, std::vector<int>& ties2, 
                           int n) {
  if (ties1.empty() && ties2.empty()) {
    return 0.0;
  }
  double sum_term1 = calculate_sum_term(ties1);
  double sum_term2 = calculate_sum_term(ties2);
  return (sum_term1 + sum_term2) / (2 * n * ((n * n) - 1));
}

static double calculate_c2(std::vector<int>& ties1, std::vector<int>& ties2, 
                           int n) {
  if (ties1.empty() && ties2.empty()) {
    return 1.0;
  }
  double sum_term1 = calculate_sum_term(ties1);
  double sum_term2 = calculate_sum_term(ties2);
  double term1 = 1.0 - (sum_term1 / (n * ((n * n) - 1)));
  double term2 = 1.0 - (sum_term2 / (n * ((n * n) - 1)));
  return sqrt(term1 * term2);
}

struct ValuePair {
  ValuePair() { }
  ~ValuePair() { }

  static bool CompareByValue1(const ValuePair& a, const ValuePair& b) {
    return a.value1 < b.value1;
  }
  static bool CompareByValue2(const ValuePair& a, const ValuePair& b) {
    return a.value2 < b.value2;
  }

  double value1;
  double value2;
  double rank1;
  double rank2;
};

double spearman_correlation(std::vector<double>& a, std::vector<double>& b) {
  if (a.size() != b.size()) {
    die((char*) "Expected the same number of elements for the two arrays");
  }
  int n = a.size();
  std::vector<ValuePair> value_pairs;
  for (int i = 0; i < n; ++i) {
    ValuePair vp;
    vp.value1 = a[i];
    vp.value2 = b[i];
    value_pairs.push_back(vp);
  }
  
  std::sort(value_pairs.begin(), value_pairs.end(), 
            ValuePair::CompareByValue1);
  std::vector<int> ties1;
  int i = 0; 
  while (i < value_pairs.size()) {
    ValuePair& vp = value_pairs[i];
    int sum_ranks = i + 1;
    int tie_count = 1;
    int j = i + 1;
    while (j < value_pairs.size()) {
      ValuePair& next_vp = value_pairs[j];
      if (vp.value1 == next_vp.value1) {
        ++tie_count;
      } else {
        break;
      }
      sum_ranks += j + 1;
      ++j;
    }
    if (tie_count > 1) {
      ties1.push_back(tie_count);
    }
    for (int k = i; k < tie_count + i; ++k) {
      value_pairs[k].rank1 = sum_ranks / tie_count;
    }
    i = j;
  }

  std::sort(value_pairs.begin(), value_pairs.end(),
            ValuePair::CompareByValue2);
  std::vector<int> ties2;
  i = 0; 
  while (i < value_pairs.size()) {
    ValuePair& vp = value_pairs[i];
    int sum_ranks = i + 1;
    int tie_count = 1;
    int j = i + 1;
    while (j < value_pairs.size()) {
      ValuePair& next_vp = value_pairs[j];
      if (vp.value2 == next_vp.value2) {
        ++tie_count;
      } else {
        break;
      }
      sum_ranks += j + 1;
      ++j;
    }
    if (tie_count > 1) {
      ties2.push_back(tie_count);
    }
    for (int k = i; k < i + tie_count; k++) {
      value_pairs[k].rank2 = sum_ranks / tie_count;
    }
    i = j;
  }

  double c1 = calculate_c1(ties1, ties2, n);
  double c2 = calculate_c2(ties1, ties2, n);
  int sum_squared_rank_differentials = 0;
  for (int i = 0; i < value_pairs.size(); i++) {
    ValuePair& vp = value_pairs[i];
    int rank_differential = vp.rank1 - vp.rank2;
    sum_squared_rank_differentials += (rank_differential * rank_differential);
  }
  return ((1.0 - ((6.0 * sum_squared_rank_differentials) / 
                  (n * ((n * n) - 1)))) - c1) / c2;
}

}; // namespace num

/* vim: set ai ts=2 sts=2 sw=2 et: */
