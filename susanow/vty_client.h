


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
  std::string prompt;
  int   fd;
  bool  closed;

  void press_keys(const void* d, size_t l);
 public:
  commandhistory history;
  inputbuffer    ibuf;
  const std::vector<command*> * commands;
  const std::vector<key_func*>* keyfuncs;
  void* user_ptr;

  void close() { closed = true; }

  void Printf(const char* fmt, ...)
  {
    FILE* fp = fdopen(fd, "w");
    va_list args;
    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    va_end(args);
    fflush(fp);
  }

  int get_fd() const { return fd; }

  vty_client(int d, const char* bootmsg, const char* prmpt,
      const std::vector<command*>* cmds,
      const std::vector<key_func*>* kfs, void* ptr);
  void process();
  void refresh_prompt();
  void exec_command();
};


