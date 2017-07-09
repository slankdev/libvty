
#pragma once
#include <vector>
#include <vty_cmd_node.h>


class vty_cmd_match {
 public:
  std::vector<node*> nodes;
  vty_cmd_match() {}
};


