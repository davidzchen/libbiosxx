/* 
 * bits - Handle operations on arrays of bits. 
 *
 * This file is copyright 2002 Jim Kent, but license is hereby
 * granted for all use - public, private or commercial. 
 */

/** 
 *   \file bits.h
 *   \author Adapted by Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOINFO_BITS_H__
#define BIOINFO_BITS_H__

#include "common.h"

class BitField {
 public:
  BitField(int size);
  BitField(BitField& source);
  ~BitField();

  int size() const { return size_; }
  const char* bit_field() { return bit_field_; }

  void Resize(int size);
  void SetBit(int bit_index);
  void ClearBit(int bit_index);
  void SetRange(int start_index, int bit_count);
  int ReadBit(int bit_index);
  int CountRange(int start_index, int bit_count);
  int FindSet(int index);
  int FindClear(int index);
  void Clear(int bit_count);
  void ClearRange(int index, int bit_count);
  void And(BitField& b);
  void Or(BitField& b);
  void Xor(BitField& b);
  void Not();
  void Print(int start);

 private:
  void Init();
  int Find(int start_index, int val);

 private:
  int size_;
  char* bit_field_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_BITS_H__ */
