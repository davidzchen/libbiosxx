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

#include "common.h"

namespace str {

char* subString(char* str, int start, int end);

/**
 * Returns position of needle in haystack or NULL if it's not there. 
 */
static char* stringIn(char* needle, char* haystack) {
  return strstr(haystack, needle);
}

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
