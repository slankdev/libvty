

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

inline std::string commandhistory::deep_get()
{
  if (history.empty()) return "";
  if (hist_index+2 > history.size()) return *(history.end() - hist_index - 1);
  return *(history.end() - ++hist_index);
}
inline std::string commandhistory::shallow_get()
{
  if (history.empty()) return "";
  if (ssize_t(hist_index)-1 < 0) return *(history.end() - hist_index - 1);
  return *(history.end() - --hist_index - 1);
}


