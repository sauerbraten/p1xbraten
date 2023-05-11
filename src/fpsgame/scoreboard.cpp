// creation of scoreboard
#include "game.h"

namespace game
{
    VARP(scoreboard2d, 0, 1, 1);
    VARP(showservinfo, 0, 1, 1);
    VARP(showclientnum, 0, 0, 1);
    VARP(showpj, 0, 0, 1);
    VARP(showping, 0, 1, 2);
    VARP(showspectators, 0, 1, 1);
    VARP(showspectatorping, 0, 0, 1);
    VARP(highlightscore, 0, 1, 1);
    VARP(showconnecting, 0, 0, 1);
    VARP(hidefrags, 0, 1, 1);
    VARP(showdeaths, 0, 0, 1);
    VARP(tiesort, 0, 0, 2);

    hashset<teaminfo> teaminfos;

    void clearteaminfo()
    {
        teaminfos.clear();
    }

    void setteaminfo(const char *team, int frags)
    {
        teaminfo *t = teaminfos.access(team);
        if(!t) { t = &teaminfos[team]; copystring(t->team, team, sizeof(t->team)); }
        t->frags = frags;
    }

    static inline bool playersort(const fpsent *a, const fpsent *b)
    {
        if(a->state==CS_SPECTATOR)
        {
            if(b->state==CS_SPECTATOR) return strcmp(a->name, b->name) < 0;
            else return false;
        }
        else if(b->state==CS_SPECTATOR) return true;
        if(m_ctf || m_collect)
        {
            if(a->flags > b->flags) return true;
            if(a->flags < b->flags) return false;
            if(tiesort)
            {
                if(a == player1) return tiesort == 1;
                if(b == player1) return tiesort != 1;
            }
        }
        if(a->frags > b->frags) return true;
        if(a->frags < b->frags) return false;
        if(tiesort)
        {
            if(a == player1) return tiesort == 1;
            if(b == player1) return tiesort != 1;
        }
        return strcmp(a->name, b->name) < 0;
    }

    void getbestplayers(vector<fpsent *> &best)
    {
        loopv(players)
        {
            fpsent *o = players[i];
            if(o->state!=CS_SPECTATOR) best.add(o);
        }
        best.sort(playersort);
        while(best.length() > 1 && best.last()->frags < best[0]->frags) best.drop();
    }

    void getbestteams(vector<const char *> &best)
    {
        if(cmode && cmode->hidefrags())
        {
            vector<teamscore> teamscores;
            cmode->getteamscores(teamscores);
            teamscores.sort(teamscore::compare);
            while(teamscores.length() > 1 && teamscores.last().score < teamscores[0].score) teamscores.drop();
            loopv(teamscores) best.add(teamscores[i].team);
        }
        else
        {
            int bestfrags = INT_MIN;
            enumerate(teaminfos, teaminfo, t, bestfrags = max(bestfrags, t.frags));
            if(bestfrags <= 0) loopv(players)
            {
                fpsent *o = players[i];
                if(o->state!=CS_SPECTATOR && !teaminfos.access(o->team) && best.htfind(o->team) < 0) { bestfrags = 0; best.add(o->team); }
            }
            enumerate(teaminfos, teaminfo, t, if(t.frags >= bestfrags) best.add(t.team));
        }
    }

    vector<scoregroup *> groups;
    vector<fpsent *> spectators;

    static inline bool scoregroupcmp(const scoregroup *x, const scoregroup *y)
    {
        if(!x->team)
        {
            if(y->team) return false;
        }
        else if(!y->team) return true;
        if(x->score > y->score) return true;
        if(x->score < y->score) return false;
        if(x->players.length() > y->players.length()) return true;
        if(x->players.length() < y->players.length()) return false;
        return x->team && y->team && strcmp(x->team, y->team) < 0;
    }

    int groupplayers()
    {
        int numgroups = 0;
        spectators.setsize(0);
        loopv(players)
        {
            fpsent *o = players[i];
            if(!showconnecting && !o->name[0]) continue;
            if(o->state==CS_SPECTATOR) { spectators.add(o); continue; }
            const char *team = m_teammode && o->team[0] ? o->team : NULL;
            bool found = false;
            loopj(numgroups)
            {
                scoregroup &g = *groups[j];
                if(team!=g.team && (!team || !g.team || strcmp(team, g.team))) continue;
                g.players.add(o);
                found = true;
            }
            if(found) continue;
            if(numgroups>=groups.length()) groups.add(new scoregroup);
            scoregroup &g = *groups[numgroups++];
            g.team = team;
            if(!team) g.score = 0;
            else if(cmode && cmode->hidefrags()) g.score = cmode->getteamscore(o->team);
            else { teaminfo *ti = teaminfos.access(team); g.score = ti ? ti->frags : 0; }
            g.players.setsize(0);
            g.players.add(o);
        }
        loopi(numgroups) groups[i]->players.sort(playersort);
        spectators.sort(playersort);
        groups.sort(scoregroupcmp, 0, numgroups);
        return numgroups;
    }

    int statuscolor(fpsent *d, int color)
    {
        if(d->privilege)
        {
            color = d->privilege>=PRIV_ADMIN ? COL_ADMIN : (d->privilege>=PRIV_AUTH ? COL_AUTH : COL_MASTER);
            if(d->state==CS_DEAD) color = (color>>1)&0x7F7F7F;
        }
        else if(d->state==CS_DEAD) color = COL_GRAY;
        return color;
    }

    void renderscoreboard(g3d_gui &g, bool firstpass)
    {
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
        g.separator();
        const char *mname = getclientmap();
        g.text(mname[0] ? mname : "[new map]", COL_WHITE);
        extern int gamespeed;
        if(gamespeed != 100) { g.separator(); g.textf("%d.%02dx", COL_WHITE, NULL, gamespeed/100, gamespeed%100); }
        if(m_timed && mname[0] && (maplimit >= 0 || intermission))
        {
            g.separator();
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
        if(ispaused()) { g.separator(); g.text("paused", COL_WHITE); }
        g.spring();
        g.poplist();

        g.separator();

        int numgroups = groupplayers();
        loopk(numgroups)
        {
            if((k%2)==0) g.pushlist(); // horizontal

            scoregroup &sg = *groups[k];
            int bgcolor = sg.team && m_teammode ? (isteam(player1->team, sg.team) ? COL_BLUE : COL_RED) : 0;
#define COL_YELLOWISH 0xFFFF80

            g.pushlist(); // vertical
            g.pushlist(); // horizontal

            #define loopscoregroup(o, b) \
                loopv(sg.players) \
                { \
                    fpsent *o = sg.players[i]; \
                    b; \
                }

            g.pushlist();
            if(sg.team && m_teammode)
            {
                g.pushlist();
                g.background(bgcolor, numgroups>1 ? 3 : 5);
                g.strut(1);
                g.poplist();
            }
            g.text("", 0, " ");
            loopscoregroup(o,
            {
                if(o==player1 && highlightscore && (multiplayer(false) || demoplayback || players.length() > 1))
                {
                    g.pushlist();
                    g.background(COL_BACKGROUND, numgroups>1 ? 3 : 5);
                }
                const playermodelinfo &mdl = getplayermodelinfo(o);
                const char *icon = sg.team && m_teammode ? (isteam(player1->team, sg.team) ? mdl.blueicon : mdl.redicon) : mdl.ffaicon;
                g.text("", 0, icon);
                if(o==player1 && highlightscore && (multiplayer(false) || demoplayback || players.length() > 1)) g.poplist();
            });
            g.poplist();

            if(sg.team && m_teammode)
            {
                g.pushlist(); // vertical

                if(sg.score>=10000) g.textf("%s: WIN", COL_YELLOWISH, NULL, sg.team);
                else g.textf("%s: %d", COL_YELLOWISH, NULL, sg.team, sg.score);

                g.pushlist(); // horizontal
            }

            if(!cmode || !cmode->hidefrags() || !hidefrags)
            {
                g.pushlist();
                g.strut(6);
                g.text("frags", COL_YELLOWISH);
                loopscoregroup(o, g.textf("%d", COL_WHITE, NULL, o->frags));
                g.poplist();
            }

            if(showdeaths)
            {
                g.pushlist();
                g.strut(6);
                g.text("deaths", COL_YELLOWISH);
                loopscoregroup(o, g.textf("%d", COL_WHITE, NULL, o->deaths));
                g.poplist();
            }

            g.pushlist();
            g.text("name", COL_YELLOWISH);
            g.strut(12);
            loopscoregroup(o,
            {
                g.textf("%s ", statuscolor(o, COL_WHITE), NULL, colorname(o));
            });
            g.poplist();

            if(multiplayer(false) || demoplayback)
            {
                if(showpj || showping) g.space(1);

                if(showpj && showping <= 1)
                {
                    g.pushlist();
                    g.strut(6);
                    g.text("pj", COL_YELLOWISH);
                    loopscoregroup(o,
                    {
                        if(o->state==CS_LAGGED) g.text("LAG", COL_WHITE);
                        else g.textf("%d", COL_WHITE, NULL, o->plag);
                    });
                    g.poplist();
                }

                if(showping > 1)
                {
                    g.pushlist();
                    g.strut(6);

                    g.pushlist();
                    g.text("ping", COL_YELLOWISH);
                    g.space(1);
                    g.spring();
                    g.text("pj", COL_YELLOWISH);
                    g.poplist();

                    loopscoregroup(o,
                    {
                        fpsent *p = o->ownernum >= 0 ? getclient(o->ownernum) : o;
                        if(!p) p = o;
                        g.pushlist();
                        if(p->state==CS_LAGGED) g.text("LAG", COL_WHITE);
                        else
                        {
                            g.textf("%d", COL_WHITE, NULL, p->ping);
                            g.space(1);
                            g.spring();
                            g.textf("%d", COL_WHITE, NULL, o->plag);
                        }
                        g.poplist();

                    });
                    g.poplist();
                }
                else if(showping)
                {
                    g.pushlist();
                    g.text("ping", COL_YELLOWISH);
                    g.strut(6);
                    loopscoregroup(o,
                    {
                        fpsent *p = o->ownernum >= 0 ? getclient(o->ownernum) : o;
                        if(!p) p = o;
                        if(!showpj && p->state==CS_LAGGED) g.text("LAG", COL_WHITE);
                        else g.textf("%d", COL_WHITE, NULL, p->ping);
                    });
                    g.poplist();
                }
            }

            if(showclientnum || player1->privilege>=PRIV_MASTER)
            {
                g.space(1);
                g.pushlist();
                g.text("cn", COL_YELLOWISH);
                loopscoregroup(o, g.textf("%d", COL_WHITE, NULL, o->clientnum));
                g.poplist();
            }

            if(sg.team && m_teammode)
            {
                g.poplist(); // horizontal
                g.poplist(); // vertical
            }

            g.poplist(); // horizontal
            g.poplist(); // vertical

            if(k+1<numgroups && (k+1)%2) g.space(2);
            else g.poplist(); // horizontal
        }

        if(showspectators && spectators.length())
        {
            if(showclientnum || player1->privilege>=PRIV_MASTER)
            {
                g.pushlist();

                g.pushlist();
                g.text("spectator", COL_YELLOWISH, " ");
                g.strut(12);
                loopv(spectators)
                {
                    fpsent *o = spectators[i];
                    if(o==player1 && highlightscore)
                    {
                        g.pushlist();
                        g.background(COL_BACKGROUND, 3);
                    }
                    g.text(colorname(o), statuscolor(o, COL_WHITE), "spectator");
                    if(o==player1 && highlightscore) g.poplist();
                }
                g.poplist();

                if((multiplayer(false) || demoplayback) && showspectatorping)
                {
                    g.space(1);
                    g.pushlist();
                    g.text("ping", COL_YELLOWISH);
                    g.strut(6);
                    loopv(spectators)
                    {
                        fpsent *o = spectators[i];
                        fpsent *p = o->ownernum >= 0 ? getclient(o->ownernum) : o;
                        if(!p) p = o;
                        if(p->state==CS_LAGGED) g.text("LAG", COL_WHITE);
                        else g.textf("%d", COL_WHITE, NULL, p->ping);
                    }
                    g.poplist();
                }

                g.space(1);
                g.pushlist();
                g.text("cn", COL_YELLOWISH);
                loopv(spectators) g.textf("%d", COL_WHITE, NULL, spectators[i]->clientnum);
                g.poplist();

                g.poplist();
            }
            else
            {
                g.textf("%d spectator%s", COL_YELLOWISH, " ", spectators.length(), spectators.length()!=1 ? "s" : "");
                loopv(spectators)
                {
                    if((i%3)==0)
                    {
                        g.pushlist();
                        g.text("", COL_WHITE, "spectator");
                    }
                    fpsent *o = spectators[i];
                    if(o==player1 && highlightscore)
                    {
                        g.pushlist();
                        g.background(COL_BACKGROUND);
                    }
                    g.text(colorname(o), statuscolor(o, COL_WHITE));
                    if(o==player1 && highlightscore) g.poplist();
                    if(i+1<spectators.length() && (i+1)%3) g.space(1);
                    else g.poplist();
                }
            }
        }
    }

    extern int usep1xbratenmenus;
    namespace mod { extern void renderscoreboard(g3d_gui &g, bool firstpass); }

    struct scoreboardgui : g3d_callback
    {
        bool showing;
        vec menupos;
        int menustart;

        scoreboardgui() : showing(false) {}

        void show(bool on)
        {
            if(!showing && on)
            {
                menupos = menuinfrontofplayer();
                menustart = starttime();
            }
            showing = on;
        }

        void gui(g3d_gui &g, bool firstpass)
        {
            g.start(menustart, 0.03f, NULL, false);
            if(usep1xbratenmenus) mod::renderscoreboard(g, firstpass);
            else renderscoreboard(g, firstpass);
            g.end();
        }

        void render()
        {
            if(showing) g3d_addgui(this, menupos, (scoreboard2d ? GUI_FORCE_2D : GUI_2D | GUI_FOLLOW) | GUI_BOTTOM);
        }

    } scoreboard;

    void g3d_gamemenus()
    {
        scoreboard.render();
    }

    int wasfullconsole = 0;
    void showscores(bool on)
    {
        if(on && !wasfullconsole)
        {
            wasfullconsole = fullconsole;
            fullconsole = 0;
        }
        if(!on)
        {
           if(!fullconsole && wasfullconsole) fullconsole = 1;
            wasfullconsole = 0;
        }
        extern int showscoreboard;
        showscoreboard = on ? 1 : 0;
        scoreboard.show(on);
    }
    ICOMMAND(showscores, "D", (int *down), showscores(*down!=0));
    VARFN(scoreboard, showscoreboard, 0, 0, 1, showscores(showscoreboard!=0));

    VARP(hudscore, 0, 0, 1);
    FVARP(hudscorescale, 0.5, 1.0, 2.0);
    VARP(hudscorealign, -1, 0, 1);
    FVARP(hudscorex, 0.0, 0.50, 1.0);
    FVARP(hudscorey, 0.0, 0.03, 1.0);
    HVARP(hudscoreplayercolour, 0, COL_BLUE, COL_WHITE);
    HVARP(hudscoreenemycolour, 0, COL_RED, COL_WHITE);
    VARP(hudscorealpha, 0, 255, 255);
    VARP(hudscoresep, 0, 200, 1000);

    void drawhudscore(int w, int h)
    {
        int numgroups = groupplayers();
        if(!numgroups || (m_teammode ? numgroups : groups[0]->players.length()) < 2) return;

        scoregroup *g = groups[0];
        int score = INT_MIN, score2 = INT_MIN;
        bool best = false;
        if(m_teammode)
        {
            score = g->score;
            best = isteam(player1->team, g->team);
            if(numgroups > 1)
            {
                if(best) score2 = groups[1]->score;
                else for(int i = 1; i < groups.length(); ++i) if(isteam(player1->team, groups[i]->team)) { score2 = groups[i]->score; break; }
                if(score2 == INT_MIN)
                {
                    fpsent *p = followingplayer(player1);
                    if(p->state==CS_SPECTATOR) score2 = groups[1]->score;
                }
            }
        }
        else
        {
            fpsent *p = followingplayer(player1);
            score = g->players[0]->frags;
            best = p == g->players[0];
            if(g->players.length() > 1)
            {
                if(best || p->state==CS_SPECTATOR) score2 = g->players[1]->frags;
                else score2 = p->frags;
            }
        }
        if(score == score2 && !best) best = true;

        score = clamp(score, -999, 9999);
        defformatstring(buf, "%d", score);
        int tw = 0, th = 0;
        text_bounds(buf, tw, th);

        string buf2;
        int tw2 = 0, th2 = 0;
        if(score2 > INT_MIN)
        {
            score2 = clamp(score2, -999, 9999);
            formatstring(buf2, "%d", score2);
            text_bounds(buf2, tw2, th2);
        }

        int fw = 0, fh = 0;
        text_bounds("00", fw, fh);
        fw = max(fw, max(tw, tw2));

        vec2 offset = vec2(hudscorex, hudscorey).mul(vec2(w, h).div(hudscorescale));
        if(hudscorealign == 1) offset.x -= 2*fw + hudscoresep;
        else if(hudscorealign == 0) offset.x -= (2*fw + hudscoresep) / 2.0f;
        vec2 offset2 = offset;
        offset.x += (fw-tw)/2.0f;
        offset.y -= th/2.0f;
        offset2.x += fw + hudscoresep + (fw-tw2)/2.0f;
        offset2.y -= th2/2.0f;

        pushhudmatrix();
        hudmatrix.scale(hudscorescale, hudscorescale, 1);
        flushhudmatrix();

        int color = hudscoreplayercolour, color2 = hudscoreenemycolour;
        if(!best) swap(color, color2);

        draw_text(buf, int(offset.x), int(offset.y), (color>>16)&0xFF, (color>>8)&0xFF, color&0xFF, hudscorealpha);
        if(score2 > INT_MIN) draw_text(buf2, int(offset2.x), int(offset2.y), (color2>>16)&0xFF, (color2>>8)&0xFF, color2&0xFF, hudscorealpha);

        pophudmatrix();
    }
}

