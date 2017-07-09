
#include <vty.h>

#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include <string>
#include <vector>
#include <algorithm>

#include <slankdev/socketfd.h>
#include <slankdev/telnet.h>
#include <slankdev/poll.h>
#include <slankdev/exception.h>
#include <slankdev/string.h>
#include <slankdev/util.h>
#include <slankdev/asciicode.h>

static inline void ssn_sleep(size_t n) { usleep(1000 * n); }

void shell::process()
{
  char str[100];
  ssize_t res = ::read(fd, str, sizeof(str));
  if (res <= 0) throw slankdev::exception("OKASHII");

  press_keys(str, res);
  refresh_prompt();
}

void shell::refresh_prompt()
{
  char lineclear[] = {slankdev::AC_ESC, '[', 2, slankdev::AC_K, '\0'};
  Printf("\r%s", lineclear);
  Printf("\r%s%s", prompt.c_str(), ibuf.c_str());

  size_t backlen = ibuf.length() - ibuf.index();
  char left [] = {slankdev::AC_ESC, '[', slankdev::AC_D, '\0'};
  for (size_t i=0; i<backlen; i++) {
    Printf("%s", left);
  }
}


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
static inline bool endisspace(std::string str)
{
  const char* istr = str.c_str();
  return (istr[strlen(istr)-1] == ' ');
}
inline bool is_prefix(std::string str, std::string pref)
{
  auto ret = str.find(pref);
  if (ret != std::string::npos) {
    if (ret == 0) return true;
  }
  return false;
}

vty::vty(uint16_t p, const char* msg, const char* prmpt)
  : port(p), bootmsg(msg), prompt(prmpt), server_fd(get_server_sock())
{
  add_default_keyfunctions();
}
vty::~vty()
{
  for (command* c : commands) delete c;
  for (key_func* f : keyfuncs) delete f;
}
void vty::install_keyfunction(key_func* kf) { keyfuncs.push_back(kf); }
void vty::install_command(command* cmd) { commands.push_back(cmd); }


namespace ssn_cmd {

struct echo : public command {
  echo()
  {
    nodes.push_back(new node_fixedstring("echo", ""));
    nodes.push_back(new node_string              );
  }
  void func(shell* sh)
  {
    std::string s = nodes[1]->get();
    sh->Printf("%s\n", s.c_str());
  }
};

struct show_author : public command {
  show_author()
  {
    nodes.push_back(new node_fixedstring("show", ""));
    nodes.push_back(new node_fixedstring("author", ""));
  }
  void func(shell* sh)
  {
    sh->Printf("Hiroki SHIROKURA.\r\n");
    sh->Printf(" Twitter : @\r\n");
    sh->Printf(" Github  : \r\n");
    sh->Printf(" Facebook: hiroki.shirokura\r\n");
    sh->Printf(" E-mail  : slank.dev@gmail.com\r\n");
  }
};

struct show_version : public command {
  show_version()
  {
    nodes.push_back(new node_fixedstring("show", ""));
    nodes.push_back(new node_fixedstring("version", ""));
  }
  void func(shell* sh)
  {
    sh->Printf("Susanow 0.0.0\r\n");
    sh->Printf("Copyright 2017-2020 Hiroki SHIROKURA.\r\n");
  }
};

struct quit : public command {
  quit() { nodes.push_back(new node_fixedstring("quit", "")); }
  void func(shell* sh) { sh->close(); }
};

struct clear : public command {
  clear() { nodes.push_back(new node_fixedstring("clear", "")); }
  void func(shell* sh) { sh->Printf("\033[2J\r\n"); }
};

struct list : public command {
  list() { nodes.push_back(new node_fixedstring("list", "")); }
  void func(shell* sh)
  {
    const std::vector<command*>& commands = *sh->commands;
    for (command* cmd : commands) {
      std::string s = "";
      for (node* nd : cmd->nodes) {
        s += nd->to_string();
        s += " ";
      }
      sh->Printf("  %s\r\n", s.c_str());
    }
  }
};

} /* namespace ssn_cmd */


bool command::match(const std::string& str)
{
  std::vector<std::string> list = slankdev::split(str, ' ');
  if (list.size() != nodes.size()) {
    return false;
  }

  for (size_t i=0; i<list.size(); i++) {
    if (nodes[i]->match(list[i]) == false) {
      return false;
    }
  }
  return true;
}

void shell::press_keys(const void* d, size_t l)
{
  const uint8_t* p = reinterpret_cast<const uint8_t*>(d);
  if (l == 0) throw slankdev::exception("empty data received");

  for (key_func* kf : *keyfuncs) {
    if (kf->match(p, l)) {
      kf->function(this);
      return ;
    }
  }

  if (l > 1) {
    return ;
  }
  ibuf.input_char(p[0]);
}


shell::shell(
    int d,
    const char* bootmsg,
    const char* prmpt,
    const std::vector<command*>* cmds,
    const std::vector<key_func*>* kfs,
    void* ptr
    ) :
  prompt(prmpt),
  fd(d),
  closed(false),
  commands(cmds),
  keyfuncs(kfs),
  user_ptr(ptr)
{
  Printf(bootmsg);
  refresh_prompt();
}


void shell::exec_command()
{
  Printf("\r\n");
  if (!ibuf.empty()) {
    history.add(ibuf.to_string());
    for (size_t i=0; i<commands->size(); i++) {
      if (commands->at(i)->match(ibuf.c_str())) {
        commands->at(i)->func(this);
        ibuf.clear();
        history.clean();
        Printf("\r%s%s", prompt.c_str(), ibuf.c_str());
        refresh_prompt();
        return ;
      }
    }
    Printf("command not found: \"%s\"\r\n", ibuf.c_str());
  }
  history.clean();
  ibuf.clear();
  Printf("\r%s%s", prompt.c_str(), ibuf.c_str());
  refresh_prompt();
}



void KF_help::function(shell* sh)
{
  const std::vector<command*>& commands = *sh->commands;
  std::vector<std::string> list = slankdev::split(sh->ibuf.c_str(), ' ');
  if (endisspace(sh->ibuf.c_str()) || list.empty()) list.push_back("");

  sh->Printf("\r\n");
  std::vector<std::string> match;
  for (command* cmd : commands) {

    for (size_t i=0; i<list.size(); i++) {

      if (i == cmd->nodes.size()) {

        if (list[i] == "") {
          sh->Printf("  <CR>\r\n");
        } else {
          sh->Printf("  %% There is no matched command.\r\n");
        }
        return ;

      } else {

        if (!cmd->nodes[i]->match_prefix(list[i])) {
          break;
        } else {
          if (i+1==list.size()) {
            std::string s = slankdev::fs("%-10s:   %-20s",
                cmd->nodes[i]->to_string().c_str(),
                cmd->nodes[i]->msg().c_str()
                );
            match.push_back(s);
          } else {
            ;
          }
        }

      }

    }

  }

  std::sort(match.begin(), match.end());
  match.erase(std::unique(match.begin(), match.end()), match.end());
  for (std::string& s : match) {
    sh->Printf("  %s \r\n", s.c_str());
  }

}


void KF_completion::function(shell* sh)
{
  const std::vector<command*>& commands = *sh->commands;
  std::vector<std::string> list = slankdev::split(sh->ibuf.c_str(), ' ');
  if (endisspace(sh->ibuf.c_str()) || list.empty()) list.push_back("");

  sh->Printf("\r\n");
  std::vector<std::string> match;
  for (command* cmd : commands) {

    for (size_t i=0; i<list.size(); i++) {

      if (i == cmd->nodes.size()) {

        if (list[i] == "") {
          sh->Printf("  <CR>\r\n");
        } else {
          sh->Printf("  %% There is no matched command.\r\n");
        }
        return ;

      } else {

        if (!cmd->nodes[i]->match_prefix(list[i])) {
          break;
        } else {
          if (i+1==list.size()) {
            std::string s = cmd->nodes[i]->to_string();
            match.push_back(s);
          } else {
            ;
          }
        }

      }

    }

  }

  std::sort(match.begin(), match.end());
  match.erase(std::unique(match.begin(), match.end()), match.end());

  if (match.size() == 1) {
    std::string s;
    for (size_t i=0; i<list.size(); i++) {
      if (i == list.size()-1) {
        s += match[0];
      } else {
        s += list[i];
      }
      s += " ";
    }
    sh->ibuf.clear();
    sh->ibuf.input_str(s);
  } else {
    for (std::string& s : match) {
      sh->Printf("  %s ", s.c_str());
    }
    sh->Printf("\r\n");
  }
}




int vty::get_server_sock()
{
  slankdev::socketfd server_sock;
  server_sock.noclose_in_destruct = true;
  server_sock.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  int option = 1;
  server_sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  server_sock.bind((sockaddr*)&addr, sizeof(addr));
  server_sock.listen(5);
  return server_sock.get_fd();
}

void vty::dispatch()
{
  struct Pollfd : public pollfd {
    Pollfd(int ifd, short ievents)
    {
      fd = ifd;
      events = ievents;
    }
  };
  std::vector<struct Pollfd> fds;
  fds.push_back(Pollfd(server_fd, POLLIN));
  for (const shell& sh : shells) fds.emplace_back(Pollfd(sh.fd, POLLIN));

  if (slankdev::poll(fds.data(), fds.size(), 1000)) {
    if (fds[0].revents & POLLIN) {
      /*
       * Server Accept Process
       */
      struct sockaddr_in client;
      socklen_t client_len = sizeof(client);
      int fd = accept(fds[0].fd, (sockaddr*)&client, &client_len);

      slankdev::socketfd client_sock(fd);
      uint32_t on = 1;
      client_sock.setsockopt(IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
      int flags = client_sock.fcntl(F_GETFL);
      client_sock.fcntl(F_SETFL, (flags | O_NONBLOCK));
      client_sock.noclose_in_destruct = true;
      slankdev::vty_will_echo (fd);
      slankdev::vty_will_suppress_go_ahead (fd);
      slankdev::vty_dont_linemode (fd);
      slankdev::vty_do_window_size (fd);

      shells.push_back(
          shell(
            fd,
            bootmsg.c_str(),
            prompt.c_str(),
            &commands,
            &keyfuncs,
            user_ptr
            )
          );
    }

    /*
     * Client Read Process
     */
    for (size_t i=1; i<fds.size(); i++) {
      if (fds[i].revents & POLLIN) {
        shells[i-1].process();
        if (shells[i-1].closed) {
          close(fds[i].fd);
          shells.erase(shells.begin() + i);
          continue;
        }
      }
    }
  }
}

void vty::add_default_keyfunctions()
{
  using namespace slankdev;

  uint8_t up   [] = {AC_ESC, '[', AC_A};
  uint8_t down [] = {AC_ESC, '[', AC_B};
  uint8_t right[] = {AC_ESC, '[', AC_C};
  uint8_t left [] = {AC_ESC, '[', AC_D};
  install_keyfunction(new KF_hist_search_deep   (up   , sizeof(up   )));
  install_keyfunction(new KF_hist_search_shallow(down , sizeof(down )));
  install_keyfunction(new KF_right(right, sizeof(right)));
  install_keyfunction(new KF_left (left , sizeof(left )));

  uint8_t ctrlP[] = {AC_Ctrl_P};
  uint8_t ctrlN[] = {AC_Ctrl_N};
  uint8_t ctrlF[] = {AC_Ctrl_F};
  uint8_t ctrlB[] = {AC_Ctrl_B};
  install_keyfunction(new KF_hist_search_deep   (ctrlP, sizeof(ctrlP)));
  install_keyfunction(new KF_hist_search_shallow(ctrlN, sizeof(ctrlN)));
  install_keyfunction(new KF_right(ctrlF, sizeof(ctrlF)));
  install_keyfunction(new KF_left (ctrlB, sizeof(ctrlB)));

  uint8_t ctrlA[] = {AC_Ctrl_A};
  uint8_t ctrlE[] = {AC_Ctrl_E};
  install_keyfunction(new KF_cursor_top(ctrlA, sizeof(ctrlA)));
  install_keyfunction(new KF_cursor_end(ctrlE, sizeof(ctrlE)));

  uint8_t question[] = {'?'};
  install_keyfunction(new KF_help(question, sizeof(question)));

  uint8_t tab[] = {'\t'};
  install_keyfunction(new KF_completion(tab, sizeof(tab)));

  uint8_t ret[] = {'\r', '\0'};
  install_keyfunction(new KF_return  (ret, sizeof(ret)));
  uint8_t CtrlJ[] = {AC_Ctrl_J};
  install_keyfunction(new KF_return  (CtrlJ, sizeof(CtrlJ)));

  uint8_t backspace[] = {0x7f};
  install_keyfunction(new KF_backspace  (backspace, sizeof(backspace)));
}



struct slank : public command {
  slank() { nodes.push_back(new node_fixedstring("slank", "")); }
  void func(shell* sh)
  {
    sh->Printf("slankdev\r\n");
  }
};

ssn_vty::ssn_vty(uint32_t addr, uint16_t port)
{
  char str[] = "\r\n"
      "Hello, this is Susanow (version 0.00.00.0).\r\n"
      "Copyright 2017-2020 Hiroki SHIROKURA.\r\n"
      "\r\n"
      " .d8888b.                                                             \r\n"
      "d88P  Y88b                                                            \r\n"
      "Y88b.                                                                 \r\n"
      " \"Y888b.   888  888 .d8888b   8888b.  88888b.   .d88b.  888  888  888 \r\n"
      "    \"Y88b. 888  888 88K          \"88b 888 \"88b d88\"\"88b 888  888  888 \r\n"
      "      \"888 888  888 \"Y8888b. .d888888 888  888 888  888 888  888  888 \r\n"
      "Y88b  d88P Y88b 888      X88 888  888 888  888 Y88..88P Y88b 888 d88P \r\n"
      " \"Y8888P\"   \"Y88888  88888P\' \"Y888888 888  888  \"Y88P\"   \"Y8888888P\"  \r\n"
      "\r\n";
  v = new vty(port, str, "ssn> ");
}
ssn_vty::~ssn_vty() { delete v; }


bool ssn_vty_poll_thread_running;
void ssn_vty_poll_thread(void* arg)
{
  ssn_vty* Vty = reinterpret_cast<ssn_vty*>(arg);
  Vty->v->install_command(new slank);
  Vty->v->install_command(new ssn_cmd::quit        );
  Vty->v->install_command(new ssn_cmd::clear       );
  Vty->v->install_command(new ssn_cmd::echo        );
  Vty->v->install_command(new ssn_cmd::list        );
  // Vty->v->install_command(new ssn_cmd::show_cpu    );
  Vty->v->install_command(new ssn_cmd::show_author );
  Vty->v->install_command(new ssn_cmd::show_version);

  ssn_vty_poll_thread_running = true;
  while (ssn_vty_poll_thread_running) {
    Vty->v->dispatch();
    ssn_sleep(1);
  }
}
void ssn_vty_poll_thread_stop() { ssn_vty_poll_thread_running = false; }


