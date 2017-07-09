
#pragma once
#include <vector>
#include <vty_cmd_node.h>
#include <slankdev/string.h>


class vty_cmd_match {
 public:
  std::vector<node*> nodes;
  vty_cmd_match() {}
  bool is_match(const std::string& str) const
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
};


