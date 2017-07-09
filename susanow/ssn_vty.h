

#pragma once
#include <stdint.h>
#include <string.h>
#include <string>
#include <slankdev/util.h>
#include <slankdev/exception.h>

class shell;

class key_func {
 public:
  uint8_t code[256];
  const size_t  len;
  key_func(const void* d, size_t l) : len(l)
  {
    if (sizeof(code) < l) throw slankdev::exception("length is too big");
    memcpy(code, d, l);
  }
  virtual ~key_func() {}
  virtual void function(shell*) = 0;
  virtual bool match(const void* d, size_t l)
  {
    return (len == l) && (memcmp(code, d, l) == 0);
  }
};


class node {
 public:
  virtual bool match(const std::string& str) = 0;
  virtual bool match_prefix(const std::string& str) = 0;
  virtual std::string& get() { throw slankdev::exception("FAAA"); }
  virtual std::string to_string() = 0;
  virtual const std::string& msg() = 0;
};


class node_fixedstring : public node {
  std::string str;
  const std::string msg_;
 public:
  node_fixedstring(const std::string& s, const std::string m)
    : str(s), msg_(m) {}
  bool match(const std::string& s) { return str == s; }
  bool match_prefix(const std::string& s);
  std::string to_string() { return str; }
  const std::string& msg() { return msg_; }
};

class node_string : public node {
  std::string str;
  const std::string msg_;
 public:
  node_string() : msg_("string") {}
  bool match(const std::string& s)
  {
    if (s.length() > 0) {
      str = s;
      return true;
    }
    return false;
  }
  bool match_prefix(const std::string&)
  { return true; }
  std::string& get() { return str; }
  std::string to_string() { return "<string>"; }
  const std::string& msg() { return msg_; }
};

class command {
 public:
  std::vector<node*> nodes;
  virtual ~command() {}
  virtual void func(shell* sh) = 0;
  virtual bool match(const std::string& str);
};


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


