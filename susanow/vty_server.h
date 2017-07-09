

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
#include "vty_client.h"


class vty_server {
  int                     server_fd;
  const std::string       bootmsg;
  const std::string       prompt;
  std::vector<vty_client> clients;
  std::vector<command*>   commands;
  std::vector<key_func*>  keyfuncs;

 public:
  void* user_ptr;

  vty_server(uint32_t addr_, uint16_t port_,
      const char* bootmsg_, const char* prompt_);
  virtual ~vty_server();

  void install_keyfunction(key_func* kf);
  void install_command(command* cmd);
  void dispatch();
};



