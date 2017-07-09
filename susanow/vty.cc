
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


