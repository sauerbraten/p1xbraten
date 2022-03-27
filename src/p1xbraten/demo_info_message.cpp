#include "game.h"

bool _demoinfomsg(packetbuf &p, const char *serverhostport, const char *serverdesc)
{
    if(!serverhostport) return false;

    static string serverstr;
    if(serverdesc && serverdesc[0]) formatstring(serverstr, "%s (%s)", serverdesc, serverhostport);
    else copystring(serverstr, serverhostport);

    static string timestr;
    time_t t = time(NULL);
    size_t timestrlen = strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S UTC", gmtime(&t));
    timestr[min(timestrlen, sizeof(timestr)-1)] = '\0';

    putint(p, N_SERVMSG);
    defformatstring(msg, "recorded on %s, starting at %s", serverstr, timestr);
    sendstring(msg, p);
    return true;
}

extern ENetHost *serverhost;

namespace server {
    const char *serverhostport()
    {
        if(!serverhost) return "<unkown>";
        static string hostport;
        if(enet_address_get_host_ip(&serverhost->address, hostport, MAXSTRLEN) || !strcmp(hostport, "0.0.0.0")) copystring(hostport, "<unknown>");
        concformatstring(hostport,":%d", serverhost->address.port);
        return hostport;
    }

    extern char *serverdesc;

    bool demoinfomsg(packetbuf &p) { return _demoinfomsg(p, serverhostport(), serverdesc); }
}

#ifndef STANDALONE
namespace game {
    const char *serverhostport()
    {
        const ENetAddress *addr = connectedpeer();
        if(!remote || !addr) return "localhost";
        static string hostport;
        if (enet_address_get_host_ip(addr, hostport, MAXSTRLEN)) return "<unknown>";
        concformatstring(hostport,":%d", addr->port);
        return hostport;
    }

    bool demoinfomsg(packetbuf &p) { return _demoinfomsg(p, serverhostport(), servinfo); }
}
#endif
