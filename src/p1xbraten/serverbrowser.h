#include "engine.h"

struct pingattempts
{
    enum { MAXATTEMPTS = 2 };

    int offset, attempts[MAXATTEMPTS];

    pingattempts() : offset(0) { clearattempts(); }

    void clearattempts() { memset(attempts, 0, sizeof(attempts)); }

    void setoffset() { offset = 1 + rnd(0xFFFFFF); }

    int encodeping(int millis)
    {
        millis += offset;
        return millis ? millis : 1;
    }

    int decodeping(int val)
    {
        return val - offset;
    }

    int addattempt(int millis)
    {
        int val = encodeping(millis);
        loopk(MAXATTEMPTS-1) attempts[k+1] = attempts[k];
        attempts[0] = val;
        return val;
    }

    bool checkattempt(int val, bool del = true)
    {
        if(val) loopk(MAXATTEMPTS) if(attempts[k] == val)
        {
            if(del) attempts[k] = 0;
            return true;
        }
        return false;
    }

};

enum { UNRESOLVED = 0, RESOLVING, RESOLVED };

struct serverinfo : pingattempts
{
    enum
    {
        WAITING = INT_MAX,

        MAXPINGS = 3
    };

    string name, map, sdesc;
    int port, numplayers, resolved, ping, lastping, nextping;
    int pings[MAXPINGS];
    vector<int> attr;
    ENetAddress address;
    bool keep;
    const char *password;

    serverinfo()
        : port(-1), numplayers(0), resolved(UNRESOLVED), keep(false), password(NULL)
    {
        name[0] = map[0] = sdesc[0] = '\0';
        clearpings();
        setoffset();
    }

    ~serverinfo()
    {
        DELETEA(password);
    }

    void clearpings()
    {
        ping = WAITING;
        loopk(MAXPINGS) pings[k] = WAITING;
        nextping = 0;
        lastping = -1;
        clearattempts();
    }

    void cleanup()
    {
        clearpings();
        attr.setsize(0);
        numplayers = 0;
    }

    void reset()
    {
        lastping = -1;
    }

    void checkdecay(int decay)
    {
        if(lastping >= 0 && totalmillis - lastping >= decay)
            cleanup();
        if(lastping < 0) lastping = totalmillis;
    }

    void calcping()
    {
        int numpings = 0, totalpings = 0;
        loopk(MAXPINGS) if(pings[k] != WAITING) { totalpings += pings[k]; numpings++; }
        ping = numpings ? totalpings/numpings : WAITING;
    }

    void addping(int rtt, int millis)
    {
        if(millis >= lastping) lastping = -1;
        pings[nextping] = rtt;
        nextping = (nextping+1)%MAXPINGS;
        calcping();
    }

    static bool compare(serverinfo *a, serverinfo *b)
    {
        bool ac = server::servercompatible(a->name, a->sdesc, a->map, a->ping, a->attr, a->numplayers),
             bc = server::servercompatible(b->name, b->sdesc, b->map, b->ping, b->attr, b->numplayers);
        if(ac > bc) return true;
        if(bc > ac) return false;
        if(a->keep > b->keep) return true;
        if(a->keep < b->keep) return false;
        if(a->numplayers < b->numplayers) return false;
        if(a->numplayers > b->numplayers) return true;
        if(a->ping > b->ping) return false;
        if(a->ping < b->ping) return true;
        int cmp = strcmp(a->name, b->name);
        if(cmp != 0) return cmp < 0;
        if(a->port < b->port) return true;
        if(a->port > b->port) return false;
        return false;
    }
};
