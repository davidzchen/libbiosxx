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

void eraseWhiteSpace(char* s);

/// @brief Returns whether the input string contains any whitespace characters.
///
/// @param    str        The input string
///
/// @return   true if the string has whitespace characters, false otherwise.
static bool has_whitespace(std::string& str) {
  return str.find_first_of(kWhiteSpace) != std::string::npos;
}

/// @brief Returns whether the input string contains all whitespace characters.
///
/// @param    str        The input string.
///
/// @return   true if the string only contains whitespace characters, false
///           otherwise.
static bool is_whitespace(std::string& str) {
  return str.find_first_not_of(kWhiteSpace) == std::string::npos;
}

/// @brief Returns a copy of the first word in the line.
///
/// @param    word       The word to return to the caller.
/// @param    str        The input string.
///
/// @return   true if the last word is found and copied, false otherwise.
bool first_word_in_line(std::string& word, std::string& line);

/// @brief Returns a copy of the last word in the line.
///
/// @param    word       The word to return to the caller.
/// @param    str        The input string.
///
/// @return   true if last word is found and copied, false otherwise.
bool last_word_in_line(std::string& word, std::string& line);

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
static void chop_suffix(std::string& str) {
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
static void chop_prefix(std::string& str) {
  chop_prefix_at(str, '.');
}

/// @brief Returns the index of the first non-numeric character in the string.
///
/// @param    str        The input string.
///
/// @return   Index of the first non-numerical character in the string.
size_t first_non_numeric(std::string& str, size_t pos);

size_t first_non_numeric(std::string& str);

char* skipToNumeric(char* s);
char* insertWordEveryNthPosition (char *string, char *word, int n);
char *strCaseStr (char *s, char *t) ;  /* case-insensitive strstr() */
char *strCopySubstr(char *string, char begin, char end, std::string substr);
int strTranslate(char *s, char *fromChars, char *toChars) ;
void strScramble(char *s) ;
void strUnscramble(char *s) ;
int isBlankStr(char *s) ;
int strTrim(char *s, char *left, char *right) ;

}; // namespace string

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_STRING_H__ */
