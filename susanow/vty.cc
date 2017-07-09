
#include <vty.h>


/*
 *======================================
 */

struct echo : public command {
  echo()
  {
    nodes.push_back(new node_fixedstring("echo", ""));
    nodes.push_back(new node_string              );
  }
  void func(vty_client* sh)
  {
    std::string s = nodes[1]->get();
    sh->Printf("%s\n", s.c_str());
  }
};

struct show_author : public command {
  show_author()
  {
    nodes.push_back(new node_fixedstring("show", ""));
    nodes.push_back(new node_fixedstring("author", ""));
  }
  void func(vty_client* sh)
  {
    sh->Printf("Hiroki SHIROKURA.\r\n");
    sh->Printf(" Twitter : @\r\n");
    sh->Printf(" Github  : \r\n");
    sh->Printf(" Facebook: hiroki.shirokura\r\n");
    sh->Printf(" E-mail  : slank.dev@gmail.com\r\n");
  }
};

struct show_version : public command {
  show_version()
  {
    nodes.push_back(new node_fixedstring("show", ""));
    nodes.push_back(new node_fixedstring("version", ""));
  }
  void func(vty_client* sh)
  {
    sh->Printf("Susanow 0.0.0\r\n");
    sh->Printf("Copyright 2017-2020 Hiroki SHIROKURA.\r\n");
  }
};

struct quit : public command {
  quit() { nodes.push_back(new node_fixedstring("quit", "")); }
  void func(vty_client* sh) { sh->close(); }
};

struct clear : public command {
  clear() { nodes.push_back(new node_fixedstring("clear", "")); }
  void func(vty_client* sh) { sh->Printf("\033[2J\r\n"); }
};

struct list : public command {
  list() { nodes.push_back(new node_fixedstring("list", "")); }
  void func(vty_client* sh)
  {
    const std::vector<command*>& commands = *sh->commands;
    for (command* cmd : commands) {
      std::string s = "";
      for (node* nd : cmd->nodes) {
        s += nd->to_string();
        s += " ";
      }
      sh->Printf("  %s\r\n", s.c_str());
    }
  }
};

struct slank : public command {
  slank() { nodes.push_back(new node_fixedstring("slank", "")); }
  void func(vty_client* sh)
  {
    sh->Printf("slankdev\r\n");
  }
};

/*
 *======================================
 */


ssn_vty::ssn_vty(uint32_t addr, uint16_t port)
{
  char str[] = "\r\n"
      "Hello, this is Susanow (version 0.00.00.0).\r\n"
      "Copyright 2017-2020 Hiroki SHIROKURA.\r\n"
      "\r\n"
      " .d8888b.                                                             \r\n"
      "d88P  Y88b                                                            \r\n"
      "Y88b.                                                                 \r\n"
      " \"Y888b.   888  888 .d8888b   8888b.  88888b.   .d88b.  888  888  888 \r\n"
      "    \"Y88b. 888  888 88K          \"88b 888 \"88b d88\"\"88b 888  888  888 \r\n"
      "      \"888 888  888 \"Y8888b. .d888888 888  888 888  888 888  888  888 \r\n"
      "Y88b  d88P Y88b 888      X88 888  888 888  888 Y88..88P Y88b 888 d88P \r\n"
      " \"Y8888P\"   \"Y88888  88888P\' \"Y888888 888  888  \"Y88P\"   \"Y8888888P\"  \r\n"
      "\r\n";
  v = new vty_server(port, str, "ssn> ");
}
ssn_vty::~ssn_vty() { delete v; }

static inline void ssn_sleep(size_t n) { usleep(1000 * n); }

bool ssn_vty_poll_thread_running;
void ssn_vty_poll_thread(void* arg)
{
  ssn_vty* Vty = reinterpret_cast<ssn_vty*>(arg);
  Vty->v->install_command(new slank);
  Vty->v->install_command(new quit        );
  Vty->v->install_command(new clear       );
  Vty->v->install_command(new echo        );
  Vty->v->install_command(new list        );
  Vty->v->install_command(new show_author );
  Vty->v->install_command(new show_version);

  ssn_vty_poll_thread_running = true;
  while (ssn_vty_poll_thread_running) {
    Vty->v->dispatch();
    ssn_sleep(1);
  }
}
void ssn_vty_poll_thread_stop() { ssn_vty_poll_thread_running = false; }
