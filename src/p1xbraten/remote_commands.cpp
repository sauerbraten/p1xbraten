#include "game.h"

namespace server {

    void runclientscript(clientinfo *sender, const char *script)
    {
        if(sender->privilege<PRIV_ADMIN) return;
        conoutf("running script '%s' from %s (cn %d)", script, sender->name, sender->clientnum);
        char *result = executestr(script);
        defformatstring(msg, "'%s' => %s", script, result ? result : "<null>");
        sendf(sender->clientnum, 1, "ris", N_SERVMSG, msg);
    }

    bool handleservcmd(clientinfo *sender, const char *cmd)
    {
        string sval; int ival;
        if(!strncmp(cmd, "do ", strlen("do ")))            { runclientscript(sender, &cmd[strlen("do ")]); return true; }
        else if(!strcmp(cmd, "competitive"))               { setupmanagedgame(sender);                     return true; }
        else if(sscanf(cmd, "competitive %5s", sval) == 1) { setupmanagedgame(sender, sval);               return true; }
        else if(sscanf(cmd, "getip %d", &ival) == 1)          { getip(sender, ival);                      return true; }
        else if(sscanf(cmd, "ban %15s %d", sval, &ival) == 2) { addban(sender, sval, ival);                return true; }
        else if(sscanf(cmd, "ban %15s", sval) == 1)           { addban(sender, sval);                      return true; }
        else if(sscanf(cmd, "unban %15s", sval) == 1)         { delban(sender, sval);                      return true; }
        else if(!strcmp(cmd, "listbans"))                     { listbans(sender);                          return true; }
        return false;
    }

}
