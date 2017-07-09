
#pragma once
#include <string>
#include <vector>

class shell;
class command {
 public:
  std::vector<node*> nodes;
  virtual ~command() {}
  virtual void func(shell* sh) = 0;
  virtual bool match(const std::string& str);
};

