#include <cstdlib>

#include <gtest/gtest.h>
#include <bios/worditer.hh>

TEST(WordIter, EmptyStringTest) {
  char* s = strdup("");
  bios::WordIter w(s, "", false);
  char* next = w.Next();
  EXPECT_EQ(NULL, next);
  free(s);
}

TEST(WordIter, NoCollapseSeparatorsTest) {
  char* s = strdup("The quick brown fox");
  bios::WordIter w(s, " ", false);
  char* next = w.Next();
  EXPECT_STREQ(next, "The");
  next = w.Next();
  EXPECT_STREQ(next, "quick");
  next = w.Next();
  EXPECT_STREQ(next, "brown");
  next = w.Next();
  EXPECT_STREQ(next, "fox");
  free(s);
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
