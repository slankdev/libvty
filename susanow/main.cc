

#include <thread>
#include <ssn_vty.h>

int main(int argc, char** argv)
{
  ssn_vty vty(0x00000000, 9999);

  std::thread t(ssn_vty_poll_thread, &vty);
  getchar();
  ssn_vty_poll_thread_stop();
  t.join();
}

void Slankdev(void*) {}

int new_main(int argc, char** argv)
{
  ssn_vty vty(0x0, 9999);
  vty.new_install_command("", Slankdev, nullptr);
}
