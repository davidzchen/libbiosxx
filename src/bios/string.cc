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
/// This module implements additional functions for manipulating std::string 
/// strings.

#include "string.hh"

namespace bios {

namespace string {

bool string_between(std::string& between, std::string& haystack,
                    std::string& start, std::string& end) {
  size_t start_pos = haystack.find(start);
  if (start_pos == std::string::npos) {
    return false;
  }
  size_t end_pos = haystack.find(end, start_pos + start.size());
  if (end_pos == std::string::npos) {
    return false;
  }
  between = haystack.substr(start_pos + start.size(), 
                            end_pos - start_pos + start.size());
  return true;
}

void toggle_case(std::string& str) {
  for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
    char c = *it;
    if (isupper(c)) {
      c = tolower(c);
    } else if (islower(c)) {
      c = toupper(c);
    }
    *it = c;
  }
}

void to_upper(std::string& str) {
  for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
    *it = toupper(*it);
  }
}

void to_lower(std::string& str) {
  for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
    *it = tolower(*it);
  }
}

size_t count_char(std::string& str, char c) {
  size_t count = 0;
  for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
    if (*it == c) {
      ++count;
    }
  }
  return count;
}

size_t count_same(std::string& a, std::string& b) {
  size_t count = 0;
  for (unsigned int i = 0; ; ++i) {
    if (a[i] == '\0' || b[i] == '\0') {
      break;
    }
    if (b[i] != a[i]) {
      break;
    }
    ++count;
  }
  return count;
}

void rtrim(std::string& str) {
  size_t pos = str.find_last_not_of(kWhiteSpaces);
  if (pos == std::string::npos) {
    str.clear();
  } else {
    str.erase(found + 1);
  }
}

void ltrim(std::string& str) {
  size_t pos = str.find_first_not_of(kWhiteSpaces);
  if (pos == std::string::npos) {
    str.clear();
  } else {
    str.erase(0, pos);
  }
}

void trim(std::string& str) {
  rtrim(str);
  ltrim(str);
}

/**
 * Remove white space from a string.
 */
void eraseWhiteSpace(char* s) {
  char* in, *out;
  char c;
  
  in = out = s;
  for (;;) {
    c = *in++;
    if (c == 0) {
      break;
    }
    if (!isspace(c)) {
      *out++ = c;
    }
  }
  *out++ = 0;
}

bool first_word_in_line(std::string& word, std::string& line) {
  size_t start = line.find_first_not_of(kWhiteSpaces);
  if (start == std::string::npos) {
    return false;
  }
  size_t end = line.find_first_of(kWhiteSpaces, start);
  if (end == std::string::npos) {
    end = line.size();
  }
  word = line.substr(start, end - start);
  return true;
}

std::string last_word_in_line(std::string& line) {
  size_t end = line.find_last_not_of(kWhiteSpaces);
  if (end == std::string::npos) {
    return false;
  }
  size_t start = line.find_last_of(kWhiteSpaces, end);
  if (start == std::string::npos) {
    start = 0;
  }
  word = line.substr(start, end - start);
}

void chop_suffix_at(std::string& str, char c) {
  size_t pos = str.find_last_of(c);
  if (pos == std::string::npos) {
    return;
  }
  str.erase(pos, str.size() - pos - 1);
}

void chop_prefix_at(std::string& str, char c) {
  size_t pos = str.find_first_of(c);
  if (pos == std::string::npos) {
    return;
  }
  str.erase(0, pos + 1);
}

size_t first_non_numeric(std::string& str, size_t pos) {
  if (pos >= str.size()) {
    return std::string::npos;
  }
  while (isdigit(s[pos]) != false) {
    ++pos;
  }
  return pos;
}

size_t first_non_numeric(std::string& str) {
  return first_non_numeric(str, 0);
}

size_t first_numeric(std::string& str, size_t pos) {
  if (pos >= str.size()) {
    return std::string::npos;
  }
  while (!isdigit(s[pos]) != false) {
    ++pos;
  }
  return pos;
}

size_t first_numeric(std::string& str) {
  return first_numeric(str, 0);
}

std::string& insert_word_every_nth(std::string& str, const char* word, int n) {
  std::stringstream string_buffer;
  int len = str.size();
  int i = 0;
  while (i < len) {
    string_buffer << string[i];
    i++;
    if ((i % n) == 0 && i != len) {
      string_buffer << word;
    }  
  }
  return std::string(string_buffer.str());
}

std::string& insert_word_every_nth(std::string& str, std::string& word, 
                                   int n) {
  return insert_word_every_nth(str, word.c_str(), n);
}

/**
 * Case-insensitive version of strstr(3C) from the C-libarary.
 * @param[in] s String to be searched in (subject)
 * @param[in] t String to look for in s (query)
 * @return If t is the empty string return s, else if t does not occur in s 
 *         return NULL, 
   else pointer to first position of t in s
 */
char *strCaseStr (char *s, char *t) { 
  char *p , *r ;
  if (*t == '\0') {
    return s ;
  }
  for ( ; *s != '\0'; s++) {
    for (p = s, r = t; *r != '\0' && tolower(*p) == tolower(*r); p++, r++) 
      ;
    if (r > t && *r == '\0') {
      return s ;
    }
  }
  return NULL ;
}

/**
 * From a supplied string copy a substring delimited by two supplied 
 * characters, excluding these characters.
 * @param[in] string String to copy from
 * @param[in] begin Start copying after the leftmost occurrence of this 
 *            character in string
 * @param[in] end Stop copying before the leftmost occurrence of this 
 *            character from occurrence of begin on; 
   may be null-terminated to copy to the end of string
 * @param[in] substr Stringa, must exist
 * @param[out] substr Filled with string extracted; empty string if nothing 
 *             extracted;
 * @return Position after location of end, NULL if no substring extracted
 */
char *strCopySubstr(char *string, char begin, char end, std::string& substr) { 
  char *pbegin = NULL;
  char *pend = NULL;
  char *nextPos = NULL;
  substr.clear();
  if ((pbegin = strchr(string, begin)) != NULL) {
    ++pbegin;
    if ((pend = strchr(pbegin, end)) != NULL) {
      nextPos = pend + 1;
      --pend;
      substr.copy(pbegin, pend - pbegin + 1);
    }
  }
  return nextPos;
}

/** 
 * Translates each character from 's' which matches one of the characters in 
 * 'fromChars' with the corresponding character from 'toChars' or, if this 
 * position in 'toChars' is not filled, deletes this character from s, thus 
 * shortening 's'.
 * This function resembles the Unix command and the Perl function 'tr'.
  \verbatim
   example: strTranslate("abc", "ac", "b") modifies
            "abc" into "bb" and returns 2
            strTranslate("a|b|c", "|", "|")
            just counts the number of '|' chars
  \endverbatim   
 * @param[in] s
 * @param[in] fromChars
 * @param[in] toChars
 * @param[out] s
 * @return Number of chars translated or modified
*/
int strTranslate(char *s, char *fromChars, char *toChars) { 
  char *from = s - 1 ;
  char *to = s ;
  char c ;
  int toLen = strlen(toChars) ;
  char *hit ;
  int cnt = 0 ;

  while ((c = *++from) != '\0') {
    if ((hit = strchr(fromChars, c)) != NULL) {
      ++cnt ;
      if (hit - fromChars < toLen) {
        *to++ = toChars[hit - fromChars];
      }
    } else {
      *to++ = c ;
    }
  }
  *to = '\0' ;
  return cnt ;
}

/**
 * Remove leading and trailing characters from s. 
  \verbatim
  example: strTrim("<<=text=>>", "=<", "=")
           returns 7 and leaves output "text=>>" 
 \endverbatim  	   
 * @param[in] s Zero-terminated string of char or NULL (nothing will happen)
 * @param[in] left Set of chars to be removed from left end of s, NULL or empty 
 *            string to leave beginning of s as is
 * @param[in] right Set of chars to be removed from right end of s, NULL or 
 *            empty string to leave tail of s as is
 * @param[out] s Changed
 * @return Length of s after trim
 */
/* tried to keep this efficent:
   - don't use strlen() unless unavoidable
   - first remove right side, because then there is less
   to be shifted left
*/ 
int strTrim(char *s, char *left, char *right) { 
  int len ;
  char *cp ;
  if (!s) {
    return 0;
  }
  len = strlen(s);
  if (len && right) {
    /* move to last char of sequence, then run as long as
       there are only chars from 'right' or we bump into the
       beginning of the string */
    cp = s + len - 1;  
    while( cp >= s && strchr(right, *cp)) {
      *cp-- = '\0';
    }
    len = cp - s + 1;
  }

  if (len && left) {
    /* move cp to the first char not in 'left'; then start
       shuffling chars from this position onward down to the beginning
       of the string */
    cp = s ;
    while (*cp && strchr(left, *cp)) {
      ++cp;
    }
    len = len - (cp - s) ;
    while (*cp) {
      *s++ = *cp++ ;
    }
    *s = '\0' ;
  }
  return len ;
}

void scramble(std::string& str) { 
  for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
    if (*it == 255) {
      std::cerr << "clsv_scramble: cannot scramble 0xFF" << std::endl;
      return;
    }
    *it = *it ^ 255;
  }
}

void unscramble(std::string& s) { 
  scramble(s);
}

}; // namespace string

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
