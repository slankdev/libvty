

#include <unistd.h>
#include <stdint.h>
#include <thread>
#include <ssn_vty.h>


int main(int argc, char** argv)
{
  auto addr = 0x00000000;
  auto port = 9999;
  ssn_vty vty(addr, port);

  printf("Listen %u:%u\n", addr, port);
  std::thread t(vty_poll, &vty);
  getchar();
  vty_poll_stop();
  t.join();
}



