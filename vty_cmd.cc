
#include <stddef.h>
#include <slankdev/string.h>
#include <string>
#include "vty_cmd.h"

bool command::match(const std::string& str)
{
  std::vector<std::string> list = slankdev::split(str, ' ');
  if (list.size() != nodes.size()) {
    return false;
  }

  for (size_t i=0; i<list.size(); i++) {
    if (nodes[i]->match(list[i]) == false) {
      return false;
    }
  }
  return true;
}
