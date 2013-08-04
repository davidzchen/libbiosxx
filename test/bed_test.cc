#include <bios/bed.hh>
#include <gtest/gtest.h>

TEST(BedParser, DefaultConstructor) {
  bios::BedParser parser;
  bios::Bed* b = parser.NextEntry();
  EXPECT_EQ(NULL, b);
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
