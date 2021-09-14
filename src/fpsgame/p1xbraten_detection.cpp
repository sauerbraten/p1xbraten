#include "game.h"

namespace game {
    #define P1XBRATEN_VERSION_PREFIX (uchar(69))

    MOD(VARP, enablep1xbratendetection, 0, 1, 1);

    void broadcastp1xbratenversion()
    {
        if(!enablep1xbratendetection) return;
        static const semver v(p1xbratenversion);
        addmsg(N_SWITCHMODEL, "ri", P1XBRATEN_VERSION_PREFIX<<24 | int(v.maj) << 16 | int(v.min) << 8 | int(v.patch));
        addmsg(N_SWITCHMODEL, "ri", player1->playermodel);
    }

    bool setp1xbratenversion(fpsent *d, int version)
    {
        if((version>>24)!=P1XBRATEN_VERSION_PREFIX) return false;
        if(!enablep1xbratendetection) return true;
        d->p1xbratenversion = semver((version>>16)&0xF, (version>>8)&0xF, version&0xF);
        return true;
    }
}
