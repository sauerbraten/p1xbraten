#ifndef __FRAGMESSAGES_H__
#define __FRAGMESSAGES_H__

#include "game.h"

namespace game {
    extern vector<fragmessage> fragmessages;
    extern int hudfragmessages;
    extern void addfragmessage(fpsent *c, const char *aname, const char *vname, int gun);
    extern void drawfragmessages(fpsent *d, int w, int h);
}

#endif