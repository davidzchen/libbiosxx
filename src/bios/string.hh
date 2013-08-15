/** 
 *   \file stringUtil.h
 *   \author Lukas Habegger (lukas.habegger@yale.edu)
 */

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

static inline bool starts_with(std::string& haystack, const char* needle) {
  return haystack.find(needle) == 0;
}

size_t find_between(std::string& haystack, const char* needle, 
                    size_t start, size_t end);

void toggle_case(std::string* str, size_t size);

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

void strReplace(char **s1, char *s2) ;
void toupperStr(char *s) ; /* converts string to uppercase */
void tolowerStr(char *s) ; /* converts string to lowercase */
char *strCaseStr (char *s, char *t) ;  /* case-insensitive strstr() */
char *strCopySubstr(char *string, char begin, char end, std::string substr);
int strTranslate(char *s, char *fromChars, char *toChars) ;
int strTrim(char *s, char *left, char *right) ;
void strScramble(char *s) ;
void strUnscramble(char *s) ;
int isBlankStr(char *s) ;

}; // namespace str

}; // namespace bios

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOS_STRING_H__ */
