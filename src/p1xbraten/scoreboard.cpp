#include "game.h"
#include "weaponstats.h"

namespace game
{
    extern int showservinfo, gamespeed, highlightscore, hidefrags, showdeaths, showping, showpj, showclientnum, showspectators, showspectatorping;

namespace mod
{
    MOD(VARP, showsuicides, 0, 0, 1);
    MOD(VARP, showflags, 0, 0, 1);
    MOD(VARP, showkpd, 0, 0, 1);
    MOD(VARP, showaccuracy, 0, 0, 1);
    MOD(VARP, showdamage, 0, 0, 2);
    MOD(VARP, showdamagereceived, 0, 0, 1);
    MOD(VARP, showanticheatstatus, 0, 0, 1);

    int statuscolor(fpsent *d, int color)
    {
        if(d->privilege) color = d->privilege>=PRIV_ADMIN ? COL_ADMIN : (d->privilege>=PRIV_AUTH ? COL_AUTH : COL_MASTER);
        if(d->state==CS_DEAD) color = (color>>1)&0x7F7F7F;
        return color;
    }

    void renderscoreboard(g3d_gui &g, bool firstpass)
    {
        g.space(.25f);

        const ENetAddress *address = connectedpeer();
        if(showservinfo && address)
        {
            string hostname;
            if(enet_address_get_host_ip(address, hostname, sizeof(hostname)) >= 0)
            {
                if(servdesc[0]) g.titlef("%.25s", COL_WHITE, NULL, servdesc);
                else g.titlef("%s:%d", COL_WHITE, NULL, hostname, address->port);
            }
        }

        g.pushlist();
        g.spring();
        g.text(server::modename(gamemode), COL_WHITE);
        g.space(3);
        const char *mname = getclientmap();
        g.text(mname[0] ? mname : "[new map]", COL_WHITE);
        if(m_timed && mname[0] && (maplimit >= 0 || intermission))
        {
            g.space(3);
            if(intermission) g.text("intermission", COL_WHITE);
            else
            {
                int secs = max(maplimit-lastmillis+999, 0)/1000, mins = secs/60;
                secs %= 60;
                g.pushlist();
                g.strut(mins >= 10 ? 4.5f : 3.5f);
                g.textf("%d:%02d", COL_WHITE, NULL, mins, secs);
                g.poplist();
            }
        }
        if(gamespeed != 100) { g.space(3); g.textf("%d.%02dx", COL_GRAY, NULL, gamespeed/100, gamespeed%100); }
        if(ispaused()) { g.space(3); g.text("paused", COL_GRAY); }
        g.spring();
        g.poplist();

        g.space(.75f);

        int numgroups = groupplayers();
        loopk(numgroups)
        {
            if((k%2)==0) g.pushlist(); // horizontal

            scoregroup &sg = *groups[k];
            int teamcolor = sg.team && m_teammode ? (isteam(player1->team, sg.team) ? COL_BLUE : COL_RED) : 0;

            g.pushlist(); // vertical
            g.pushlist(); // horizontal

            #define loopscoregroup(o, b) \
                loopv(sg.players) \
                { \
                    fpsent *o = sg.players[i]; \
                    b; \
                }

            #define rightjustified(b) \
                { \
                    g.pushlist(); \
                    g.spring(); \
                    b; \
                    g.poplist(); \
                }

            #define fgcolor (o==hudplayer() && highlightscore && (multiplayer(false) || demoplayback || players.length() > 1) ? COL_YELLOW : COL_WHITE)

            if(sg.team && m_teammode)
            {
                g.pushlist(); // vertical
                if(sg.score>=10000) g.textf("%s: WIN", teamcolor, NULL, sg.team);
                else g.textf("%s: %d", teamcolor, NULL, sg.team, sg.score);
                g.pushlist(); // horizontal
            }

            g.pushlist();
            g.text("name", COL_GRAY);
            loopscoregroup(o, g.text(colorname(o), statuscolor(o, fgcolor)););
            g.poplist();

            if(m_ctf && showflags)
            {
                g.space(2);
                g.pushlist();
                rightjustified(g.text("flags", COL_GRAY))
                loopscoregroup(o, rightjustified(g.textf("%d", fgcolor, NULL, o->flags)));
                g.poplist();
            }

            if(!cmode || !cmode->hidefrags() || !hidefrags)
            {
                g.space(2);
                g.pushlist();
                rightjustified(g.text("frags", COL_GRAY))
                loopscoregroup(o, rightjustified(g.textf("%d", fgcolor, NULL, o->frags)));
                g.poplist();
            }

            if(showdeaths)
            {
                g.space(2);
                g.pushlist();
                rightjustified(g.text("deaths", COL_GRAY))
                loopscoregroup(o, rightjustified(g.textf("%d", fgcolor, NULL, o->deaths)));
                g.poplist();
            }

            if(showsuicides)
            {
                g.space(2);
                g.pushlist();
                rightjustified(g.text("suis", COL_GRAY))
                loopscoregroup(o, rightjustified(g.textf("%d", fgcolor, NULL, o->suicides)));
                g.poplist();
            }

            if(showkpd)
            {
                g.space(2);
                g.pushlist();
                g.strut(3);
                rightjustified(g.text("kpd", COL_GRAY))
                loopscoregroup(o, rightjustified(g.textf("%.1f", fgcolor, NULL, (float)o->frags/max(1, o->deaths))));
                g.poplist();
            }

            if(showaccuracy)
            {
                g.space(2);
                g.pushlist();
                g.strut(4);
                rightjustified(g.text("acc", COL_GRAY))
                loopscoregroup(o, rightjustified(g.textf("%.0f%%", fgcolor, NULL, playeraccuracy(o))));
                g.poplist();
            }

            if(!m_insta)
            {
                if(showdamage)
                {
                    g.space(2);
                    g.pushlist();
                    g.strut(4);
                    rightjustified(g.text("dmg", COL_GRAY))
                    loopscoregroup(o, {
                        float dmg = (float) showdamage == 1 ? playerdamage(o, DMG_DEALT) : playernetdamage(o);
                        const char *fmt = "%.0f";
                        if(fabs(dmg) > 1000.0f) { fmt = "%.1fk"; dmg = dmg/1000.0f; }
                        rightjustified(g.textf(fmt, fgcolor, NULL, dmg));
                    });
                    g.poplist();
                }

                if(showdamagereceived)
                {
                    g.space(2);
                    g.pushlist();
                    g.strut(4);
                    rightjustified(g.text("dr", COL_GRAY))
                    loopscoregroup(o, {
                        float dmg = (float) playerdamage(o, DMG_RECEIVED);
                        const char *fmt = "%.0f";
                        if(fabs(dmg) > 1000.0f) { fmt = "%.1fk"; dmg = dmg/1000.0f; }
                        rightjustified(g.textf(fmt, fgcolor, NULL, dmg));
                    });
                    g.poplist();
                }
            }

            if(multiplayer(false) || demoplayback)
            {
                if(showping)
                {
                    g.space(2);
                    g.pushlist();
                    rightjustified(g.text("ping", COL_GRAY))
                    loopscoregroup(o,
                    {
                        fpsent *p = o->ownernum >= 0 ? getclient(o->ownernum) : o;
                        if(!p) p = o;
                        if(!showpj && p->state==CS_LAGGED) rightjustified(g.text("LAG", fgcolor))
                        else rightjustified(g.textf("%d", fgcolor, NULL, p->ping))
                    });
                    g.poplist();
                }

                if(showpj)
                {
                    g.space(2);
                    g.pushlist();
                    g.strut(2);
                    rightjustified(g.text("pj", COL_GRAY))
                    loopscoregroup(o,
                    {
                        fpsent *p = o->ownernum >= 0 ? getclient(o->ownernum) : o;
                        if(!p) p = o;
                        if(p==player1) rightjustified(g.text("0", fgcolor))
                        else if(p->state==CS_LAGGED) rightjustified(g.text("LAG", fgcolor))
                        else rightjustified(g.textf("%d", fgcolor, NULL, abs(33-p->plag)))
                    });
                    g.poplist();
                }
            }

#ifdef ANTICHEAT
            if(anticheatenabled && showanticheatstatus)
            {
                g.space(2);
                g.pushlist();
                rightjustified(g.text("anticheat", COL_GRAY))
                loopscoregroup(o, rightjustified(g.textf("%s", fgcolor, NULL, o->anticheatverified ? "yes" : "no")));
                g.poplist();
            }
#endif

            if(showclientnum || player1->privilege>=PRIV_MASTER)
            {
                g.space(2);
                g.pushlist();
                rightjustified(g.text("cn", COL_GRAY))
                loopscoregroup(o, rightjustified(g.textf("%d", fgcolor, NULL, o->clientnum)));
                g.poplist();
            }

            if(sg.team && m_teammode)
            {
                g.poplist(); // horizontal
                g.poplist(); // vertical
            }

            g.poplist(); // horizontal
            g.poplist(); // vertical

            if(k+1<numgroups && (k+1)%2) g.space(3);
            else
            {
                g.poplist(); // horizontal
                if(k+1<numgroups) g.space(.75f);
            }
        }

        if(showspectators && spectators.length())
        {
            #define loopspectators(o, b) \
                loopv(spectators) \
                { \
                    fpsent *o = spectators[i]; \
                    b; \
                }

            g.space(.75f);

            if(showclientnum || showspectatorping || player1->privilege>=PRIV_MASTER)
            {
                g.pushlist();

                g.pushlist();
                g.text("spectator", COL_GRAY);
                loopspectators(o, g.text(colorname(o), statuscolor(o, fgcolor)));
                g.poplist();

                if((multiplayer(false) || demoplayback) && showspectatorping)
                {
                    g.space(2);
                    g.pushlist();
                    rightjustified(g.text("ping", COL_GRAY))
                    loopspectators(o,
                    {
                        fpsent *p = o->ownernum >= 0 ? getclient(o->ownernum) : o;
                        if(!p) p = o;
                        if(p->state==CS_LAGGED) rightjustified(g.text("LAG", fgcolor))
                        else rightjustified(g.textf("%d", fgcolor, NULL, p->ping))
                    });
                    g.poplist();
                }

                if(showclientnum || player1->privilege>=PRIV_MASTER)
                {
                    g.space(2);
                    g.pushlist();
                    rightjustified(g.text("cn", COL_GRAY))
                    loopspectators(o, rightjustified(g.textf("%d", fgcolor, NULL, o->clientnum)));
                    g.poplist();
                }

                g.poplist();
            }
            else
            {
                g.textf("%d spectator%s", COL_GRAY, NULL, spectators.length(), spectators.length()!=1 ? "s" : "");
                loopspectators(o,
                {
                    if((i%3)==0)
                    {
                        g.pushlist();
                        g.text("", fgcolor, 0);
                    }
                    g.text(colorname(o), statuscolor(o, fgcolor));
                    if(i+1<spectators.length() && (i+1)%3) g.space(1);
                    else g.poplist();
                });
            }
        }

        g.space(.25f);
        }

    }
}
