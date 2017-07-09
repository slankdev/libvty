
#pragma once
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <slankdev/exception.h>
#include <vty_client.h>


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

struct KF_return : public key_func {
  KF_return(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh) { sh->exec_command(); }
};
struct KF_backspace : public key_func {
  KF_backspace(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh) { sh->ibuf.cursor_backspace(); }
};
struct KF_right : public key_func {
  KF_right(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh) { sh->ibuf.cursor_right(); }
};
struct KF_left : public key_func {
  KF_left(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh) { sh->ibuf.cursor_left(); }
};
struct KF_hist_search_deep : public key_func {
  KF_hist_search_deep(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh)
  {
    sh->ibuf.clear();
    sh->ibuf.input_str(sh->history.deep_get());
    sh->refresh_prompt();
  }
};
struct KF_hist_search_shallow : public key_func {
  KF_hist_search_shallow(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh)
  {
    sh->ibuf.clear();
    sh->ibuf.input_str(sh->history.shallow_get());
    sh->refresh_prompt();
  }
};
struct KF_cursor_top : public key_func {
  KF_cursor_top(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh) { sh->ibuf.cursor_top(); }
};
struct KF_cursor_end : public key_func {
  KF_cursor_end(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh) { sh->ibuf.cursor_end(); }
};
struct KF_completion : public key_func {
  KF_completion(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh);
};


struct KF_help : public key_func {
  KF_help(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh);
};
