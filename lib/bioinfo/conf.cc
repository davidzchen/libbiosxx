#include "conf.h"

Conf::Conf(const char* filename) {
  if (filename == NULL) {
    return;
  }
  filename_ = strdup(filename);
  Read();
}

Conf::~Conf() {
  free(filename_);
  map_.clear();
}

int Conf::ParseLine(char* line, int n) {
  char* start = line;
  char* end = start + strlen(line) - 1;
  char* c = strchr(line, '#');
  if (c != NULL) {
    end = c;
  }

  /* Skip leading whitespace */
  for (c = start; *c && isspace(*c); c++)
    ;
  
  if (!*c || *c == '#') {
    return 0;
  }

  char* mid = NULL;
  if ((mid = strchr(line, '=')) == NULL) {
    fprintf(stderr, "Syntax error in config file line %d: missing '='\n", n);
    return -1;
  }

  start = c;
  if (c == mid) {
    fprintf(stderr, "Syntax error in config file line %d: missing key\n", n);
    return -1;
  }

  /* Find end of key token and copy key*/
  for ( ; *c && !isspace(*c) && c != mid; c++)
    ;
  end = c;

  int len = end - start;
  char* key = (char *) malloc((len + 1) * sizeof(char));
  strncpy(key, start, len);
  *(key + len) = '\0';

  /* Skip whitespace before value token */
  for (c = mid + 1; *c && isspace(*c); c++)
    ;

  start = c;
  int quote = 0;
  if (*c == '"') {
    quote = 1;
    start++;
  }

  /* Find end of value token and copy value */
  if (quote) {
    for (c = start; *c && *c != '"'; c++)
      ;
  } else {
    for (c = start; *c && !isspace(*c); c++)
      ;
  }
  end = c;

  if (quote && *c != '"') {
    fprintf(stderr, "Syntax error in config file line %d: unmatched quote\n", n);
    free(key);
    return -1;
  }

  len = end - start;
  char* val = (char *) malloc((len + 1) * sizeof(char));
  strncpy(val, start, len);
  *(val + len) = '\0';

  std::string k(key);
  std::string v(val);
  map_[k] = v;

  free(key);
  free(val);
  return 0;
}

int Conf::Read() {
  LineStream* ls = LineStream::FromFile(filename_);
  if (ls == NULL) {
    fprintf(stderr, "Cannot open config file %s\n", filename_);
    return -1;
  }

  int n = 1;
  char* line = NULL;
  while ((line = ls->GetLine()) != NULL) {
    if (ParseLine(line, n) < 0) {
      fprintf(stderr, "Failed to load config file %s\n", filename_);
      return -1;
    }
    ++n;
  }
  delete ls;
  return n;
}

const char* Conf::Get(const char* key) {
  std::map<std::string, std::string>::iterator it = map_.find(key);
  if (it == map_.end()) {
    return NULL;
  } else {
    std::string& value = (*it).second;
    return value.c_str();
  }
}

/* vim: set ai ts=2 sts=2 sw=2 et: */
