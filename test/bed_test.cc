#include <iostream>

#include <bios/bed.hh>
#include <gtest/gtest.h>

TEST(BedParser, DefaultConstructor) {
  bios::BedParser parser;
  bios::Bed* b = parser.NextEntry();
  EXPECT_EQ(NULL, b);
}

TEST(BedParser, ParseBasic) {
  bios::BedParser parser;
  parser.InitFromFile("./in/basic.bed");
  bios::Bed* b = parser.NextEntry();
  std::cout << b->ToString() << std::endl;
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
