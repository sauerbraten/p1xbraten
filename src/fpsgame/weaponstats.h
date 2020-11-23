#ifndef __WEAPONSTATS_H__
#define __WEAPONSTATS_H__

namespace game {
    extern void recordshot(fpsent *shooter);
    extern bool shouldcounthit(fpsent *attacker, fpsent *target);
    extern void recordhit(fpsent *attacker, fpsent *target, int damage);
    extern int damage(fpsent *p, int typ, int gun = -1);
    extern int damagewasted(fpsent *p, int gun = -1);
    extern int netdamage(fpsent *p, int gun = -1);
    extern float accuracy(fpsent *p, int gun = -1);
}

#endif
