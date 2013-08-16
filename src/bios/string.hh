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
/// subString - implemented by std::string::substr()
/// stringIn  - implemented by std::string::find()
/// rStringIn - implemented by std::string::rfind()
/// lastChar  - can be implemented easily using str[str.size() - 1];
/// subChar   - can be implemented using std::replace in <algorithm>
/// stripChar - can be implemented using std::remove in <algorithm>

#ifndef BIOS_STRING_H__ 
#define BIOS_STRING_H__

#include <cctype>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>

#include "misc.hh"

namespace bios {

namespace str {

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
size_t skip_leading_spaces(std::string& str);

/// @brief Returns the index of the first whitespace-character in str.
///
/// @param    str        The string.
///
/// @return   The index of the first whitespace character in str.
char* skipToSpaces(char* s);

int strTrim(char *s, char *left, char *right) ;

void eraseTrailingSpaces(char* s);
void eraseWhiteSpace(char* s);
char* trimSpaces(char* s);
int hasWhiteSpace(char* s);
char* firstWordInLine(char* line);
char* lastWordInLine(char* line);
char* addSuffix(char* head, char* suffix);
void chopSuffix(char* s);
void chopSuffixAt(char* s, char c);
char* chopPrefix(char* s);
char* chopPrefixAt(char* s, char c);
char* nullIfAllSpace(char* s);
const char *trueFalseString(int b);
char* skipNumeric(char* s);
char* skipToNumeric(char* s);
char* insertWordEveryNthPosition (char *string, char *word, int n);

void strReplace(char **s1, char *s2) ;
char *strCaseStr (char *s, char *t) ;  /* case-insensitive strstr() */
char *strCopySubstr(char *string, char begin, char end, std::string substr);
int strTranslate(char *s, char *fromChars, char *toChars) ;
void strScramble(char *s) ;
void strUnscramble(char *s) ;
int isBlankStr(char *s) ;

}; // namespace str

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_STRING_H__ */
