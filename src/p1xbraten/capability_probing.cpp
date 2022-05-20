#include "game.h"

namespace game {

    void handlecapprobe(const char *msg)
    {
        if(!strcmp(msg, CAP_PROBE_CLIENT_DEMO_UPLOAD)) addmsg(N_P1X_CLIENT_DEMO_UPLOAD_SUPPORTED, "r");
#ifdef ANTICHEAT
        if(!strcmp(msg, CAP_PROBE_ANTICHEAT) && anticheatready()) addmsg(N_P1X_ANTICHEAT_SUPPORTED, "r");
#endif
    }

}
