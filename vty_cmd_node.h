
#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <stddef.h>
#include <slankdev/exception.h>

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
  std::string to_string() { return str; }
  const std::string& msg() { return msg_; }
  bool match_prefix(const std::string& s)
  {
    auto ret = str.find(s);
    if (ret != std::string::npos) {
      if (ret == 0) return true;
    }
    return false;
  }
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

