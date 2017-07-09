
#include <vty_client.h>
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <vty_keyfunction.h>
#include <vty_cmd.h>
#include <unistd.h>
#include <slankdev/asciicode.h>

void vty_client::press_keys(const void* d, size_t l)
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


vty_client::vty_client(
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


void vty_client::exec_command()
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

void vty_client::process()
{
  char str[100];
  ssize_t res = ::read(fd, str, sizeof(str));
  if (res <= 0) throw slankdev::exception("OKASHII");

  press_keys(str, res);
  refresh_prompt();
}

void vty_client::refresh_prompt()
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



