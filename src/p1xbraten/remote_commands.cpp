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
        if(!strncmp(cmd, "do ", strlen("do "))) { runclientscript(sender, &cmd[strlen("do ")]); return true; }
        else if(!strcmp(cmd, "competitive"))    { setupmanagedgame(sender);                   return true; }
        return false;
    }

}
