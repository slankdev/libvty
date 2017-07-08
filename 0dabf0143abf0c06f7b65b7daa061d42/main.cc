

#include "VTY.h"
#include <slankdev/socketfd.h>
#include <slankdev/hexdump.h>
#include <slankdev/exception.h>
#include <poll.h>
#include <vector>
#include <string>
#include <map>
#include <thread>

class ssn_vty_client {
  slankdev::socketfd sock;
 public:
  ssn_vty_client(int fd) : sock(fd) {}
  bool operator<(const ssn_vty_client& rhs) const  { return sock.get_fd() <  rhs.sock.get_fd(); }
  bool operator>(const ssn_vty_client& rhs) const  { return sock.get_fd() >  rhs.sock.get_fd(); }
  bool operator==(const ssn_vty_client& rhs) const { return sock.get_fd() == rhs.sock.get_fd(); }
  bool operator!=(const ssn_vty_client& rhs) const { return !(*this==rhs); }

};

class ssn_vty {
  std::vector<struct pollfd> fds;
  std::vector<ssn_vty_client> clients;
 public:
  ssn_vty(uint32_t addr, uint16_t port)
  {
    slankdev::socketfd sock;
    sock.noclose_in_destruct = true;

    int yes = 1;
    sock.socket(AF_INET, SOCK_STREAM, 0);
    sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));
    sock.bind(addr, port);
    sock.listen(5);

    struct pollfd pfd;
    pfd.fd = sock.get_fd();
    pfd.events = POLLIN | POLLERR;
    fds.push_back(pfd);
  }
  void dispatch();
};

void ssn_vty::dispatch()
{
  int ret = poll(fds.data(), fds.size(), -1);
  if (ret < 0) throw slankdev::exception("poll");
  // printf("EVENT!!!\n");

  for (size_t i=0; i<fds.size(); i++) {
    if (fds[i].revents & POLLIN) {
      if (i == 0) {

        /*
         * Server Socket
         */
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);
        int fd = accept(fds[i].fd, (sockaddr*)&client, &client_len);
        if (fd < 0) throw slankdev::exception("accept");
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN | POLLERR;
        fds.push_back(pfd);
        clients.emplace_back(fd);
        printf("new client nb_clients=%zd\n", clients.size());
        break;

      } else {

        /*
         * Client Socket
         */
        printf("CLIENT EVENT!!!\n");
        int fd = fds[i].fd;

        uint8_t buf[1000];
        ssize_t recvlen = read(fd, buf, sizeof(buf));
        if (recvlen < 0) throw slankdev::exception("read");

        slankdev::hexdump(stdout, buf, recvlen);
        break;

      }
    }
  } // for
}


bool running = true;
void LOOP(void* arg)
{
  ssn_vty* vty = reinterpret_cast<ssn_vty*>(arg);
  running = true;
  while (running) {
    vty->dispatch();
  }
  printf("exit LOOP\n");
}


int main(int argc, char** argv)
{
  ssn_vty vty(INADDR_ANY, 9999);
  std::thread t(LOOP, &vty);
  getchar();
  running = false;
  t.join();
}



