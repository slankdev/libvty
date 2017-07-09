
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <vty_server.h>


typedef void (*vty_callback_t)(void* arg);

class ssn_vty {
  vty_server* v;
 public:
  ssn_vty(uint32_t addr, uint16_t port);
  virtual ~ssn_vty();

  void poll_dispatch();
  void install_command(vty_cmd_match m, vty_cmdcallback_t f, void* arg);
};

extern bool vty_running;
void vty_poll(void* arg);

