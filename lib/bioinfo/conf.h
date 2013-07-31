/**
 * \file confp.h
 * \author Created by David Z. Chen (david.chen@yale.edu)
 */

#ifndef BIOINFO_CONF_H__
#define BIOINFO_CONF_H__

#include <map>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "linestream.h"

class Conf {
 public:
  Conf(const char* filename);
  ~Conf();

  const char* Get(const char* key);

 private:
  int Read();
  int ParseLine(char* line, int n);

 private:
  std::map<std::string, std::string> map_;
  char* filename_;
};

/* vim: set ai ts=2 sts=2 sw=2 et: */
#endif /* BIOINFO_CONF_H__ */
