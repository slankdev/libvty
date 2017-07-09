

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>

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

