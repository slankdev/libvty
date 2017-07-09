
#pragma once
#include <string>
#include <vty_cmd_match.h>

class vty_client;
typedef void (*vty_cmdcallback_t)(vty_cmd_match *m, vty_client* sh, void* arg);

struct vty_cmd final {
  vty_cmd_match match;
  vty_cmdcallback_t f;
  void* arg;
};

