/** 
 *   \file stringUtil.c Additional string utilities
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#include "string.h"

namespace str {

/**
 * Get a substring.
 * @param str A valid string (not NULL)
 * @param start Etart index (inclusive)
 * @param end End index (inclusive)
 * @return The substring
 */
char* subString(char* str, int start, int end) {
  std::stringstream string_buffer;
  int i = start;
  while (i <= end) {
    string_buffer << str[i];
    i++;
  }
  return strdup(string_buffer.str().c_str());
}

/**
 * Return last position of needle in haystack, or NULL if it's not there.
 */
char* rStringIn(char* needle, char* haystack) {
  int nSize = strlen(needle);
  char* pos;
  for (pos = haystack + strlen(haystack) - nSize; pos >= haystack; pos -= 1) {
    if (memcmp(needle, pos, nSize) == 0) {
      return pos;
    }
  }
  return NULL;
}

/**
 * Return string between start and end strings, or NULL if none found.  
 * The first such instance is returned. 
 * @note String must be freed by caller. 
 */
char* stringBetween(char* start, char* end, char* haystack) {
  char* pos, *p;
  int len;
  if ((p = stringIn(start, haystack)) != NULL) {
    pos = p + strlen(start);
    if ((p = stringIn(end, pos)) != NULL) {
      len = p - pos;
      pos = (char*) cloneMem(pos, len + 1);
      pos[len] = 0;
      return pos;
    }
  }
  return NULL;
}

/**
 * Return last character in string. 
 * If s == NULL or s[o] == 0, return 0
 */
char lastChar(char* s) {
  if (s == NULL || s[0] == 0) {
    return 0;
  }
  return s[strlen(s)-1];
}

/**
 * Toggle upper and lower case chars in string. 
 */
void toggleCase(char* s, int size) {
  for (int i = 0; i < size; ++i) {
    char c = s[i];
    if (isupper(c)) {
      c = tolower(c);
    } else if (islower(c)) {
      c = toupper(c);
    }
    s[i] = c;
  }
}

/**
 * Substitute newChar for oldChar throughout string s. 
 */
void subChar(char* s, char oldChar, char newChar) {
  for (;;) {
    char c = *s;
    if (c == 0) {
      break;
    }
    if (c == oldChar) {
      *s = newChar;
    }
    ++s;
  }
}

/**
 * Remove all occurences of c from s. 
 */
void stripChar(char* s, char c) {
  char* in = s, *out = s;
  for ( ; ; ) {
    char b = *out = *in++;
    if (b == 0) {
      break;
    }
    if (b != c) {
      ++out;
    }
  }
}

/**
 * Return number of characters c in string s. 
 */
int countChars(char* s, char c) {
  char a;
  int count = 0;
  while ((a = *s++) != 0) {
    if (a == c) {
      ++count;
    }
  }
  return count;
}

/**
 * Count number of characters that from start in a,b that are same. 
 */
int countSame(char* a, char* b) {
  int count = 0;
  for (int i = 0; ; ++i) {
    char c = a[i];
    if (b[i] != c) {
      break;
    }
    if (c == 0) {
      break;
    }
    ++count;
  }
  return count;
}

/** 
 * Return first non-white space. 
 */
char* skipLeadingSpaces(char* s) {
  if (s == NULL) {
    return NULL;
  }
  for (;;) {
    char c = *s;
    if (!isspace(c)) {
      return s;
    }
    ++s;
  }
}

/**
 * Return first white space or NULL if none. 
 */
char* skipToSpaces(char* s) {
  if (s == NULL) {
    return NULL;
  }
  for ( ; ; ) {
    char c = *s;
    if (c == 0) {
      return NULL;
    }
    if (isspace(c)) {
      return s;
    }
    ++s;
  }
}

/**
 * Replace trailing white space with zeroes.
 */
void eraseTrailingSpaces(char* s) {
  int len = strlen(s);
  for (int i = len - 1; i >= 0; --i) {
    char c = s[i];
    if (isspace(c)) {
      s[i] = 0;
    } else {
      break;
    }
  }
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

/**
 * Remove leading and trailing white space.
 */
char* trimSpaces(char* s) {
  if (s != NULL) {
    s = skipLeadingSpaces(s);
    eraseTrailingSpaces(s);
  }
  return s;
}

/**
 * Return 1 if there is white space in string. 0 otherwise. 
 */
int hasWhiteSpace(char* s) {
  char c;
  while ((c = *s++) != 0) {
    if (isspace(c)) {
      return 1;
    }
  }
  return 0;
}

/**
 * Returns first word in line if any (white space separated).
 * Puts 0 in place of white space after word. 
 */
char* firstWordInLine(char* line) {
  char* e;
  line = skipLeadingSpaces(line);
  if ((e = skipToSpaces(line)) != NULL) {
    *e = 0;
  }
  return line;
}

/** 
 * Returns last word in line if any (white space separated).
 * Returns NULL if string is empty.  Removes any terminating white space
 * from line. 
 */
char* lastWordInLine(char* line) {
  char* s = line;
  char* word = NULL, *wordEnd = NULL;
  for ( ; ; ) {
    s = skipLeadingSpaces(s);
    if (s == NULL || s[0] == 0) {
      break;
    }
    word = s;
    s = wordEnd = skipToSpaces(s);
    if (s == NULL) {
      break;
    }
  }
  if (wordEnd != NULL) {
    *wordEnd = 0;
  }
  return word;
}

/**
 * Adds head to suffix. In this case returns 'headsuffix'. 
 */
char* addSuffix(char* head, char* suffix) {
  std::stringstream string_buffer;
  string_buffer << head << suffix;
  return strdup(string_buffer.str().c_str());
}

/** 
 * Remove end of string from first occurrence of char c. 
 * chopSuffixAt(s, '.') is equivalent to regular chopSuffix. 
 * @see chopSuffix()
 */
void chopSuffixAt(char* s, char c) {
  char* e = strrchr(s, c);
  if (e != NULL) {
    *e = 0;
  }
}

/**
 * Remove suffix (last dot in string and beyond) if any. 
 */
void chopSuffix(char* s) {
  chopSuffixAt(s,'.');
}

/**
 * Like chopPrefix, but can chop on any character, not just dot.
 * @see chopPrefix() 
 */
char* chopPrefixAt(char* s, char c) {
  char* e = strchr(s, c);
  if (e == NULL) {
    return s;
  }
  *e++ = 0;
  return e;
}

/** 
 * This will replace the first dot in a string with 0, and return the character after this.  
 * If there is no '.' in the string this will just return the unchanged s passed in. 
 */
char* chopPrefix(char* s) {
  return chopPrefixAt(s, '.');
}

static const char* const kNaStr = "N/A";
static const char* const kEmptyStr = "";

/**
 * Return NULL if s is all spaces, otherwise s. 
 */
char* nullIfAllSpace(char* s) {
  s = skipLeadingSpaces(s);
  if (s != NULL)
    if (s[0] == 0)
      s = NULL;
  return s;
}

/** 
 * Return "true" or "false". 
 */
const char* trueFalseString(int b) {
  return (b ? "true" : "false");
}

/**
 * Return first char of s that's not a digit.
 */
char* skipNumeric(char* s) {
  while (isdigit(*s)) {
    ++s;
  }
  return s;
}

/** 
 * Skip up to where numeric digits appear.
 */
char* skipToNumeric(char* s) {
  while (*s != 0 && !isdigit(*s)) {
    ++s;
  }
  return s;
}

/**
 * Insert word at every nth postion, but not at the end.
 */
char* insertWordEveryNthPosition(char* string, char* word, int n) {
  std::stringstream string_buffer;
  int len = strlen (string);
  int i = 0;
  while (i < len) {
    string_buffer << string[i];
    i++;
    if ((i % n) == 0 && i != len) {
      string_buffer << word;
    }  
  }
  return strdup(string_buffer.str().c_str());
}

}; // namespace str

/* vim: set ai ts=2 sts=2 sw=2 et: */
