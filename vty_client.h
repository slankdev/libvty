


#pragma once

#include <string>
#include <vector>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#include <vty_cmd_history.h>
#include <vty_input_buffer.h>


class key_func;
class vty_cmd;

class vty_client {
  friend class vty_server;
  std::string prompt_;
  int client_fd_;
  bool closed_;

 public:
  commandhistory history;
  vty_input_buffer  ibuf;
  const std::vector<vty_cmd*> * commands;
  const std::vector<key_func*>* keyfuncs;
  void* user_ptr;

  vty_client(int d,
      const char* bootmsg, const char* prmpt,
      const std::vector<vty_cmd*>* cmds,
      const std::vector<key_func*>* kfs,
      void* ptr);
  void poll_dispatch();
  void close();
  void Printf(const char* fmt, ...);
  void refresh_prompt();
  void exec_command();
  int  get_fd() const;
};


