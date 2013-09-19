/** 
 *   \file numUtil.h
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOS_NUMBER_H__
#define BIOS_NUMBER_H__

#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdint.h>

namespace bios {

namespace num {

/** 
 * Return log base two of number x. 
 */
static inline double log_base2(double x) {
  return log(x) / log(2.0);
}

/** 
 *  Round floating point value to nearest integer.
 */
static inline int round(double a) {
  return (int) (a + 0.5);
}

/** 
 * Round floating point value to nearest long long. 
 */
static inline long long roundll(double a) {
  return (long long) (a + 0.5);
}

int round_scale(int a, int p, int q);
int range_intersection(int start1, int end1, int start2, int end2);
int positive_range_intersection(int start1, int end1, int start2, int end2);
unsigned byteswap32(unsigned a);
int digits_base2(unsigned long x);
int digits_base10(int x);
double spearman_correlation(std::vector<double>& a, std::vector<double>& b);

class GraphCoordTrans {
 public:
  GraphCoordTrans()
      : min_u_(0.0), max_u_(0.0), min_p_(1), max_p_(0) {
  }

  GraphCoordTrans(double min_u, double max_u, int min_p, int max_p)
      : min_u_(min_u), max_u_(max_u), min_p_(min_p), max_p_(max_p) {
  }

  ~GraphCoordTrans() {
  }

  // Returns a pixel coordinate for the user coordinate x.
  int ToPix(double x) {
    return min_p_ + (x - min_u_) * (max_p_ - min_p_) / (max_u_ - min_u_);
  }

  // Returns a user coordinate for the pixel coordinate.
  double ToUser(int x) {
    return min_u_ + (x - min_p_) * (max_u_ - min_u_) / (max_p_ - min_p_);
  }

  double min_u() const { return min_u_; }
  double max_u() const { return max_u_; }
  int min_p() const { return min_p_; }
  int max_p() const { return max_p_; }

  void set_min_u(double min_u) { min_u_ = min_u; }
  void set_max_u(double max_u) { max_u_ = max_u; }
  void set_min_p(int min_p) { min_p_ = min_p; }
  void set_max_p(int max_p) { max_p_ = max_p; }

 private:
  double min_u_;
  double max_u_;
  int min_p_;
  int max_p_;
};

}; // namespace num

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_NUMBER_H__ */
