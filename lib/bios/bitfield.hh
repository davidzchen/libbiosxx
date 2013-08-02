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

#ifndef BIOINFO_BITS_H__
#define BIOINFO_BITS_H__

#include <cstring>
#include <cstdio>

namespace bios {

/// @class BitField
/// @brief Class representing a bitfield.
class BitField {
 public:
  BitField(int size);
  BitField(BitField& source);
  ~BitField();

  int size() const { return size_; }
  const char* bit_field() { return bit_field_; }

  /// @brief Resize the bit field.
  ///
  /// This method resizes the bitfield to the given number of bits.
  ///
  /// @param     size    The number of bits to resize the bitfield to.
  void Resize(int size);

  /// @brief Set a bit in the bit field.
  ///
  /// This method sets a bit in the bit field at the given bit index.
  ///
  /// @param     bit_index  The bit index at which to set the bit.
  void SetBit(int bit_index);
  
  /// @brief Clear a bit in the bit field.
  ///
  /// This method clears a bit in the bit field at the given bit index.
  ///
  /// @param     bit_index  The bit index at which to clear the bit.
  void ClearBit(int bit_index);

  /// @brief Set all bits in a range in the bit field.
  ///
  /// This method sets all bits within a range specified by the starting bit
  /// index and the count.
  ///
  /// @param    start_index  The starting index for the range to set.
  /// @param    bit_count    The number of bits to set.
  void SetRange(int start_index, int bit_count);

  /// @brief Reads a bit.
  ///
  /// This method reads a bit at the specified bit index.
  ///
  /// @param    bit_index    The index of the bit to read.
  ///
  /// @return   The value of the bit at the specified index.
  int ReadBit(int bit_index);

  /// @brief Count the number of bits set in a range.
  ///
  /// This method counts the number of bits set within a range specified by
  /// the starting bit index and the count.
  ///
  /// @param    start_index  The starting index of the range to count.
  /// @param    bit_count    The number of bits to count.
  ///
  /// @return   The number of bits set in the range.
  int CountRange(int start_index, int bit_count);

  /// @brief Find the index of the next set bit.
  ///
  /// This method finds the index of the next set bit starting at the specified
  /// bit index.
  ///
  /// @param    index        The starting bit index.
  ///
  /// @return The index of the next set bit.
  int FindSet(int index);

  /// @brief Find the index of the next cleared bit.
  ///
  /// This method finds the index of the next cleared bit starting at the
  /// specifed bit index.
  ///
  /// @param    index        The starting bit index.
  ///
  /// @return The index of the next cleared bit.
  int FindClear(int index);

  /// @brief Clear bits starting at the beginning of the bitfield.
  ///
  /// This method clears a given number of bits starting at the beginning of
  /// the bitfield.
  ///
  /// @param    bit_count    The number of bits to clear.
  void Clear(int bit_count);

  /// @brief Clear a range of bits.
  ///
  /// This method clears a range of bits specified by the starting index and
  /// count.
  ///
  /// @param    start_index  The starting index of the range to clear.
  /// @param    bit_count    The number of bits to clear.
  void ClearRange(int start_index, int bit_count);

  /// @brief Perform a bitwise AND with another bitmap.
  /// 
  /// This method performs a bitwise AND with another bitmap. The result is
  /// stored in the current bitmap.
  ///
  /// @param    b            The bitmap to AND the current bitmap with.
  void And(BitField& b);
  
  /// @brief Perform a bitwise OR with another bitmap.
  /// 
  /// This method performs a bitwise OR with another bitmap. The result is
  /// stored in the current bitmap.
  ///
  /// @param    b            The bitmap to OR the current bitmap with.
  
  void Or(BitField& b);

  /// @brief Perform a bitwise XOR with another bitmap.
  /// 
  /// This method performs a bitwise XOR with another bitmap. The result is
  /// stored in the current bitmap.
  ///
  /// @param    b            The bitmap to XOR the current bitmap with.
  void Xor(BitField& b);
  
  /// @brief Flip all bits.
  /// 
  /// This method performs a bitwise NOT on the current bitmap, flipping all
  /// the bits.
  void Not();

  /// @brief Print the bits of the bitmap.
  ///
  /// This method prints bits of the bitmap starting at the given bit index
  /// as a string of '0's and '1's.
  ///
  /// @param    start        The starting bit index.
  void Print(int start);

 private:
  /// @brief Find the index of the first bit equal to the given value.
  ///
  /// This method find the index of the first bit that is set or cleared
  /// according to val starting at the given starting bit index.
  ///
  /// @param    start_index  The starting index.
  /// @param    val          The value of the bit to search for.
  ///
  /// @return   The index of the first bit equal to val.
  int Find(int start_index, int val);

 private:
  int size_;
  char* bit_field_;
};

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_BITS_H__ */
