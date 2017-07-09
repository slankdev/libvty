

#include <unistd.h>
#include <stdint.h>
#include <thread>
#include <ssn_vty.h>

bool running;
void vty_poll(void* arg)
{
  ssn_vty* v = reinterpret_cast<ssn_vty*>(arg);
  running = true;
  while (running) {
    v->poll_dispatch();
  }
  printf("Finish %s\n", __func__);
}


int main(int argc, char** argv)
{
  auto addr = 0x00000000;
  auto port = 9999;
  ssn_vty vty(addr, port);

  printf("Listen %u:%u\n", addr, port);
  std::thread t(vty_poll, &vty);
  getchar();
  running = false;
  t.join();
}



