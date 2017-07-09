
#include <thread>
#include "ssn_vty.h"
#include <slankdev/util.h>
#include <slankdev/socketfd.h>



int main(int argc, char** argv)
{
  ssn_vty vty(INADDR_ANY, 9999);

  std::thread t(ssn_vty_poll_thread, &vty);
  getchar();
  ssn_vty_poll_thread_stop();
  t.join();
}
