#include "masterserver.h"

enum { PRIV_NONE = 0, PRIV_MASTER, PRIV_AUTH, PRIV_ADMIN };

struct authserver : masterserver {
    string name; // = key domain
    int privilege = PRIV_NONE;

    void notifygameconnected()
    {
        server::authserverconnected(name);
    }

    void notifygamedisconnected()
    {
        server::authserverdisconnected(name);
    }

    bool notifygameinput(const char *input, int cmdlen, const char *args)
    {
        if(!name[0] && masterserver::notifygameinput(input, cmdlen, args)) return true; // master registration, gbans
        server::processauthserverinput(name, input, cmdlen, args);
        return true;
    }
};

extern authserver *master;
extern hashnameset<authserver> authservers;