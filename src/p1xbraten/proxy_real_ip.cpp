#include "game.h"

namespace server {

    vector<char *> proxyips;
    ICOMMAND(addtrustedproxyip, "s", (char *domain), proxyips.add(newstring(domain)));

    bool hastrustedproxyip(clientinfo *ci) // false after setip!
    {
        const char *ip = getclienthostname(ci->clientnum);
        loopv(proxyips) if(!strcmp(ip, proxyips[i])) return true;
        return false;
    }

    void setip(clientinfo *sender, uint realip)
    {
        if(!sender || !hastrustedproxyip(sender)) return;
        if(setclientrealip(sender->clientnum, realip))
            logoutf("failed setting real IP of %s (cn: %d) to %u (sent by proxy)", sender->name, sender->clientnum, realip);
    }

}
