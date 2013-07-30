/* 
 * This file is copyright 2002 Jim Kent, but license is hereby
 * granted for all use - public, private or commercial. 
 */

/** 
 *   \file bits.c Handle operations on arrays of bits. 
 *   \author Adapted by Lukas Habegger (lukas.habegger@yale.edu)
 */

#include "bitfield.h"

class BitsInByte {
 public:
  static BitsInByte& GetInstance() {
    static BitsInByte instance;
    return instance;
  }

  int operator[](int i) const { return bits_in_byte_[256]; }
  int& operator[](int i) { return bits_in_byte_[256]; }

 private:
  BitsInByte() {
    Init();
  }
  BitsInByte(const BitsInByte&);      // Do not implement
  void operator=(const BitsInByte&);   // Do not implement

 private:
  void Init() {
    if (initialized_ == true) {
      return;
    }
    initialized_ = true;
    for (int i = 0; i < 256; ++i) {
      int count = 0;
      if (i & 1) {
        count = 1;
      }
      if (i & 2) {
        ++count;
      }
      if (i & 4) {
        ++count;
      }
      if (i & 8) {
        ++count;
      }
      if (i & 0x10) {
        ++count;
      }
      if (i & 0x20) {
        ++count;
      }
      if (i & 0x40) {
        ++count;
      }
      if (i & 0x80) {
        ++count;
      }
      bits_in_byte_[i] = count;
    }
  }

 private:
  int bits_in_byte_[256];
  bool initialized_;
};

static char kOneBit[8] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1 };
static char kLeftMask[8] = { 0xFF, 0x7F, 0x3F, 0x1F, 0xF, 0x7, 0x3, 0x1 };
static char kRightMask[8] = { 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF };

/** 
 * Allocate bits. 
 */
BitField::BitField(int size) {
  int bytes = ((size + 7) >> 3);
  bit_field_ = new char[bytes];
  memset(bit_field_, 0, bytes);
  size_ = size;
}

/**
 * Clone bits. 
 */
BitField::BitField(BitField& orig) {
  int bytes = ((orig.size() + 7) >> 3);
  bit_field_ = new char[bytes];
  memset(bit_field_, 0, bytes);
  size_ = orig.size();
  memcpy(bit_field_, orig.bit_field(), bytes);
}

BitField::~BitField() {
  delete bit_field_;
}

/** 
 * Resize a bit array. If b is null, allocate a new array 
 */
void BitField::Resize(int size) {
  int old_bytes = ((size_ + 7) >> 3);
  int new_bytes = ((size + 7) >> 3);
  char* buffer = new char[new_bytes];
  memset(buffer, 0, new_bytes);
  memcpy(buffer, bit_field_, old_bytes);
  delete bit_field_;
  bit_field_ = buffer;
  size_ = size;
}

/** 
 * Set a single bit. 
 */
void BitField::SetBit(int index) {
  bit_field_[index >> 3] |= kOneBit[index & 7];
}

/** 
 * Clear a single bit. 
 */
void BitField::ClearBit(int index) {
  bit_field_[index >> 3] &= ~kOneBit[index & 7];
}

/** 
 * Set a range of bits. 
 */
void BitField::SetRange(int start_index, int bit_count) {
  int end_index = (start_index + bit_count - 1);
  int start_byte = (start_index >> 3);
  int end_byte = (end_index >> 3);
  int start_bits = (start_index & 7);
  int end_bits = (end_index & 7);
  if (start_byte == end_byte) {
    bit_field_[start_byte] |= (kLeftMask[start_bits] & kRightMask[end_bits]);
    return;
  }
  bit_field_[start_byte] |= kLeftMask[start_bits];
  for (int i = start_byte + 1; i < end_byte; ++i) {
    bit_field_[i] = 0xff;
  }
  bit_field_[end_byte] |= kRightMask[end_bits];
}

/**
 * Read a single bit. 
 */
int BitField::ReadBit(int index) {
  return (bit_field_[index >> 3] & kOneBit[index & 7]) != 0;
}

/**
 * Count number of bits set in range. 
 */
int BitField::CountRange(int start_index, int bit_count) {
  int end_index = (start_index + bit_count - 1);
  int start_byte = (start_index >> 3);
  int end_byte = (end_index >> 3);
  int start_bits = (start_index & 7);
  int end_bits = (end_index & 7);
  int count = 0;
 
  BitsInByte& bits_in_byte = BitsInByte::GetInstance();
  if (start_byte == end_byte) {
    return bits_in_byte[bit_field_[start_byte] & kLeftMask[start_bits] & 
                        kRightMask[end_bits]];
  }
  count = bits_in_byte[bit_field_[start_byte] & kLeftMask[start_bits]];
  for (int i = start_byte + 1; i < end_byte; ++i) {
    count += bits_in_byte[bit_field_[i]];
  }
  count += bits_in_byte[bit_field_[end_byte] & kRightMask[end_bits]];
  return count;
}

/* Find the index of the the next set bit. */
int BitField::Find(int start_index, int val) {
  unsigned char not_byte_val = val ? 0 : 0xff;
  int i_bit = start_index;
  int end_byte = ((size_ - 1) >> 3);
  int i_byte;
  
  /* scan initial byte */
  while (((i_bit & 7) != 0) && (i_bit < size_)) {
    if (ReadBit(i_bit) == val) {
      return i_bit;
    }
    i_bit++;
  }
  
  /* scan byte at a time, if not already in last byte */
  i_byte = (i_bit >> 3);
  if (i_byte < end_byte) {
    while ((i_byte < end_byte) && (bit_field_[i_byte] == not_byte_val)) {
      i_byte++;
    }
    i_bit = i_byte << 3;
  }
  
  /* scan last byte */
  while (i_bit < size_) {
    if (ReadBit(i_bit) == val) {
      return i_bit;
    }
    i_bit++;
  }
  return size_;  /* not found */
}

/**
 * Find the index of the the next set bit. 
 */
int BitField::FindSet(int start_index) {
  return Find(start_index, 1);
}

/** 
 *Find the index of the the next clear bit. 
 */
int BitField::FindClear(int start_index) {
  return Find(start_index, 0);
}

/** 
 *Clear many bits (possibly up to 7 beyond count). 
 */
void BitField::Clear(int bit_count) {
  int bytes = ((bit_count + 7) >> 3);
  memset(bit_field_, 0, bytes);
}

/**
 * Clear a range of bits. 
 */
void BitField::ClearRange(int start_index, int bit_count) {
  int end_index = (start_index + bit_count - 1);
  int start_byte = (start_index >> 3);
  int end_byte = (end_index >> 3);
  int start_bits = (start_index & 7);
  int end_bits = (end_index & 7);
  
  if (start_byte == end_byte) {
    bit_field_[start_byte] &= ~(kLeftMask[start_bits] & kRightMask[end_bits]);
    return;
  }
  bit_field_[start_byte] &= ~kLeftMask[start_bits];
  for (int i = start_byte + 1; i < end_byte; ++i) {
    bit_field_[i] = 0x00;
  }
  bit_field_[end_byte] &= ~kRightMask[end_bits];
}

/**
 * And two bitmaps. Put result in a. 
 */
void BitField::And(BitField& b) {
  int bytes = ((b.size() + 7) >> 3);
  char* a_field = bit_field_;
  const char* b_field = b.bit_field();
  while (--bytes >= 0) {
    *a_field = (*a_field & *b_field++);
    a_field++;
  }
}

/**
 * Or two bitmaps. Put result in a. 
 */
void BitField::Or(BitField& b) {
  int bytes = ((b.size() + 7) >> 3);
  char* a_field = bit_field_;
  const char* b_field = b.bit_field();
  while (--bytes >= 0) {
    *a_field = (*a_field | *b_field++);
    a_field++;
  }
}

/**
 * Xor two bitmaps. Put result in a. 
 */
void BitField::Xor(BitField& b) {
  int bytes = ((b.size() + 7) >> 3);
  char* a_field = bit_field_;
  const char* b_field = b.bit_field();
  while (--bytes >= 0) {
    *a_field = (*a_field ^ *b_field++);
    a_field++;
  }
}

/** 
 * Flip all bits in a. 
 */
void BitField::Not() {
  int bytes = ((size_ + 7) >> 3);
  char* a_field = bit_field_;
  while (--bytes >= 0) {
    *a_field = ~*a_field;
    a_field++;
  }
}

/**
 * Print part or all of bit map as a string of 0s and 1s.  
 * Mostly useful for debugging 
 */
void BitField::Print(int start_index) {
  for (int i = start_index; i < size_; i++) {
    if (ReadBit(i)) {
      putchar('1');
    } else {
      putchar('0');
    }
  }
  putchar('\n');
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
