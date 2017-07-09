
#pragma once

#include <vty_server.h>
#include <stddef.h>
#include <unistd.h>


typedef void (*vty_callback_t)(void* arg);

class ssn_vty {
 public:
  vty_server* v;
  ssn_vty(uint32_t addr, uint16_t port);
  virtual ~ssn_vty();

  void new_install_command(const char* match, vty_callback_t cb, void* arg) {}
};

void ssn_vty_poll_thread(void* arg);
void ssn_vty_poll_thread_stop();


