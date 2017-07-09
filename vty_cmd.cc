
#include <stddef.h>
#include <slankdev/string.h>
#include <string>
#include "vty_cmd.h"

bool vty_cmd::match(const std::string& str)
{
  std::vector<std::string> list = slankdev::split(str, ' ');
  if (list.size() != match_.nodes.size()) {
    return false;
  }

  for (size_t i=0; i<list.size(); i++) {
    if (match_.nodes[i]->match(list[i]) == false) {
      return false;
    }
  }
  return true;
}
