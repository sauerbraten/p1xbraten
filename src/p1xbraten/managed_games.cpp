#include "game.h"

#ifndef STANDALONE
namespace game {

    bool managedgamedemonextmatch = false;
    string managedgamedemofname;
    
    void sendclientdemo()
    {       
        if(!managedgamedemofname[0]) { conoutf(CON_ERROR, "client demo requested, but demo file name unknown!"); return; };
        conoutf("sending demo %s to server...", managedgamedemofname);
        stream *demo = NULL;
        if(const char *buf = server::getdemofile(managedgamedemofname, true)) demo = openrawfile(buf, "rb");
        if(!demo) demo = openrawfile(managedgamedemofname, "rb");
        managedgamedemofname[0] = '\0';
        if(!demo) { conoutf("failed to open demo file"); return; }
        stream::offset len = demo->size();
        if(len > 16*1024*1024) conoutf(CON_ERROR, "demo is too large"); // todo: is 16 MB enough?
        else if(len <= 0) conoutf(CON_ERROR, "could not read demo");
        else sendfile(-1, 3, demo);
        DELETEP(demo);
        conoutf("client demo sent");
    }
}
#endif


namespace server {

    void probeforclientdemoupload(packetbuf &p)
    {
        putint(p, N_SERVCMD);
        sendstring(CAP_PROBE_CLIENT_DEMO_UPLOAD, p);
    }

    bool managedgame = false;
    bool managedgamenextmatch = false;

    void preparemanagedgame(clientinfo *referee)
    {
        if(!referee || (!referee->local && !referee->privilege)) return;
        if(mastermode < MM_LOCKED) { mastermode = MM_LOCKED; sendf(-1, 1, "rii", N_MASTERMODE, mastermode); }
        
        sendf(-1, 1, "ris", N_SERVMSG, "next match will be a managed game");

        loopv(clients)
        {
            clientinfo *ci = clients[i];
            if(ci->state.state==CS_SPECTATOR) continue;
            if(ci->supportsclientdemoupload) sendf(ci->clientnum, 1, "rii", N_P1X_RECORDDEMO, 1);
        }

        managedgamenextmatch = true;
        demonextmatch = true;
    }

    void cleanupmanagedgame()
    {
        managedgame = false;
        managedgamenextmatch = false;
    }

    static vector<clientinfo *> notyetspawned;

    void setupmanagedgame()
    {
        managedgame = true;
        managedgamenextmatch = false;
        notyetspawned.shrink(0);
        loopv(clients)
        {
            clientinfo *ci = clients[i];
            if(ci->state.state!=CS_SPECTATOR) notyetspawned.add(ci);
        }
        pausegame(true);
        sendf(-1, 1, "ris", N_SERVMSG, "waiting for all players to load the map");
        resuming = true;
    }

    void managedgameended()
    {
        managedgame = false;
        loopv(clients)
        {
            clientinfo *ci = clients[i];
            if(ci->state.state!=CS_SPECTATOR) sendf(ci->clientnum, 1, "rii", N_P1X_RECORDDEMO, 0);
        }
    }

    const char *demofilename(const char *povname)
    {
        if(!povname) return NULL;

        static string ts;
        ts[0] = 0;
        time_t t = time(NULL);
        size_t len = strftime(ts, sizeof(ts), "%Y-%m-%d_%H.%M.%S", localtime(&t));
        ts[min(len, sizeof(ts)-1)] = 0;

        static string basename;
        formatstring(basename, "%s_%s_%s_%s", ts, gamemodes[gamemode-STARTGAMEMODE].name, smapname, povname);
        filtertext(basename, basename, false);
        len = strlen(basename);
        if(len < 4 || strcasecmp(&basename[len-4], ".dmo")) concatstring(basename, ".dmo");
        
        const char* fname = getdemofile(basename, true);
        return fname ? fname : basename;
    }

    bool savedemo(demofile &d, const char *povname)
    {
        const char *fname = demofilename(povname);
        if(!fname) { conoutf(CON_WARN, "could not build demo file name"); return false; }
        stream *f = openrawfile(fname, "wb");
        if(!f) { conoutf(CON_WARN, "could not open %s to save demo", fname); return false; }
        f->write(d.data, d.len);
        conoutf("saved demo \"%s\"", fname);
        delete f;
        return true;
    }


    void notifyprivusers(int minpriv, char *msg)
    {
        loopv(clients)
        {
            clientinfo *ci = clients[i];
            if(!ci || ci->privilege < minpriv) continue;
            sendf(ci->clientnum, 1, "ris", N_SERVMSG, msg);
        }
    }

    void receiveclientdemo(int sender, uchar *data, int len)
    {
        conoutf("receiving client demo file (length %d)", len);
        if(len <= 0 || len > 16*1024*1024) return;
        clientinfo *ci = getinfo(sender);
        if(ci->state.state==CS_SPECTATOR) return;
        demofile d; d.data = data; d.len = len;
        savedemo(d, ci->name);
        static string msg;
        formatstring(msg, "received client-side demo from %s", colorname(ci));
        notifyprivusers(PRIV_ADMIN, msg);
    }

    struct messageevent : gameevent
    {
        int millis;
        clientinfo *ci = NULL;
        string msg;

        bool flush(clientinfo *_ , int fmillis)
        {
            if(millis > fmillis) return false;
            if(!ci || ci->connected) sendf(-1, 1, "ris", N_SERVMSG, msg);
            return true;
        }
    };

    struct startevent : gameevent
    {
        int millis;

        bool flush(clientinfo *_ , int fmillis)
        {
            if(millis > fmillis) return false;
            pausegame(false);
            if(m_mp(gamemode)) loopv(clients)
            {
                clientinfo *ci = clients[i];
                if(ci->state.state!=CS_SPECTATOR) sendspawn(ci);
            }
            return true;
        }
    };

    static const int COUNTDOWN_INTERVAL = 650;

    void onspawn(clientinfo *ci)
    {
        if(!notyetspawned.length()) return;
        loopv(notyetspawned) if(notyetspawned[i]==ci)
        {
            notyetspawned.remove(i);
            if(!notyetspawned.length())
            {
                sendf(-1, 1, "ris", N_SERVMSG, "all players ready, game starts in 3");

                messageevent *m2 = new messageevent;
                m2->millis = totalmillis + 1*COUNTDOWN_INTERVAL;
                copystring(m2->msg, "... 2");
                events.add(m2);

                messageevent *m1 = new messageevent;
                m1->millis = totalmillis + 2*COUNTDOWN_INTERVAL;
                copystring(m1->msg, "... 1");
                events.add(m1);

                startevent *u = new startevent;
                u->millis = totalmillis + 3*COUNTDOWN_INTERVAL;
                events.add(u);
            }
        }
    }

    bool resuming = false;

    struct resumeevent : gameevent
    {
        int millis;
        clientinfo *ci = NULL;

        bool flush(clientinfo *_ , int fmillis)
        {
            if(millis > fmillis) return false;
            if(ci)
            {
                if(!ci->connected) return true;
                if(ci->state.state==CS_SPECTATOR && ci->privilege < (restrictpausegame ? PRIV_ADMIN : PRIV_MASTER) && !ci->local) return true;
            } 
            pausegame(false, ci);
            return true;
        }
    };

    void resumewithcountdown(clientinfo *ci)
    {
        if(resuming) return;
        resuming = true;

        sendf(-1, 1, "ris", N_SERVMSG, "resuming in 3");

        messageevent *m2 = new messageevent;
        m2->millis = totalmillis + 1*COUNTDOWN_INTERVAL;
        m2->ci = ci;
        copystring(m2->msg, "... 2");
        events.add(m2);

        messageevent *m1 = new messageevent;
        m1->millis = totalmillis + 2*COUNTDOWN_INTERVAL;
        m1->ci = ci;
        copystring(m1->msg, "... 1");
        events.add(m1);

        resumeevent *r = new resumeevent;
        r->millis = totalmillis + 3*COUNTDOWN_INTERVAL;
        r->ci = ci;
        events.add(r);
    }
}
