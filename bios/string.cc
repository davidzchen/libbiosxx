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

void erase_whitespace(std::string& str) {
  std::string::iterator s = str.begin();
  for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
    char c = *it;
    if (!isspace(c)) {
      *s = c;
    }
  }
  str.erase(s, str.end());
}

std::string first_word_in_line(std::string& line, size_t pos) {
  std::string word("");
  size_t start = line.find_first_not_of(kWhiteSpaces, pos);
  if (start == std::string::npos) {
    return false;
  }
  size_t end = line.find_first_of(kWhiteSpaces, start);
  if (end == std::string::npos) {
    end = line.size();
  }
  word = line.substr(start, end - start);
  return word;
}

std::string first_word_in_line(std::string& line) {
  return first_word_in_line(line, 0);
}

std::string last_word_in_line(std::string& line, size_t pos) {
  std::string word("");
  size_t end = line.find_last_not_of(kWhiteSpaces, pos);
  if (end == std::string::npos) {
    return word;
  }
  size_t start = line.find_last_of(kWhiteSpaces, end);
  if (start == std::string::npos) {
    start = 0;
  }
  word = line.substr(start, end - start);
  return word;
}

std::string last_word_in_line(std::string& line) {
  return last_word_in_line(line, 0);
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

size_t find_case(std::string& haystack, const char* needle, 
                 size_t needle_size) {
  if (needle_size == 0 || needle == NULL) {
    return std::string::npos;
  }
  for (size_t i = 0; i < haystack.size(); ++i) {
    size_t j = i;
    size_t k = 0;
    while (j < haystack.size() && k < needle_size && 
        tolower(haystack[j]) == tolower(needle[j])) {
      ++j;
      ++k;
    }
    if (k == needle_size) {
      return i;
    }
  }
  return std::string::npos;
}

size_t find_case(std::string& haystack, const char* needle) {
  return find_case(haystack, needle, strlen(needle));
}

size_t find_case(std::string& haystack, std::string& needle) {
  return find_case(haystack, needle.c_str(), needle.size());
}

size_t copy_substr(std::string& str, char begin, char end, 
                   std::string& substr) {
  size_t substr_begin = str.find(&begin);
  if (substr_begin == std::string::npos) {
    return std::string::npos;
  }
  ++substr_begin;
  size_t substr_end = str.find(&end, substr_begin);
  if (substr_end == std::string::npos) {
    return std::string::npos;
  }
  substr = str.substr(substr_begin, substr_end - substr_begin);
  return substr_end + 1;
}

int translate(std::string& str, const char* from_chars, const char* to_chars) {
  if (from_chars == NULL || to_chars == NULL) {
    return 0;
  }
  size_t to_chars_len = strlen(to_chars);
  int count = 0;
  std::string::iterator from_it = str.begin();
  std::string::iterator to_it = from_it;
  for ( ; from_it != str.end(); ++from_it) {
    char* hit = strchr(from_chars, *it);
    if (hit != NULL) {
      ++count;
      size_t hit_index = hit - from_chars;
      if (hit_index < to_chars_len) {
        *to = to_chars[hit_index];
        ++to_it;
      }
    } else {
      *to_it = *from_it;
      ++to_it;
    }
  }
  str.erase(to_it, str.end());
  return count;
}

void trim_chars(std::string& str, const char* left, const char* right) { 
  size_t len = str.size();
  // Move to the last character of the sequence, then run as long as there are
  // only characters from 'right' or the beginning of the string is reached.
  if (str.empty() == false && right != NULL) {
    std::string::iterator it = str.end();
    while (it != str.begin() && strchr(right, *it) != NULL) {
      str.remove(it);
      --it;
    }
    len = std::distance(str.begin(), it) + 1;
  }
  // Move to the first character not in 'left' then erase all characters from
  // the beginning of the string to this character.
  if (str.empty() == false && left != NULL) {
    std::string::iterator it = str.begin();
    while (it != str.end() && strchr(left, *it) != NULL) {
      ++it;
    }
    len -= std::distance(str.begin(), it);
    str.erase(str.begin(), it);
  }
  return len;
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
