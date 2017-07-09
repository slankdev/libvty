

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


class shell {
  friend class vty;
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

  shell(int d, const char* bootmsg, const char* prmpt,
      const std::vector<command*>* cmds,
      const std::vector<key_func*>* kfs, void* ptr);
  void process();
  void refresh_prompt();
  void exec_command();
};

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


