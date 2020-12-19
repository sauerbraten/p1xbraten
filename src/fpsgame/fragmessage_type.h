#ifndef __FRAGMESSAGE_TYPE_H__
#define __FRAGMESSAGE_TYPE_H__

#include "game.h"

struct fragmessage {
    string attackername, victimname;
    int weapon;
    int fragtime;

    fragmessage(const char *aname, const char *vname, int fragweapon)
    {
        copystring(attackername, aname ? aname : "");
        copystring(victimname, vname);
        weapon = fragweapon;
        fragtime = lastmillis;
    }
};

#endif
