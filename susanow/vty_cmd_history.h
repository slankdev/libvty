

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <string>

class commandhistory {
  size_t hist_index;
  std::vector<std::string> history;
 public:
  commandhistory() : hist_index(0) {}
  void add(const std::string& str) { history.push_back(str); }
  void clean() { hist_index=0; }
  std::string deep_get();
  std::string shallow_get();
};



