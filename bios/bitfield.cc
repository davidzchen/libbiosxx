// bits - Handle operations on arrays of bits. 
//
// This file is copyright 2002 Jim Kent, but license is hereby
// granted for all use - public, private or commercial. 

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

/// @file bedgraph.hh
/// @version 
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the module for handling operations on arrays of
/// bits.

#include "bitfield.hh"

namespace bios {

class BitsInByte {
 public:
  static BitsInByte& GetInstance() {
    static BitsInByte instance;
    return instance;
  }

  int operator[](int i) const { return bits_in_byte_[i]; }
  int& operator[](int i) { return bits_in_byte_[i]; }

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

static uint8_t kOneBit[8] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1 };
static uint8_t kLeftMask[8] = { 0xFF, 0x7F, 0x3F, 0x1F, 0xF, 0x7, 0x3, 0x1 };
static uint8_t kRightMask[8] = { 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF };

BitField::BitField(int size) {
  int bytes = ((size + 7) >> 3);
  bit_field_ = new uint8_t[bytes];
  memset(bit_field_, 0, bytes);
  size_ = size;
}

BitField::BitField(BitField& orig) {
  int bytes = ((orig.size() + 7) >> 3);
  bit_field_ = new uint8_t[bytes];
  memset(bit_field_, 0, bytes);
  size_ = orig.size();
  memcpy(bit_field_, orig.bit_field(), bytes);
}

BitField::~BitField() {
  delete bit_field_;
}

void BitField::Resize(int size) {
  int old_bytes = ((size_ + 7) >> 3);
  int new_bytes = ((size + 7) >> 3);
  uint8_t* buffer = new uint8_t[new_bytes];
  memset(buffer, 0, new_bytes);
  memcpy(buffer, bit_field_, old_bytes);
  delete bit_field_;
  bit_field_ = buffer;
  size_ = size;
}

void BitField::SetBit(int index) {
  bit_field_[index >> 3] |= kOneBit[index & 7];
}

void BitField::ClearBit(int index) {
  bit_field_[index >> 3] &= ~kOneBit[index & 7];
}

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

int BitField::ReadBit(int index) {
  return (bit_field_[index >> 3] & kOneBit[index & 7]) != 0;
}

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

int BitField::Find(int start_index, int val) {
  uint8_t not_byte_val = val ? 0 : 0xff;
  int i_bit = start_index;
  int end_byte = ((size_ - 1) >> 3);
  int i_byte;
  
  // scan initial byte
  while (((i_bit & 7) != 0) && (i_bit < size_)) {
    if (ReadBit(i_bit) == val) {
      return i_bit;
    }
    i_bit++;
  }
  
  // scan byte at a time, if not already in last byte.
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

int BitField::FindSet(int start_index) {
  return Find(start_index, 1);
}

int BitField::FindClear(int start_index) {
  return Find(start_index, 0);
}

void BitField::Clear(int bit_count) {
  int bytes = ((bit_count + 7) >> 3);
  memset(bit_field_, 0, bytes);
}

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

void BitField::And(BitField& b) {
  int bytes = ((b.size() + 7) >> 3);
  uint8_t* a_field = bit_field_;
  const uint8_t* b_field = b.bit_field();
  while (--bytes >= 0) {
    *a_field = (*a_field & *b_field++);
    a_field++;
  }
}

void BitField::Or(BitField& b) {
  int bytes = ((b.size() + 7) >> 3);
  uint8_t* a_field = bit_field_;
  const uint8_t* b_field = b.bit_field();
  while (--bytes >= 0) {
    *a_field = (*a_field | *b_field++);
    a_field++;
  }
}

void BitField::Xor(BitField& b) {
  int bytes = ((b.size() + 7) >> 3);
  uint8_t* a_field = bit_field_;
  const uint8_t* b_field = b.bit_field();
  while (--bytes >= 0) {
    *a_field = (*a_field ^ *b_field++);
    a_field++;
  }
}

void BitField::Not() {
  int bytes = ((size_ + 7) >> 3);
  uint8_t* a_field = bit_field_;
  while (--bytes >= 0) {
    *a_field = ~*a_field;
    a_field++;
  }
}

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

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
