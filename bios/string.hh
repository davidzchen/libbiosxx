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

/// @file string.hh
/// @author David Z. Chen <d.zhekai.chen@gmail.com>
/// @version 1.0.0
/// @since 07 Aug 2013
///
/// @section DESCRIPTION
///
/// This is the header for the BIOS string module. This module provides
/// additional functions for manipulating std::string strings.
///
/// Some functions in the original strUtil module are not included because
/// there already exist functions in the STL that implements the same logic.
/// Those functions are as follows:
///
/// subString  - implemented by std::string::substr
/// stringIn   - implemented by std::string::find
/// rStringIn  - implemented by std::string::rfind
/// lastChar   - can be implemented easily using str[str.size() - 1];
/// subChar    - can be implemented using std::replace in <algorithm>
/// stripChar  - can be implemented using std::remove in <algorithm>
/// addSuffix  - can be implemented using std::operator+=
/// strReplace - implemented by std::string::replace
///
/// Other functions are not included because they should are not suitable as
/// library functions and should rather be implemented by application logic.
/// Those functions are as follows:
///
/// naForNull
/// naForEmpty
/// trueFalseString
/// emptyForNull
/// nullIfAllSpace - replaced with is_whitespace

#ifndef BIOS_STRING_H__ 
#define BIOS_STRING_H__

#include <cctype>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>

#include "misc.hh"

namespace bios {

namespace string {

const char* kWhiteSpaces = " \t\n\r\f\v";

/// @brief Returns whether the haystack begins with the substring needle.
///
/// @param    haystack   The string to search in.
/// @param    needle     The substring to search for.
///
/// @return true if haystack begins with needle, false otherwise.
static inline bool starts_with(std::string& haystack, std::string& needle) {
  return haystack.find(needle) == 0;
}

/// @brief Returns whether the haystack begins with the substring needle.
///
/// @param    haystack   The string to search in.
/// @param    needle     The substring to search for.
///
/// @return true if haystack begins with needle, false otherwise.
static inline bool starts_with(std::string& haystack, const char* needle) {
  return haystack.find(needle) == 0;
}

/// @brief Returns the string between the start and end strings.
///
/// This function creates a new std::string object that is a copy of the
/// substring contained between the given start and end strings.
///
/// @param    between    The substring between the start and end string 
///                      returned to the caller.
/// @param    haystack   The string to search in.
/// @param    start      The start string.
/// @param    end        The end string.
/// 
/// @return   true if the string between the start and end was successfully
///           found and returned, or false otherwise.
bool string_between(std::string& bewteen, std::string& haystack, 
                    std::string& start, std::string& end);

/// @brief Toggles the upper and lower case characters in a string.
///
/// This function modifies the input string.
///
/// @param    str        The string to modify.
void toggle_case(std::string& str);

/// @brief Changes all characters of the string to upper case.
///
/// This function modifies the input string.
///
/// @param    str        The input string converted to all uppercase.
void to_upper(std::string& str);

/// @brief Changes all characters of the string to lower case.
///
/// This function modifies the input string.
///
/// @param    str        The input string converted to all lowercase.
void to_lower(std::string& s);

/// @brief returns the number of characters c in the string s.
///
/// @param    str        The input string.
/// @param    c          The character to count.
///
/// @return   The count of the number of times c appears in str.
size_t count_char(std::string& str, char c);

/// @brief returns the number of characters at the beginning of the strings
///        a and b that are the same.
///
/// XXX More discussion is needed to determine the use cases for this function.
/// This rewrite is according to the algorithm implemented by the original
/// version of this function, which appears to count the number of characters
/// at the _beginning_ of the strings a and b that are the same, but does not
/// count
///
/// @param    a          The first string.
/// @param    b          The second string.
///
/// @return   The number of characters at the beginning of a and b that are
///           the same.
size_t count_same(std::string& a, std::string& b);

/// @brief returns the index of the first non-whitespace character in str.
///
/// @param    str        The string.
///
/// @return   The index of the first non-whitespace character in str.
static inline size_t skip_leading_spaces(std::string& str) {
  return str.find_first_not_of(kWhiteSpaces);
}

/// @brief Returns the index of the first whitespace-character in str.
///
/// @param    str        The string.
///
/// @return   The index of the first whitespace character in str.
static inline size_t skip_to_spaces(std::string& str) {
  return str.find_first_of(kWhiteSpaces);
}

/// @brief Trims all trailing whitespace in the input string.
///
/// This function modifies the input string.
///
/// @param    str        The input string.
void rtrim(std::string& str);

/// @brief Trims all leading whitespace in the input string.
///
/// This function modifies the input string.
///
/// @param    str        The input string.
void ltrim(std::string& str);

/// @brief Trims all leading and trailing whitespace in the input string.
///
/// This function modifies the input string.
///
/// @param    str        The input string.
void trim(std::string& str);

/// @brief Removes all white space from the input string.
///
/// @param    str        The input string.
void erase_whitespace(std::string& str);

/// @brief Returns whether the input string contains any whitespace characters.
///
/// @param    str        The input string
///
/// @return   true if the string has whitespace characters, false otherwise.
static inline bool has_whitespace(std::string& str) {
  return str.find_first_of(kWhiteSpaces) != std::string::npos;
}

/// @brief Returns whether the input string contains all whitespace characters.
///
/// @param    str        The input string.
///
/// @return   true if the string only contains whitespace characters, false
///           otherwise.
static inline bool is_whitespace(std::string& str) {
  return str.find_first_not_of(kWhiteSpaces) == std::string::npos;
}

/// @brief Returns a copy of the first word in the line.
///
/// @param    str        The input string.
/// @param    pos        Position of the first character in the string to be
///                      considered in the search.
///
/// @return   Copy of the first word in the line.
std::string first_word_in_line(std::string& line, size_t pos);

/// @brief Returns a copy of the first word in the line.
///
/// @param    str        The input string.
///
/// @return   Copy of the first word in the line.
std::string first_word_in_line(std::string& line);

/// @brief Returns a copy of the last word in the line.
///
/// @param    str        The input string.
/// @param    pos        Position of the last character in the string to be
///                      considered in the search.
///
/// @return   Copy of the last word in the line.
std::string last_word_in_line(std::string& line, size_t pos);

/// @brief Returns a copy of the last word in the line.
///
/// @param    str        The input string.
///
/// @return   Copy of the last word in the line.
std::string last_word_in_line(std::string& line);

/// @brief Removes the end of the input string starting at the first occurrence
///        of char c.
///
/// This function modifies the input string.
///
/// @param    str        The input string.
/// @param    c          The character to search for.
void chop_suffix_at(std::string& str, char c);

/// @brief Removes suffix, starting at the last '.' character and beyond, from
///        the input string, if any.
///
/// This function modifies the input string. This function is equivalent to
/// calling bios::string::chop_suffix_at(str, '.').
///
/// @param    str        The input string.
static inline void chop_suffix(std::string& str) {
  chop_suffix_at(str, '.');
}

/// @brief Removes the beginning of the input string starting at the first 
///        occurrence of char c.
///
/// This function modifies the input string.
///
/// @param    str        The input string.
/// @param    c          The character to search for.
void chop_prefix_at(std::string& str, char c);

/// @brief Removes prefix, starting at the last '.' character and beyond, from
///        the input string, if any.
///
/// This function modifies the input string. This function is equivalent to
/// calling bios::string::chop_prefix_at(str, '.').
///
/// @param    str        The input string.
static inline void chop_prefix(std::string& str) {
  chop_prefix_at(str, '.');
}

/// @brief Returns the index of the first non-numeric character in the string.
///
/// @param    str        The input string.
///
/// @return   Index of the first non-numerical character in the string.
size_t first_non_numeric(std::string& str, size_t pos);

/// @brief Returns the index of the first non-numeric character in the string.
///
/// @param    str        The input string.
///
/// @return   Index of the first non-numerical character in the string.
size_t first_non_numeric(std::string& str);

/// @brief Returns the index of the first numeric character in the string.
///
/// @param    str        The input string.
///
/// @return   Index of the first numerical character in the string.
size_t first_numeric(std::string& str, size_t pos);

/// @brief Returns the index of the first numeric character in the string.
///
/// @param    str        The input string.
///
/// @return   Index of the first numerical character in the string.
size_t first_numeric(std::string& str);

/// @brief Returns a new string consisting of the original input string with
///        the input word inserted at each nth position.
///
/// @param    str        The input string.
/// @param    word       The word to insert.
/// @param    n          The position to insert the word.
///
/// @return   A new string consisting of the input string with the word
///           inserted at every nth position.
std::string insert_word_every_nth(std::string& str, const char* word, int n);

/// @brief Returns a new string consisting of the original input string with
///        the input word inserted at each nth position.
///
/// @param    str        The input string.
/// @param    word       The word to insert.
/// @param    n          The position to insert the word.
///
/// @return   A new string consisting of the input string with the word
///           inserted at every nth position.
std::string insert_word_every_nth(std::string& str, std::string& word, 
                                  int n);

/// @brief Case insensitive version of find.
///
/// This method searches for the substring needle in the string haystack but
/// ignores case.
///
/// @param    haystack   The string to search.
/// @param    needle     The substring to look for.
///
/// @param    The index in haystack that the first instance of needle is found.
size_t find_case(std::string& haystack, const char* needle);

/// @brief Case insensitive version of find.
///
/// This method searches for the substring needle in the string haystack but
/// ignores case.
///
/// @param    haystack   The string to search.
/// @param    needle     The substring to look for.
///
/// @param    The index in haystack that the first instance of needle is found.
size_t find_case(std::string& haystack, std::string& needle);

/// @brief Copy a substring delimited by the given characters, excluding those
///        characters.
///
/// @param    str        The input string.
/// @param    begin      The character that delimits the start of the 
///                      substring.
/// @param    end        The character that delimits the end of the substring.
///
/// @return   The index after the location of end, or std::string::npos if no
///           substring extracted.
size_t copy_substr(std::string& str, char begin, char end, 
                   std::string& substr);

/// @brief Translates each character in the input string using the provided
///        character mappings.
///
/// This function modifies the input string. This function translates each
/// character from the input string which matches one of the characters in
/// 'from_chars' wtih the corresponding character from 'to_chars', or, if the
/// position in 'to_chars' is not filled, deletes the character from the input
/// string.
///
/// This function is similar to the UNIX command and the Perl function 'tr'.
/// For example, translate("abc", "ac", "b") modifies "abc" into "bb" and
/// returns 2. translate("a|b|c", "|", "|") would return the number of '|'
/// characters.
///
/// @param    str        The input string.
/// @param    from_chars The characters to translate from.
/// @param    to_chars   The characters to translate to.
/// 
/// @return   The number of characters translated or modified.
int translate(std::string& str, const char* from_chars, const char* to_chars);

/// @brief Removes leading and trailing characters from the input string.
///
/// This function modifies the input string. For example:
/// trim_chars("<<=text=>>", "=<", "=") returns 7 and leaves the output
/// "text=>>".
///
/// @param    str        The input string. If this string is empty, then
///                      nothing will happen.
/// @param    left       The set of characters to be removed from the left
///                      end of the input string, or NULL or empty string to
///                      leave the beginning of the input string as is.
/// @param    right      The set of characters to be removed from the right
///                      end of the input string, or NULL or empty string to
///                      leave the tail of the input string as is.
void trim_chars(std::string& str, const char* left, const char* right);

/// @brief Encrypts the input string such that it is unreadable to humans but
///        can easily be unscrambled() again.
///
/// The input string must not contain 0xff. This function modifies the input
/// string.
///
/// @param    str        The input string.
void scramble(std::string& str);

/// @brief Undos the encryption performed on the input string by scramble().
///
/// The input string must not contain 0xff. This function modifies the input
/// string.
///
/// @param    str        The input string.
void unscramble(std::string& str);

}; // namespace string

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_STRING_H__ */
