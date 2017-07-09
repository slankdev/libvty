

#include <thread>
#include "vty.h"

int main(int argc, char** argv)
{
  ssn_vty vty(0x00000000, 9999);

  std::thread t(ssn_vty_poll_thread, &vty);
  getchar();
  ssn_vty_poll_thread_stop();
  t.join();
}

