/** 
 *   \file stringUtil.h
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

#ifndef BIOINFO_STRING_H__ 
#define BIOINFO_STRING_H__

#include <cctype>
#include <cstring>
#include <string>
#include <sstream>

#include "misc.h"

namespace str {

char* subString(char* str, int start, int end);

/**
 * Returns position of needle in haystack or NULL if it's not there. 
 */
static inline char* stringIn(char* needle, char* haystack) {
  return strstr(haystack, needle);
}

/**
 * Check if s1 starts with s2.
 * strStartsWith() always works, but is slower than strStartsWithC()
 * @note s2 must not be an expression with side effects since it is evaluated twice
 */
static inline int strStartsWith(char* s1, char* s2) {
  int len = strlen(s2);
  return strncmp(s1, s2, len) == 0;
}

/**
 * Check if s1 starts with s2.
 * Same as strStartsWith, but can only be used if s2 is a string constant, e.g. if (strStartsWithC(s, "CC   ")) ...strStartsWith() always works, but is slower than strStartsWithC()
 */
static inline int strStartsWithC(char* s1, const char* s2) {
  int len = sizeof(s2) - 1;
  return strncmp(s1, s2, len) == 0;
}

int strTrim(char *s, char *left, char *right) ;

char* rStringIn(char* needle, char *haystack);
char* stringBetween(char* start, char *end, char *haystack);
void toggleCase(char* s, int size);
void stripChar(char* s, char c);
int countChars(char* s, char c);
int countSame(char* a, char* b);
char* skipLeadingSpaces(char* s);
char* skipToSpaces(char* s);
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

}; // namespace str

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_STRING_H__ */
