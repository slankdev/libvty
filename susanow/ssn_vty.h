
/*
 * MIT License
 *
 * Copyright (c) 2017 Hiroki SHIROKURA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 * @file   ssn_vty.h
 * @brief  vty implementation
 * @author Hiroki SHIROKURA
 * @date   2017.6.16
 */


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
class shell;
class command {
 public:
  std::vector<node*> nodes;
  virtual ~command() {}
  virtual void func(shell* sh) = 0;
  virtual bool match(const std::string& str);
};

class inputbuffer {
  std::string ibuf;
  size_t cur_idx;
 public:
  inputbuffer() : cur_idx(0) {}
  void input_char(char c) { ibuf.insert(ibuf.begin()+cur_idx, c); cur_idx++; }
  void input_str(const std::string& str) { for (char c : str) input_char(c); }
  void clear()  { ibuf.clear(); cur_idx = 0; }
  size_t length()   const { return ibuf.length(); }
  const char* c_str() const { return ibuf.c_str();  }
  size_t index() const { return cur_idx; }
  bool empty() const { return ibuf.empty(); }
  std::string to_string() const { return ibuf; }

  void cursor_top() { cur_idx = 0; }
  void cursor_end() { cur_idx = ibuf.size(); }
  void cursor_right() { if (cur_idx < ibuf.length()) cur_idx ++ ; }
  void cursor_left() { if (cur_idx > 0) cur_idx -- ; }
  void cursor_backspace()
  {
    if (cur_idx > 0) {
      cur_idx --;
      ibuf.erase(ibuf.begin() + cur_idx);
    }
  }
};

class commandhistory {
  size_t hist_index;
  std::vector<std::string> history;
 public:
  commandhistory() : hist_index(0) {}
  void add(const std::string& str) { history.push_back(str); }
  void clean() { hist_index=0; }
  std::string deep_get();
  std::string shallow_get();
};
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


