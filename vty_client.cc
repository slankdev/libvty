
#include <vty_client.h>
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <vty_keyfunction.h>
#include <vty_cmd.h>
#include <unistd.h>
#include <slankdev/asciicode.h>


int vty_client::get_fd() const { return client_fd_; }

vty_client::vty_client(
    int d,
    const char* bootmsg,
    const char* prmpt,
    const std::vector<vty_cmd*>* cmds,
    const std::vector<key_func*>* kfs,
    void* ptr
    ) :
  prompt_(prmpt),
  client_fd_(d),
  closed_(false),
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
      if (commands->at(i)->match.is_match(ibuf.to_string().c_str())) {
        commands->at(i)->f(&commands->at(i)->match, this, commands->at(i)->arg);
        ibuf.clear();
        history.clean();
        Printf("\r%s%s", prompt_.c_str(), ibuf.to_string().c_str());
        refresh_prompt();
        return ;
      }
    }
    Printf("command not found: \"%s\"\r\n", ibuf.to_string().c_str());
  }
  history.clean();
  ibuf.clear();
  refresh_prompt();
}

void vty_client::poll_dispatch()
{
  char str[100];
  ssize_t res = ::read(client_fd_, str, sizeof(str));
  if (res <= 0) throw slankdev::exception("OKASHII");

  {
    const void* d = str;
    size_t l = res;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(d);
    if (l == 0) throw slankdev::exception("empty data received");
    for (key_func* kf : *keyfuncs) {
      if (kf->match(p, l)) {
        kf->function(this);
        goto ret ;
      }
    }
    if (l > 1) goto ret ;
    ibuf.input_char(p[0]);
  }
ret:
  refresh_prompt();
}

void vty_client::close() { closed_ = true; }
void vty_client::refresh_prompt()
{
  char lineclear[] = {slankdev::AC_ESC, '[', 2, slankdev::AC_K, '\0'};
  Printf("\r%s", lineclear);
  Printf("\r%s%s", prompt_.c_str(), ibuf.to_string().c_str());

  size_t backlen = ibuf.length() - ibuf.index();
  char left [] = {slankdev::AC_ESC, '[', slankdev::AC_D, '\0'};
  for (size_t i=0; i<backlen; i++) {
    Printf("%s", left);
  }
}

void vty_client::Printf(const char* fmt, ...)
{
  FILE* fp = fdopen(client_fd_, "w");
  va_list args;
  va_start(args, fmt);
  vfprintf(fp, fmt, args);
  va_end(args);
  fflush(fp);
}



