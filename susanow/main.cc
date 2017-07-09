

#include <stdint.h>
#include <thread>
#include <ssn_vty.h>

void Slankdev(void*) {}

int main(int argc, char** argv)
{
  auto addr = 0x00000000;
  auto port = 9999;
  ssn_vty vty(addr, port);
  vty.new_install_command("", Slankdev, nullptr);

  printf("Listen %u:%u\n", addr, port);
  std::thread t(ssn_vty_poll_thread, &vty);
  getchar();
  ssn_vty_poll_thread_stop();
  t.join();
}



