
#include <vty_cmd_history.h>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>

std::string commandhistory::deep_get()
{
  if (history.empty()) return "";
  if (hist_index+2 > history.size()) return *(history.end() - hist_index - 1);
  return *(history.end() - ++hist_index);
}
std::string commandhistory::shallow_get()
{
  if (history.empty()) return "";
  if (ssize_t(hist_index)-1 < 0) return *(history.end() - hist_index - 1);
  return *(history.end() - --hist_index - 1);
}

