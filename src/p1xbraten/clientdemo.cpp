#include "game.h"

// client-side demo recording works mostly the same as server-side:
// all packets coming from the server are recorded to a file.
//
// some messages going from us to the server need to be injected,
// because those actions aren't confirmed/acknowledged explicitly
// by the server:
//  - N_SWITCHNAME
//  - N_TEXT
//  - N_GUNSELECT
//  - N_SOUND
//  - N_CLIENTPING
//  - N_SHOTFX
//  - N_POS
//  - N_TELEPORT
//  - N_JUMPPAD

namespace game {
    bool demonextmatch = false;
    stream *demo = NULL, *demorecord = NULL;
    int demostartmillis = 0;

    void enddemorecord(bool send)
    {
        if(!demorecord) return;
        DELETEP(demorecord);
        conoutf("stopped client demo recording");
        if(!demo) return;
        if(send) sendclientdemo(demo);
        DELETEP(demo);
    }


    void recordpacket(int chan, uchar *data, int len)
    {
        if(!demorecord || !len) return;
        // peek message type
        int type = (schar)data[0];
        if(type==-128 && len > 3)
        {
            type = data[1];
            type |= ((schar)data[2])<<8;
        }
        else if(type==-127 && len > 5)
        {
            type = data[1];
            type |= ((schar)data[2])<<8;
            type |= ((schar)data[3])<<16;
            type |= ((schar)data[4])<<24;
        }
        // skip certain packets
        switch (type) {
            case N_SENDDEMOLIST:
            case N_AUTHTRY: case N_AUTHCHAL: case N_AUTHANS: case N_REQAUTH:
            case N_P1X_SETIP:
            case N_P1X_RECORDDEMO:
#ifdef ANTICHEAT
            case N_P1X_ANTICHEAT_BEGINSESSION: case N_P1X_ANTICHEAT_MESSAGE: case N_P1X_ANTICHEAT_ENDSESSION:
#endif
                return;
        }
        int stamp[3] = { lastmillis-demostartmillis, chan, len };
        lilswap(stamp, 3);
        demorecord->write(stamp, sizeof(stamp));
        demorecord->write(data, len);
    }

    // mostly the original sendstate(), but works with fpsent * now
    void sendstate(fpsent *d, packetbuf &p)
    {
        putint(p, d->lifesequence);
        putint(p, d->health);
        putint(p, d->maxhealth);
        putint(p, d->armour);
        putint(p, d->armourtype);
        putint(p, d->gunselect);
        loopi(GUN_PISTOL-GUN_SG+1) putint(p, d->ammo[GUN_SG+i]);
    }

    // mostly the original putinitclient(), but works with fpsent * now
    void putinitclient(fpsent *d, packetbuf &p)
    {
        if(d->aitype != AI_NONE)
        {
            putint(p, N_INITAI);
            putint(p, d->clientnum);
            putint(p, d->ownernum);
            putint(p, d->aitype);
            putint(p, d->skill);
            putint(p, d->playermodel);
            sendstring(d->name, p);
            sendstring(d->team, p);
        }
        else
        {
            putint(p, N_INITCLIENT);
            putint(p, d->clientnum);
            sendstring(d->name, p);
            sendstring(d->team, p);
            putint(p, d->playermodel);
        }
    }

    // mostly the original welcomeinitclient() function
    void welcomeinitclient(packetbuf &p)
    {
        loopv(players)
        {
            fpsent *pl = players[i];
            putinitclient(pl, p);
        }
    }

    // mostly the original welcomepacket(), with the bits that set up a connecting client removed
    int welcomepacket(packetbuf &p)
    {
        putint(p, N_WELCOME);
        putint(p, N_MAPCHANGE);
        sendstring(clientmap, p);
        putint(p, gamemode);
        putint(p, 0); // notgotitems = false
        putint(p, N_TIMEUP);
        putint(p, intermission ? 0 : max((maplimit - lastmillis)/1000, 1));
        putint(p, N_ITEMLIST);
        loopv(entities::ents) if(entities::ents[i]->spawned())
        {
            putint(p, i);
            putint(p, entities::ents[i]->type);
        }
        putint(p, -1);
        bool hasmaster = false;
        if(mastermode != MM_OPEN)
        {
            putint(p, N_CURRENTMASTER);
            putint(p, mastermode);
            hasmaster = true;
        }
        loopv(players) if(players[i]->privilege >= PRIV_MASTER)
        {
            if(!hasmaster)
            {
                putint(p, N_CURRENTMASTER);
                putint(p, mastermode);
                hasmaster = true;
            }
            putint(p, players[i]->clientnum);
            putint(p, players[i]->privilege);
        }
        if(hasmaster) putint(p, -1);
        if(gamepaused)
        {
            putint(p, N_PAUSEGAME);
            putint(p, 1);
            putint(p, -1);
        }
        if(gamespeed != 100)
        {
            putint(p, N_GAMESPEED);
            putint(p, gamespeed);
            putint(p, -1);
        }
        if(m_teammode)
        {
            putint(p, N_TEAMINFO);
            enumerate(teaminfos, teaminfo, t,
                if(t.frags) { sendstring(t.team, p); putint(p, t.frags); }
            );
            sendstring("", p);
        }
        putint(p, N_RESUME);
        loopv(players)
        {
            fpsent *pl = players[i];
            putint(p, pl->clientnum);
            putint(p, pl->state);
            putint(p, pl->frags);
            putint(p, pl->flags);
            putint(p, pl->deaths);
            putint(p, pl->quadmillis);

            sendstate(pl, p);
        }
        putint(p, -1);
        welcomeinitclient(p);
        if(cmode) cmode->initdemoclient(p);
        return 1;
    }

    bool demoinfomsg(packetbuf &p);

    void setupdemorecord()
    {
        if(!m_mp(gamemode) || m_collect || demoplayback || demorecord) return; // todo: collect init packet

        string tname;
        tname[0] = '\0';
        time_t t = time(NULL);
        size_t len = strftime(tname, sizeof(tname), "%Y-%m-%d_%H.%M.%S", localtime(&t));
        tname[min(len, sizeof(tname)-1)] = '\0';

        defformatstring(fname, "%s_%s_%s", tname, gamemodes[gamemode-STARTGAMEMODE].name, clientmap);
        filtertext(fname, fname, false);
        len = strlen(fname);
        if(len < 4 || strcasecmp(&fname[len-4], ".dmo")) concatstring(fname, ".dmo");

        if(const char *buf = server::getdemofile(fname, true)) demo = openrawfile(buf, "w+b");
        if(!demo) demo = openrawfile(fname, "w+b");
        if(!demo) return;

        stream *f = opengzfile(NULL, "wb", demo);
        if(!f) { DELETEP(demo); return; }

        conoutf("recording client demo");

        demonextmatch = false;
        demorecord = f;

        demoheader hdr;
        memcpy(hdr.magic, DEMO_MAGIC, sizeof(hdr.magic));
        hdr.version = DEMO_VERSION;
        hdr.protocol = PROTOCOL_VERSION;
        lilswap(&hdr.version, 2);
        demorecord->write(&hdr, sizeof(demoheader));

        demostartmillis = lastmillis;

        packetbuf p(MAXTRANS, ENET_PACKET_FLAG_RELIABLE);
        welcomepacket(p);
        demoinfomsg(p);
        recordpacket(1, p.buf, p.len);
    }

    void recordclientdemo(int val)
    {
        switch (val)
        {
            case 0: // stop recording
                enddemorecord();
                break;
            // case 1: // unused, legacy
            case 2: // start immediately
                setupdemorecord();
                break;
        }
    }
    ICOMMAND(recordclientdemo, "i", (int *val), recordclientdemo(*val));
    ICOMMAND(stopclientdemo, "", (), enddemorecord());

    // mostly just a copy of addmsg()
    // differences:
    //  - mcn defaults to player1
    //  - certain message types get wrapped in N_CLIENT packets (like the server would)
    //  - packets are written directly into the demo instead of the client's message buffer
    bool recordmsg(int type, const char *fmt, ...)
    {
        if(!connected || !demorecord) return false;
        static uchar buf[MAXTRANS];
        ucharbuf p(buf, sizeof(buf));
        putint(p, type);
        int numi = 1, numf = 0, nums = 0, mcn = player1->clientnum;
        bool reliable = false;
        if(fmt)
        {
            va_list args;
            va_start(args, fmt);
            while(*fmt) switch(*fmt++)
            {
                case 'r': reliable = true; break;
                case 'c':
                {
                    fpsent *d = va_arg(args, fpsent *);
                    if(d) mcn = d->clientnum;
                    break;
                }
                case 'v':
                {
                    int n = va_arg(args, int);
                    int *v = va_arg(args, int *);
                    loopi(n) putint(p, v[i]);
                    numi += n;
                    break;
                }

                case 'i':
                {
                    int n = isdigit(*fmt) ? *fmt++-'0' : 1;
                    loopi(n) putint(p, va_arg(args, int));
                    numi += n;
                    break;
                }
                case 'f':
                {
                    int n = isdigit(*fmt) ? *fmt++-'0' : 1;
                    loopi(n) putfloat(p, (float)va_arg(args, double));
                    numf += n;
                    break;
                }
                case 's': sendstring(va_arg(args, const char *), p); nums++; break;
            }
            va_end(args);
        }
        int num = nums || numf ? 0 : numi, msgsize = server::msgsizelookup(type);
        if(msgsize && num!=msgsize) { fatal("inconsistent msg size for %d (%d != %d)", type, num, msgsize); }
        switch(type)
        {
            case N_SWITCHNAME: case N_TEXT: case N_GUNSELECT:
            case N_SOUND: case N_CLIENTPING:
            {
                // must be wrapped in N_CLIENT
                static uchar cpbuf[MAXTRANS];
                ucharbuf q(cpbuf, sizeof(cpbuf));
                putint(q, N_CLIENT);
                putint(q, mcn);
                putuint(q, p.length());
                q.put(buf, p.length());
                recordpacket(1, cpbuf, q.length());
                q.reset();
                break;
            }
            default: recordpacket(reliable ? 1 : 0, buf, p.length()); break;
        }
        p.reset();
        return true;
    }
}
