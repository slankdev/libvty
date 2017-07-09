
#include <vty_server.h>

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

static int Get_server_sock(uint32_t addr, uint16_t port)
{
  slankdev::socketfd server_sock;
  server_sock.noclose_in_destruct = true;
  server_sock.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  int option = 1;
  server_sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  server_sock.bind(addr, port);
  server_sock.listen(5);
  return server_sock.get_fd();
}

vty_server::vty_server(uint32_t addr, uint16_t port,
                const char* bootmsg, const char* prompt)
  : bootmsg_(bootmsg), prompt_(prompt)
{
  server_fd_ = Get_server_sock(addr, port);

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
vty_server::~vty_server()
{
  for (command* c : commands_) delete c;
  for (key_func* f : keyfuncs_) delete f;
}
void vty_server::install_keyfunction(key_func* kf) { keyfuncs_.push_back(kf); }
void vty_server::install_command(command* cmd) { commands_.push_back(cmd); }

void vty_server::poll_dispatch()
{
  struct Pollfd : public pollfd {
    Pollfd(int f, short e) { fd = f; events = e; }
  };

  std::vector<struct Pollfd> fds;
  fds.push_back(Pollfd(server_fd_, POLLIN));

  for (const vty_client& sh : clients_) fds.push_back(Pollfd(sh.client_fd_, POLLIN));

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

      clients_.push_back(
          vty_client(
            fd,
            bootmsg_.c_str(),
            prompt_.c_str(),
            &commands_,
            &keyfuncs_,
            user_ptr_
          )
      );
    }

    /*
     * Client Read Process
     */
    for (size_t i=1; i<fds.size(); i++) {
      if (fds[i].revents & POLLIN) {
        clients_[i-1].poll_dispatch();
        if (clients_[i-1].closed_) {
          close(fds[i].fd);
          clients_.erase(clients_.begin() + i);
          continue;
        }
      }
    } // for
  }
}


