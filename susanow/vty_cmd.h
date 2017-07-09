
#pragma once
#include <string>
#include <vector>
#include <vty_cmd_node.h>

class vty_client;

class command {
 public:
  std::vector<node*> nodes;
  virtual ~command() {}
  virtual void func(vty_client* sh) = 0;
  virtual bool match(const std::string& str);
};

