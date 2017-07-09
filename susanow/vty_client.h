


#pragma once

#include <string>
#include <vector>
#include <stdarg.h>
#include <vty_cmd_history.h>
#include <vty_input_buffer.h>

class key_func;
class command;

class vty_client {
  friend class vty_server;

  std::string prompt_   ;
  int         client_fd_;
  bool        closed_   ;

  void press_keys(const void* d, size_t l);
 public:
  commandhistory history;
  inputbuffer    ibuf;
  const std::vector<command*> * commands;
  const std::vector<key_func*>* keyfuncs;
  void* user_ptr;

  void close() { closed_ = true; }
  void Printf(const char* fmt, ...);

  int get_fd() const { return client_fd_; }
  vty_client(int d, const char* bootmsg, const char* prmpt,
      const std::vector<command*>* cmds,
      const std::vector<key_func*>* kfs, void* ptr);
  void poll_dispatch();
  void refresh_prompt();
  void exec_command();
};


