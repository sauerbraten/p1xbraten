#include "game.h"
#include "serverbrowser.h"

extern vector<serverinfo *> servers;
extern void refreshservers();
extern serverinfo *selectedserver;

namespace game { namespace mod
{
    static const char * const names[] = { "ping", "players", "slots", "mode", "map", "time", "master", "host", "port", "description" };
    static const float struts[]       = {      0,         0,       0,  12.5f,    14,      0,        0,     14,      0,         24.5f };
    static const bool right[]         = {   true,      true,    true,  false, false,  false,    false,  false,   true,         false };

    MOD(SVAR, filterservers, "");

    bool servermatches(serverinfo *si)
    {
        return cubecasefind(si->sdesc, filterservers) ||
            cubecasefind(si->map, filterservers) ||
            (si->attr.length()>=2 && cubecasefind(server::modename(si->attr[1], ""), filterservers));
    }

    #define COLUMNTOGGLE(col) MOD(VARP, serverbrowsershow##col, 0, 1, 1)

    COLUMNTOGGLE(ping); COLUMNTOGGLE(players); COLUMNTOGGLE(slots); COLUMNTOGGLE(mode); COLUMNTOGGLE(map);
    COLUMNTOGGLE(time); COLUMNTOGGLE(master);  COLUMNTOGGLE(host);  COLUMNTOGGLE(port); COLUMNTOGGLE(description);

    #undef COLUMNTOGGLE

    #define justified(elem, right) \
        { \
            /* pushlist to go horizontal, spring to push right, pushlist again to get vertical list dir back, so mergehits works */ \
            if((right)) { g->pushlist(); g->spring(); g->pushlist(); } \
            elem; \
            if((right)) { g->poplist(); g->poplist(); } \
        }

    bool serverinfostartcolumn(g3d_gui *g, int i)
    {
        if(size_t(i) >= sizeof(names)/sizeof(names[0])) return false;
        static string togglename; formatstring(togglename, "serverbrowsershow%s", names[i]);
        if(!getvar(togglename)) return false;
        if(i) g->space(2);
        g->pushlist();
        justified(g->text(names[i], COL_GRAY, NULL), right[i]);
        if(struts[i]) g->strut(struts[i]);
        g->mergehits(true);
        return true;
    }

    #define handlemouse(elem)    int hit = elem; if(hit&G3D_UP) return true
    #define leftjustified(elem)  justified(handlemouse(elem), false)
    #define rightjustified(elem) justified(handlemouse(elem), true)

    bool serverinfoentryincomplete(g3d_gui *g, int i, const char *name, int port, const char *sdesc, const char *map, int ping, const vector<int> &attr, int np)
    {
        switch(i)
        {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                leftjustified(g->button(" ", COL_WHITE))
                break;

            case 7:
                leftjustified(g->buttonf("%s", COL_WHITE, NULL, name))
                break;

            case 8:
                rightjustified(g->buttonf("%d", COL_WHITE, NULL, port))
                break;

            case 9:
                if(ping < 0) leftjustified(g->button(sdesc, COL_WHITE))
                else leftjustified(g->buttonf("[%s protocol]", COL_WHITE, NULL, attr.empty() ? "unknown" : (attr[0] < PROTOCOL_VERSION ? "older" : "newer")))
                break;
        }
        return false;
    }

    bool serverinfoentry(g3d_gui *g, int i, const char *name, int port, const char *sdesc, const char *map, int ping, const vector<int> &attr, int np)
    {
        if(ping < 0 || attr.empty() || attr[0]!=PROTOCOL_VERSION) return serverinfoentryincomplete(g, i, name, port, sdesc, map, ping, attr, np);

        switch(i)
        {
            case 0:
                rightjustified(g->buttonf("%d", COL_WHITE, NULL, ping))
                break;

            case 1:
                rightjustified(g->buttonf(attr.length()>=4 && np >= attr[3] ? "\f3%d" : "%d", COL_WHITE, NULL, np))
                break;

            case 2:
                if(attr.length()>=4) rightjustified(g->buttonf(np >= attr[3] ? "\f3%d" : "%d", COL_WHITE, NULL, attr[3]))
                else rightjustified(g->button(" ", COL_WHITE))
                break;

            case 3:
                leftjustified(g->buttonf("%s", COL_WHITE, NULL, attr.length()>=2 ? server::modename(attr[1], "") : ""))
                break;

            case 4:
                leftjustified(g->buttonf("%.25s", COL_WHITE, NULL, map))
                break;

            case 5:
                if(attr.length()>=3 && attr[2] > 0)
                {
                    int secs = clamp(attr[2], 0, 59*60+59),
                        mins = secs/60;
                    secs %= 60;
                    leftjustified(g->buttonf("%d:%02d", COL_WHITE, NULL, mins, secs))
                }
                else leftjustified(g->button(" ", COL_WHITE))
                break;

            case 6:
                leftjustified(g->buttonf("%s%s", COL_WHITE, NULL, attr.length()>=5 ? mastermodecolor(attr[4], "") : "", attr.length()>=5 ? server::mastermodename(attr[4], "") : ""))
                break;

            case 7:
                leftjustified(g->buttonf("%s", COL_WHITE, NULL, name))
                break;

            case 8:
                rightjustified(g->buttonf("%d", COL_WHITE, NULL, port))
                break;

            case 9:
                leftjustified(g->buttonf("%.25s", COL_WHITE, NULL, sdesc))
                break;
        }

        return false;
    }

    #undef handlemouse
    #undef leftjustified
    #undef rightjustified
    #undef justified

    void serverinfoendcolumn(g3d_gui *g, int i)
    {
        g->mergehits(false);
        g->column(i);
        g->poplist();
    }

    const char *showservers(g3d_gui *cgui, uint *header, int pagemin, int pagemax)
    {
        refreshservers();
        static vector<serverinfo *> filteredservers;
        filteredservers.shrink(0);
        loopv(servers) if(servermatches(servers[i])) filteredservers.add(servers[i]);
        if(filteredservers.empty())
        {
            if(header) execute(header);
            cgui->title("no servers", COL_ORANGE);
            return NULL;
        }
        serverinfo *sc = NULL;
        for(int start = 0; start < filteredservers.length();)
        {
            if(start > 0) cgui->tab();
            if(header) execute(header);
            int end = filteredservers.length();
            cgui->pushlist();
            loopi(sizeof(names)/sizeof(names[0]))
            {
                if(!serverinfostartcolumn(cgui, i)) continue;
                for(int j = start; j < end; j++)
                {
                    if(!i && j+1 - start > pagemin && (j+1 - start >= pagemax || cgui->shouldtab())) { end = j; break; }
                    serverinfo &si = *filteredservers[j];
                    const char *sdesc = si.sdesc;
                    if(si.address.host == ENET_HOST_ANY) sdesc = "[unknown host]";
                    else if(si.ping == serverinfo::WAITING) sdesc = "[waiting for response]";
                    if(serverinfoentry(cgui, i, si.name, si.port, sdesc, si.map, sdesc == si.sdesc ? si.ping : -1, si.attr, si.numplayers))
                        sc = &si;
                }
                if(end-start < pagemin) cgui->space(pagemin-(end-start));
                serverinfoendcolumn(cgui, i);
            }
            cgui->poplist();
            start = end;
        }
        if(selectedserver || !sc) return NULL;
        selectedserver = sc;
        return "connectselected";
    }

} }
