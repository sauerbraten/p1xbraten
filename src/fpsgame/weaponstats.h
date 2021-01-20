#ifndef __WEAPONSTATS_H__
#define __WEAPONSTATS_H__

#include "game.h"

namespace game {
    extern void recordpotentialdamage(fpsent *shooter);
    extern bool shouldcountdamage(fpsent *attacker, fpsent *target);
    extern void recorddamage(fpsent *attacker, fpsent *target, int damage);
    extern int playerdamage(fpsent *p, int typ, int gun = -1);
    extern int playerdamagewasted(fpsent *p, int gun = -1);
    extern int playernetdamage(fpsent *p, int gun = -1);
    extern float playeraccuracy(fpsent *p, int gun = -1);

    #define PLAYER_VARGS_ICOMMAND(name,b) ICOMMAND(name, "V", (tagval *args, int numargs), { \
        fpsent *p = numargs > 0 ? getclient(args[0].getint()) : hudplayer(); \
        if(!p) { conoutf("\f6no player with CN %d", args[0].getint()); return; } \
        b; \
    })

    #define DAMAGECOMMAND(name,b) PLAYER_VARGS_ICOMMAND(name, { \
        int gun = numargs > 1 ? args[1].getint() : -1; \
        b;\
    })
}

#endif
