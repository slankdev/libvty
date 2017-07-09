

#pragma once
#include <stdint.h>
#include <string.h>
#include <string>
#include <slankdev/util.h>
#include <slankdev/exception.h>

#include "vty_keyfunction.h"
#include "vty_cmd_node.h"
#include "vty_cmd.h"
#include "vty_input_buffer.h"
#include "vty_cmd_history.h"
#include "vty_shell.h"


class vty {
  const uint16_t          port;
  const std::string       bootmsg;
  const std::string       prompt;
  int                     server_fd;
  std::vector<shell>      shells;
  std::vector<command*>   commands;
  std::vector<key_func*>  keyfuncs;

  int get_server_sock();
 public:
  void* user_ptr;

  vty(uint16_t p, const char* msg, const char* prmpt);
  virtual ~vty();
  void install_keyfunction(key_func* kf);
  void install_command(command* cmd);
  void add_default_keyfunctions();
  void init_default_keyfunc();
  void dispatch();
};



class vty;
class ssn_vty {
 public:
  vty* v;
  ssn_vty(uint32_t addr, uint16_t port);
  virtual ~ssn_vty();
};

void ssn_vty_poll_thread(void* arg);
void ssn_vty_poll_thread_stop();


