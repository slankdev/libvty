
#include <vty_keyfunction.h>
#include <vty_cmd.h>
#include <vty_client.h>
#include <slankdev/string.h>
#include <algorithm>

static inline bool endisspace(std::string str)
{
  const char* istr = str.c_str();
  return (istr[strlen(istr)-1] == ' ');
}


void KF_help::function(vty_client* sh)
{
  const std::vector<vty_cmd*>& commands = *sh->commands;
  std::vector<std::string> list = slankdev::split(sh->ibuf.to_string().c_str(), ' ');
  if (endisspace(sh->ibuf.to_string().c_str()) || list.empty()) list.push_back("");

  sh->Printf("\r\n");
  std::vector<std::string> match;
  for (vty_cmd* cmd : commands) {

    for (size_t i=0; i<list.size(); i++) {

      if (i == cmd->match_.nodes.size()) {

        if (list[i] == "") {
          sh->Printf("  <CR>\r\n");
        } else {
          sh->Printf("  %% There is no matched command.\r\n");
        }
        return ;

      } else {

        if (!cmd->match_.nodes[i]->match_prefix(list[i])) {
          break;
        } else {
          if (i+1==list.size()) {
            std::string s = slankdev::fs("%-10s:   %-20s",
                cmd->match_.nodes[i]->to_string().c_str(),
                cmd->match_.nodes[i]->msg().c_str()
                );
            match.push_back(s);
          } else {
            ;
          }
        }

      }

    }

  }

  std::sort(match.begin(), match.end());
  match.erase(std::unique(match.begin(), match.end()), match.end());
  for (std::string& s : match) {
    sh->Printf("  %s \r\n", s.c_str());
  }

}


void KF_completion::function(vty_client* sh)
{
  const std::vector<vty_cmd*>& commands = *sh->commands;
  std::vector<std::string> list = slankdev::split(sh->ibuf.to_string().c_str(), ' ');
  if (endisspace(sh->ibuf.to_string().c_str()) || list.empty()) list.push_back("");

  sh->Printf("\r\n");
  std::vector<std::string> match;
  for (vty_cmd* cmd : commands) {

    for (size_t i=0; i<list.size(); i++) {

      if (i == cmd->match_.nodes.size()) {

        if (list[i] == "") {
          sh->Printf("  <CR>\r\n");
        } else {
          sh->Printf("  %% There is no matched command.\r\n");
        }
        return ;

      } else {

        if (!cmd->match_.nodes[i]->match_prefix(list[i])) {
          break;
        } else {
          if (i+1==list.size()) {
            std::string s = cmd->match_.nodes[i]->to_string();
            match.push_back(s);
          } else {
            ;
          }
        }

      }

    }

  }

  std::sort(match.begin(), match.end());
  match.erase(std::unique(match.begin(), match.end()), match.end());

  if (match.size() == 1) {
    std::string s;
    for (size_t i=0; i<list.size(); i++) {
      if (i == list.size()-1) {
        s += match[0];
      } else {
        s += list[i];
      }
      s += " ";
    }
    sh->ibuf.clear();
    sh->ibuf.input_str(s);
  } else {
    for (std::string& s : match) {
      sh->Printf("  %s ", s.c_str());
    }
    sh->Printf("\r\n");
  }
}


