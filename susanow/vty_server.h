

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
  int                     server_fd_;
  const std::string       bootmsg_;
  const std::string       prompt_;
  std::vector<vty_client> clients_;
  std::vector<command*>   commands_;
  std::vector<key_func*>  keyfuncs_;
 public:
  void* user_ptr_;

  vty_server(uint32_t addr, uint16_t port,
      const char* bootmsg, const char* prompt);
  virtual ~vty_server();

  void install_keyfunction(key_func* kf);
  void install_command(command* cmd);
  void dispatch();
};



