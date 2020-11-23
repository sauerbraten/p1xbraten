#include "game.h"

namespace game {
    void recordshot(fpsent *shooter)
    {
        if(!shooter) return;
        int gun = shooter->gunselect;
        int potentialdamage = guns[gun].damage * (shooter->quadmillis ? 4 : 1 ) * guns[gun].rays;
        shooter->stats.add(DMG_POTENTIAL, gun, potentialdamage);
    }

    bool shouldcounthit(fpsent *attacker, fpsent *target)
    {
        return (attacker != target) && !isteam(attacker->team, target->team);
    }

    void recordhit(fpsent *attacker, fpsent *target, int damage)
    {
        if(!shouldcounthit(attacker, target)) return;
        int gun = attacker->gunselect;
        // try to fix gun used to deal the damage
        if(gun == GUN_RL || gun == GUN_GL || damage != guns[gun].damage * (attacker->quadmillis ? 4 : 1))
        {
            gun = attacker->stats.lastprojectile;
        }
        attacker->stats.add(DMG_DEALT, gun, damage);
        target->stats.add(DMG_RECEIVED, gun, damage);
    }

    #define DMG_ICOMMAND(name,ret) ICOMMAND(name, "V", (tagval *args, int numargs), { \
        int gun = numargs > 0 ? args[0].getint() : -1; \
        fpsent *p = numargs > 1 ? getclient(args[1].getint()) : hudplayer(); \
        return ret;\
    })

    int damage(fpsent *p, int typ, int gun)
    {
        if(!p) p = hudplayer();
        return p->stats.get(typ, gun);
    }
    DMG_ICOMMAND(getdamagepotential, intret(damage(p, DMG_POTENTIAL, gun)))
    DMG_ICOMMAND(getdamagedealt, intret(damage(p, DMG_DEALT, gun)))
    DMG_ICOMMAND(getdamagereceived, intret(damage(p, DMG_RECEIVED, gun)))

    int damagewasted(fpsent *p, int gun)
    {
        if(!p) p = hudplayer();
        return p->stats.wasted(gun);
    }
    DMG_ICOMMAND(getdamagewasted, intret(damagewasted(p, gun)))

    int netdamage(fpsent *p, int gun)
    {
        if(!p) p = hudplayer();
        return p->stats.net(gun);
    }
    DMG_ICOMMAND(getnetdamage, intret(netdamage(p, gun)))

    float accuracy(fpsent *p, int gun)
    {
        if(!p) p = hudplayer();
        return p->stats.accuracy(gun);
    }
    DMG_ICOMMAND(getaccuracy, floatret(accuracy(p, gun)))
}
