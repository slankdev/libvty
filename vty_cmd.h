
#pragma once
#include <string>
#include <vty_cmd_match.h>

class vty_client;

class vty_cmd {
 public:
  vty_cmd_match match_;
  virtual ~vty_cmd() {}
  virtual void func(vty_client* sh) = 0;
};

