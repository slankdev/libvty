
#pragma once
#include <string>
#include <vty_cmd_match.h>

class vty_client;

struct vty_cmd {
  void* arg;
  vty_cmd_match match;
  virtual ~vty_cmd() {}
#if 1
  virtual void func(vty_client* sh) = 0;
#endif
};

