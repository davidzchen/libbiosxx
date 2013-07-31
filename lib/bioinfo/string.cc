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
      pos = (char*) clone_mem(pos, len + 1);
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

/** 
 * Replace previous contents of s1 with copy of s2. s2 can be NULL. 
   This function is the same as strdup() from the C library, 
   except that it free()s the target string before duplicating.
 * @param[in] s1 Place where a pointer to a string is stored
 * @param[in] s2 Contents of s2 will replace contents of s1
 * @param[out] s2 Pervious contents free()d, new memory allocated
 */
void strReplace(char **s1, char *s2) { 
  if (!s1) {
    return;
  }
  free(*s1) ;
  if (s2) {
    *s1 = strdup(s2) ;
  }
}

/** 
 * Converts string to uppercase. 
 */
void toupperStr(char *s) { 
  char *cp = s - 1 ;
  while (*++cp) {
    *cp = toupper(*cp) ;
  }
}

/** 
 * Converts string to lowercase. 
 */
void tolowerStr(char *s) {
  char *cp = s - 1 ;
  while (*++cp) {
    *cp = tolower(*cp) ;
  }
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
  if (pbegin = strchr(string, begin)) {
    ++pbegin;
    if (pend = strchr(pbegin, end)) {
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

  while (c = *++from) {
    if (hit = strchr(fromChars, c)) {
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

/**
 * Encrypt the input string such that it is unreadable to humans and can 
 * easily be strUnscrambled() again.
 * @param[in] s Must not contain 0xFF
 * @param[out] s Scrambled
 */
void strScramble(char *s) { 
  char *cp = s - 1 ;
  while (*++cp) {
    if ((unsigned char)*cp == 255) {
      die((char*) "clsv_scramble: cannot scramble 0xFF") ;
    }
    *cp = *cp ^ 255 ;
  }
}

/**
 * Antidot for strScramble().
 */
void strUnscramble(char *s) { 
  strScramble(s) ;  /* scramble + scramble = unscramble */
}

/**
 * Check if s is blank.
 * @return True, if string consists of whitespace only or is of length 0, else 
 *         returns false 
 */
int isBlankStr(char *s) {
  char *cp = s-1 ;
  while (*++cp && isspace(*cp))
    ;
  return *cp == '\0' ;
}

}; // namespace str

/* vim: set ai ts=2 sts=2 sw=2 et: */
