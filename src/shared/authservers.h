#include "cube.h"

extern ENetAddress serveraddress;

#ifndef STANDALONE
extern bool resolverwait(const char *name, ENetAddress *address);
#else
static inline bool resolverwait(const char *name, ENetAddress *address)
{
    return enet_address_set_host(address, name) >= 0;
}
#endif

enum { PRIV_NONE = 0, PRIV_MASTER, PRIV_AUTH, PRIV_ADMIN };

struct authserver {
    string name; // = key domain
    string hostname;
    int port, privilege;
    ENetAddress address;
    ENetSocket sock;
    int connecting, connected, lastconnect, lastupdate;
    vector<char> in, out;
    int inpos, outpos;

    authserver() : port(0), privilege(PRIV_NONE),
#ifdef __clang__
                   address((ENetAddress){ENET_HOST_ANY, ENET_PORT_ANY}),
#else
                   address({ENET_HOST_ANY, ENET_PORT_ANY}),
#endif
                   sock(ENET_SOCKET_NULL), connecting(0), connected(0), lastconnect(0), lastupdate(0), inpos(0), outpos(0)
    {
        name[0] = hostname[0] = 0;
    }

    void disconnect()
    {
        if(sock != ENET_SOCKET_NULL)
        {
            server::authserverdisconnected(name);
            enet_socket_destroy(sock);
            sock = ENET_SOCKET_NULL;
        }

        out.setsize(0);
        in.setsize(0);
        outpos = inpos = 0;

        address.host = ENET_HOST_ANY;
        address.port = ENET_PORT_ANY;

        lastupdate = connecting = connected = 0;
    }

    void connect()
    {
        if(sock!=ENET_SOCKET_NULL) return;
        if(!hostname[0]) return;
        if(address.host == ENET_HOST_ANY)
        {
            if(isdedicatedserver()) logoutf("looking up %s...", hostname);
            address.port = port;
            if(!resolverwait(hostname, &address)) return;
        }
        sock = enet_socket_create(ENET_SOCKET_TYPE_STREAM);
        if(sock == ENET_SOCKET_NULL)
        {
            if(isdedicatedserver()) name[0] ? logoutf("could not open socket for auth server %s", name) : logoutf("could not open master server socket");
            return;
        }
        if(serveraddress.host == ENET_HOST_ANY || !enet_socket_bind(sock, &serveraddress))
        {
            enet_socket_set_option(sock, ENET_SOCKOPT_NONBLOCK, 1);
            if(!enet_socket_connect(sock, &address)) return;
        }
        enet_socket_destroy(sock);
        if(isdedicatedserver()) name[0] ? logoutf("could not connect to auth server %s", name) : logoutf("could not connect to master server");
        sock = ENET_SOCKET_NULL;
        return;
    }

    bool request(const char *req)
    {
        if(sock == ENET_SOCKET_NULL)
        {
            connect();
            if(sock == ENET_SOCKET_NULL) return false;
            lastconnect = connecting = totalmillis ? totalmillis : 1;
        }

        if(out.length() >= 4096) return false;

        out.put(req, strlen(req));
        return true;
    }

    bool requestf(const char *fmt, ...)
    {
        defvformatstring(req, fmt, fmt);
        return request(req);
    }

    void processinput()
    {
        if(inpos >= in.length()) return;

        char *input = &in[inpos], *end = (char *)memchr(input, '\n', in.length() - inpos);
        while(end)
        {
            *end = '\0';

            const char *args = input;
            while(args < end && !iscubespace(*args)) args++;
            int cmdlen = args - input;
            while(args < end && iscubespace(*args)) args++;

            if(!name[0] && matchstring(input, cmdlen, "failreg"))
                conoutf(CON_ERROR, "master server registration failed: %s", args);
            else if(!name[0] && matchstring(input, cmdlen, "succreg"))
                conoutf("master server registration succeeded");
            else server::processauthserverinput(name, input, cmdlen, args);

            end++;
            inpos = end - in.getbuf();
            input = end;
            end = (char *)memchr(input, '\n', in.length() - inpos);
        }

        if(inpos >= in.length())
        {
            in.setsize(0);
            inpos = 0;
        }
    }

    void flushoutput()
    {
        if(connecting && totalmillis - connecting >= 60000)
        {
            name[0] ? logoutf("could not connect to auth server %s", name) : logoutf("could not connect to master server");
            disconnect();
        }
        if(out.empty() || !connected) return;

        ENetBuffer buf;
        buf.data = &out[outpos];
        buf.dataLength = out.length() - outpos;
        int sent = enet_socket_send(sock, NULL, &buf, 1);
        if(sent >= 0)
        {
            outpos += sent;
            if(outpos >= out.length())
            {
                out.setsize(0);
                outpos = 0;
            }
        }
        else disconnect();
    }

    void flushinput()
    {
        if(in.length() >= in.capacity())
            in.reserve(4096);

        ENetBuffer buf;
        buf.data = in.getbuf() + in.length();
        buf.dataLength = in.capacity() - in.length();
        int recv = enet_socket_receive(sock, NULL, &buf, 1);
        if(recv > 0)
        {
            in.advance(recv);
            processinput();
        }
        else disconnect();
    }
};

extern char *mastername;
extern int masterport;
extern authserver *masterserver;
extern hashnameset<authserver> authservers;
