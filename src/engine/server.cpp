// server.cpp: little more than enhanced multicaster
// runs dedicated or as client coroutine

#include "engine.h"

#define LOGSTRLEN 512

static FILE *logfile = NULL;

void closelogfile()
{
    if(logfile)
    {
        fclose(logfile);
        logfile = NULL;
    }
}

FILE *getlogfile()
{
#ifdef WIN32
    return logfile;
#else
    return logfile ? logfile : stdout;
#endif
}

void setlogfile(const char *fname)
{
    closelogfile();
    if(fname && fname[0])
    {
        fname = findfile(fname, "w");
        if(fname) logfile = fopen(fname, "w");
    }
    FILE *f = getlogfile();
    if(f) setvbuf(f, NULL, _IOLBF, BUFSIZ);
}

void logoutf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    logoutfv(fmt, args);
    va_end(args);
}

time_t walltime = 0;
MOD(VARP, logtime, 0, 0, 1);

static void writelog(FILE *file, const char *buf)
{
    static uchar ubuf[512];
    size_t len = strlen(buf), carry = 0;
    if(isdedicatedserver() && logtime)
    {
        if(!walltime) { walltime = time(NULL); walltime -= totalsecs; if(!walltime) walltime++; }
        time_t walloffset = walltime + totalsecs;
        struct tm *localvals = localtime(&walloffset);
        static string ts;
        if(localvals)
        {
            int tslen = strftime(ts, sizeof(ts), "[%Y-%m-%d %H:%M:%S] ", localvals);
            if(tslen) fwrite(ts, 1, tslen, file);
        }
    }
    while(carry < len)
    {
        size_t numu = encodeutf8(ubuf, sizeof(ubuf)-1, &((const uchar *)buf)[carry], len - carry, &carry);
        if(carry >= len) ubuf[numu++] = '\n';
        fwrite(ubuf, 1, numu, file);
    }
}

static void writelogv(FILE *file, const char *fmt, va_list args)
{
    static char buf[LOGSTRLEN];
    vformatstring(buf, fmt, args, sizeof(buf));
    writelog(file, buf);
}
 
#ifdef STANDALONE
void fatal(const char *fmt, ...) 
{ 
    void cleanupserver();
    cleanupserver(); 
	defvformatstring(msg,fmt,fmt);
	if(logfile) logoutf("%s", msg);
#ifdef WIN32
	MessageBox(NULL, msg, "Cube 2: Sauerbraten fatal error", MB_OK|MB_SYSTEMMODAL);
#else
    fprintf(stderr, "server error: %s\n", msg);
#endif
    closelogfile();
    exit(EXIT_FAILURE); 
}

void conoutfv(int type, const char *fmt, va_list args)
{
    logoutfv(fmt, args);
}
#endif

#define DEFAULTCLIENTS 8

enum { ST_EMPTY, ST_LOCAL, ST_TCPIP };

struct client                   // server side version of "dynent" type
{
    int type;
    int num;
    ENetPeer *peer;
    string hostname;
    void *info;
    ENetAddress real; // set by proxy via setip remote command
};

vector<client *> clients;

ENetHost *serverhost = NULL;
int laststatus = 0; 
ENetSocket pongsock = ENET_SOCKET_NULL, lansock = ENET_SOCKET_NULL;

int localclients = 0, nonlocalclients = 0;

bool hasnonlocalclients() { return nonlocalclients!=0; }
bool haslocalclients() { return localclients!=0; }

client &addclient(int type)
{
    client *c = NULL;
    loopv(clients) if(clients[i]->type==ST_EMPTY)
    {
        c = clients[i];
        break;
    }
    if(!c)
    {
        c = new client;
        c->num = clients.length();
        clients.add(c);
    }
    c->info = server::newclientinfo();
    c->type = type;
    switch(type)
    {
        case ST_TCPIP: nonlocalclients++; break;
        case ST_LOCAL: localclients++; break;
    }
    c->real.host = 0; c->real.port = 0;
    return *c;
}

void delclient(client *c)
{
    if(!c) return;
    switch(c->type)
    {
        case ST_TCPIP: nonlocalclients--; if(c->peer) c->peer->data = NULL; break;
        case ST_LOCAL: localclients--; break;
        case ST_EMPTY: return;
    }
    c->type = ST_EMPTY;
    c->peer = NULL;
    if(c->info)
    {
        server::deleteclientinfo(c->info);
        c->info = NULL;
    }
}

void cleanupserver()
{
#ifdef ANTICHEAT
    server::shutdownanticheat();
#endif
    if(serverhost) enet_host_destroy(serverhost);
    serverhost = NULL;

    if(pongsock != ENET_SOCKET_NULL) enet_socket_destroy(pongsock);
    if(lansock != ENET_SOCKET_NULL) enet_socket_destroy(lansock);
    pongsock = lansock = ENET_SOCKET_NULL;
}

VARF(maxclients, 0, DEFAULTCLIENTS, MAXCLIENTS, { if(!maxclients) maxclients = DEFAULTCLIENTS; });
VARF(maxdupclients, 0, 0, MAXCLIENTS, { if(serverhost) serverhost->duplicatePeers = maxdupclients ? maxdupclients : MAXCLIENTS; });

void process(ENetPacket *packet, int sender, int chan);
//void disconnect_client(int n, int reason);

int getservermtu() { return serverhost ? serverhost->mtu : -1; }
void *getclientinfo(int i) { return !clients.inrange(i) || clients[i]->type==ST_EMPTY ? NULL : clients[i]->info; }
ENetPeer *getclientpeer(int i) { return clients.inrange(i) && clients[i]->type==ST_TCPIP ? clients[i]->peer : NULL; }
int getnumclients()        { return clients.length(); }
uint getclientip(int n)
{
    if(!clients.inrange(n) || clients[n]->type!=ST_TCPIP) return 0;
    return clients[n]->real.host ? clients[n]->real.host : clients[n]->peer->address.host;
}
int setclientrealip(int n, uint ip)
{
    if(!clients.inrange(n)) return -1;
    clients[n]->real.host = ip;
    return enet_address_get_host_ip(&clients[n]->real, clients[n]->hostname, strlen("xxx.xxx.xxx.xxx")+1);
}
const char *getclienthostname(int n) { return clients.inrange(n) && clients[n]->type==ST_TCPIP ? clients[n]->hostname : NULL; }
ICOMMAND(getip, "i", (int *cn), result(getclienthostname(*cn)));

void sendpacket(int n, int chan, ENetPacket *packet, int exclude)
{
    if(n<0)
    {
        server::recordpacket(chan, packet->data, packet->dataLength);
        loopv(clients) if(i!=exclude && server::allowbroadcast(i)) sendpacket(i, chan, packet);
        return;
    }
    if(clients.inrange(n)) switch(clients[n]->type)
    {
        case ST_TCPIP:
        {
            enet_peer_send(clients[n]->peer, chan, packet);
            break;
        }

#ifndef STANDALONE
        case ST_LOCAL:
            localservertoclient(chan, packet);
            break;
#endif
    }
}

ENetPacket *sendf(int cn, int chan, const char *format, ...)
{
    int exclude = -1;
    bool reliable = false;
    if(*format=='r') { reliable = true; ++format; }
    packetbuf p(MAXTRANS, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
    va_list args;
    va_start(args, format);
    while(*format) switch(*format++)
    {
        case 'x':
            exclude = va_arg(args, int);
            break;

        case 'v':
        {
            int n = va_arg(args, int);
            int *v = va_arg(args, int *);
            loopi(n) putint(p, v[i]);
            break;
        }

        case 'i': 
        {
            int n = isdigit(*format) ? *format++-'0' : 1;
            loopi(n) putint(p, va_arg(args, int));
            break;
        }
        case 'f':
        {
            int n = isdigit(*format) ? *format++-'0' : 1;
            loopi(n) putfloat(p, (float)va_arg(args, double));
            break;
        }
        case 's': sendstring(va_arg(args, const char *), p); break;
        case 'm':
        {
            int n = va_arg(args, int);
            p.put(va_arg(args, uchar *), n);
            break;
        }
    }
    va_end(args);
    ENetPacket *packet = p.finalize();
    sendpacket(cn, chan, packet, exclude);
    return packet->referenceCount > 0 ? packet : NULL;
}

ENetPacket *sendfile(int cn, int chan, stream *file, const char *format, ...)
{
    if(cn < 0)
    {
#ifdef STANDALONE
        return NULL;
#endif
    }
    else if(!clients.inrange(cn)) return NULL;

    int len = (int)min(file->size(), stream::offset(INT_MAX));
    if(len <= 0 || len > 16<<20) return NULL;

    packetbuf p(MAXTRANS+len, ENET_PACKET_FLAG_RELIABLE);
    va_list args;
    va_start(args, format);
    while(*format) switch(*format++)
    {
        case 'i':
        {
            int n = isdigit(*format) ? *format++-'0' : 1;
            loopi(n) putint(p, va_arg(args, int));
            break;
        }
        case 's': sendstring(va_arg(args, const char *), p); break;
        case 'l': putint(p, len); break;
    }
    va_end(args);

    file->seek(0, SEEK_SET);
    file->read(p.subbuf(len).buf, len);

    ENetPacket *packet = p.finalize();
    if(cn >= 0) sendpacket(cn, chan, packet, -1);
#ifndef STANDALONE
    else sendclientpacket(packet, chan);
#endif
    return packet->referenceCount > 0 ? packet : NULL;
}

const char *disconnectreason(int reason)
{
    switch(reason)
    {
        case DISC_EOP: return "end of packet";
        case DISC_LOCAL: return "server is in local mode";
        case DISC_KICK: return "kicked/banned";
        case DISC_MSGERR: return "message error";
        case DISC_IPBAN: return "ip is banned";
        case DISC_PRIVATE: return "server is in private mode";
        case DISC_MAXCLIENTS: return "server FULL";
        case DISC_TIMEOUT: return "connection timed out";
        case DISC_OVERFLOW: return "overflow";
        case DISC_PASSWORD: return "invalid password";
        default: return NULL;
    }
}

void disconnect_client(int n, int reason)
{
    if(!clients.inrange(n) || clients[n]->type!=ST_TCPIP) return;
    enet_peer_disconnect(clients[n]->peer, reason);
    server::clientdisconnect(n);
    delclient(clients[n]);
    const char *msg = disconnectreason(reason);
    string s;
    if(msg) formatstring(s, "client %s (cn %d) disconnected because: %s", clients[n]->hostname, n, msg);
    else formatstring(s, "client %s (cn %d) disconnected", clients[n]->hostname, n);
    logoutf("%s", s);
    server::sendservmsg(s);
}

void kicknonlocalclients(int reason)
{
    loopv(clients) if(clients[i]->type==ST_TCPIP) disconnect_client(i, reason);
}

void process(ENetPacket *packet, int sender, int chan)   // sender may be -1
{
    packetbuf p(packet);
    server::parsepacket(sender, chan, p);
    if(p.overread()) { disconnect_client(sender, DISC_EOP); return; }
}

void localclienttoserver(int chan, ENetPacket *packet)
{
    client *c = NULL;
    loopv(clients) if(clients[i]->type==ST_LOCAL) { c = clients[i]; break; }
    if(c) process(packet, c->num, chan);
}

vector<masterserver *> masterlikeservers; // used for socket servicing; contains master and additional auth servers
authserver *master;
SVARF(mastername, server::defaultmaster(), {
    if(!master) return;
    master->disconnect();
    copystring(master->hostname, mastername);
});
VARF(masterport, 1, server::masterport(), 0xFFFF, {
    if(!master) return;
    master->disconnect();
    master->port = masterport;
});
hashnameset<authserver> authservers;

static ENetAddress pongaddr;

void sendserverinforeply(ucharbuf &p)
{
    ENetBuffer buf;
    buf.data = p.buf;
    buf.dataLength = p.length();
    enet_socket_send(pongsock, &pongaddr, &buf, 1);
}

#define MAXPINGDATA 32

void checkserversockets()        // reply all server info requests
{
    static ENetSocketSet readset, writeset;
    ENET_SOCKETSET_EMPTY(readset);
    ENET_SOCKETSET_EMPTY(writeset);
    ENetSocket maxsock = pongsock;
    ENET_SOCKETSET_ADD(readset, pongsock);
    loopv(masterlikeservers)
    {
        masterserver *m = masterlikeservers[i];
        if(m->sock != ENET_SOCKET_NULL)
        {
            maxsock = max(maxsock, m->sock);
            ENET_SOCKETSET_ADD(readset, m->sock);
            if(!m->connected) ENET_SOCKETSET_ADD(writeset, m->sock);
    }
    }
    if(lansock != ENET_SOCKET_NULL)
    {
        maxsock = max(maxsock, lansock);
        ENET_SOCKETSET_ADD(readset, lansock);
    }
    if(enet_socketset_select(maxsock, &readset, &writeset, 0) <= 0) return;

    ENetBuffer buf;
    uchar pong[MAXTRANS];
    loopi(2)
    {
        ENetSocket sock = i ? lansock : pongsock;
        if(sock == ENET_SOCKET_NULL || !ENET_SOCKETSET_CHECK(readset, sock)) continue;

        buf.data = pong;
        buf.dataLength = sizeof(pong);
        int len = enet_socket_receive(sock, &pongaddr, &buf, 1);
        if(len < 0 || len > MAXPINGDATA) continue;
        ucharbuf req(pong, len), p(pong, sizeof(pong));
        p.len += len;
        server::serverinforeply(req, p);
    }

    loopv(masterlikeservers)
    {
        masterserver *m = masterlikeservers[i];
        if(m->sock != ENET_SOCKET_NULL)
        {
            if(!m->connected)
            { 
                if(ENET_SOCKETSET_CHECK(readset, m->sock) || ENET_SOCKETSET_CHECK(writeset, m->sock))
                {
                int error = 0;
                    if(enet_socket_get_option(m->sock, ENET_SOCKOPT_ERROR, &error) < 0 || error)
                {
                        logoutf("could not connect to %s:%d (before flushinput), error: %d", m->hostname, m->port, error);
                        m->disconnect();
                }
                else
                {
                        m->connecting = 0;
                        m->connected = totalmillis ? totalmillis : 1;
                        m->notifygameconnected();
                }
            }
        }
            if(ENET_SOCKETSET_CHECK(readset, m->sock)) m->flushinput();
    }
}
}

VAR(serveruprate, 0, 0, INT_MAX);
SVAR(serverip, "");
VARF(serverport, 0, server::serverport(), 0xFFFF-1, { if(!serverport) serverport = server::serverport(); });

#ifdef STANDALONE
int curtime = 0, lastmillis = 0, elapsedtime = 0, totalmillis = 0;
#endif

VARN(updatemaster, allowupdatemaster, 0, 1, 1);

void updatemasterserver()
{
    if(!master->connected && master->lastconnect && totalmillis-master->lastconnect <= 5*60*1000) return;
    if(master->hostname[0] && allowupdatemaster) master->requestf("regserv %d\n", serverport);
    master->lastupdate = totalmillis ? totalmillis : 1;
}

uint totalsecs = 0;

void updatetime()
{
    static int lastsec = 0;
    if(totalmillis - lastsec >= 1000) 
    {
        int cursecs = (totalmillis - lastsec) / 1000;
        totalsecs += cursecs;
        lastsec += cursecs * 1000;
    }
}

void serverslice(bool dedicated, uint timeout)   // main server update, called from main loop in sp, or from below in dedicated server
{
#ifdef ANTICHEAT
    anticheattick();
#endif

    if(!serverhost) 
    {
        server::serverupdate();
        server::sendpackets();
        return;
    }
       
    // below is network only

    if(dedicated) 
    {
        int millis = (int)enet_time_get();
        elapsedtime = millis - totalmillis;
        static int timeerr = 0;
        int scaledtime = server::scaletime(elapsedtime) + timeerr;
        curtime = scaledtime/100;
        timeerr = scaledtime%100;
        if(!server::ispaused()) lastmillis += curtime;
        totalmillis = millis;
        updatetime();
    }
    server::serverupdate();

    loopv(masterlikeservers) masterlikeservers[i]->flushoutput();

    checkserversockets();

    if(!master->lastupdate || totalmillis-master->lastupdate>60*60*1000)       // send alive signal to master every hour of uptime
        updatemasterserver();
    
    if(totalmillis-laststatus>60*1000)   // display bandwidth stats, useful for server ops
    {
        laststatus = totalmillis;     
        if(nonlocalclients || serverhost->totalSentData || serverhost->totalReceivedData) logoutf("status: %d remote clients, %.1f send, %.1f rec (K/sec)", nonlocalclients, serverhost->totalSentData/60.0f/1024, serverhost->totalReceivedData/60.0f/1024);
        serverhost->totalSentData = serverhost->totalReceivedData = 0;
    }

    ENetEvent event;
    bool serviced = false;
    while(!serviced)
    {
        if(enet_host_check_events(serverhost, &event) <= 0)
        {
            if(enet_host_service(serverhost, &event, timeout) <= 0) break;
            serviced = true;
        }
        switch(event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                client &c = addclient(ST_TCPIP);
                c.peer = event.peer;
                c.peer->data = &c;
                string hn;
                copystring(c.hostname, (enet_address_get_host_ip(&c.peer->address, hn, sizeof(hn))==0) ? hn : "unknown");
                logoutf("connected: %s (cn %d)", c.hostname, c.num);
                int reason = server::clientconnect(c.num, c.peer->address.host);
                if(reason) disconnect_client(c.num, reason);
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                client *c = (client *)event.peer->data;
                if(c) process(event.packet, c->num, event.channelID);
                if(event.packet->referenceCount==0) enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT: 
            {
                client *c = (client *)event.peer->data;
                if(!c) break;
                logoutf("disconnected: %s (cn %d)", c->hostname, c->num);
                server::clientdisconnect(c->num);
                delclient(c);
                break;
            }
            default:
                break;
        }
    }
    if(server::sendpackets()) enet_host_flush(serverhost);
}

void flushserver(bool force)
{
    if(server::sendpackets(force) && serverhost) enet_host_flush(serverhost);
}

#ifndef STANDALONE
void localdisconnect(bool cleanup)
{
    bool disconnected = false;
    loopv(clients) if(clients[i]->type==ST_LOCAL) 
    {
        server::localdisconnect(i);
        delclient(clients[i]);
        disconnected = true;
    }
    if(!disconnected) return;
    game::gamedisconnect(cleanup);
    mainmenu = 1;
}

void localconnect()
{
    if(initing) return;
    client &c = addclient(ST_LOCAL);
    copystring(c.hostname, "local");
    game::gameconnect(false);
    server::localconnect(c.num);
}
#endif

#ifdef WIN32
#include "shellapi.h"

#define IDI_ICON1 1

static string apptip = "";
static HINSTANCE appinstance = NULL;
static ATOM wndclass = 0;
static HWND appwindow = NULL, conwindow = NULL;
static HICON appicon = NULL;
static HMENU appmenu = NULL;
static HANDLE outhandle = NULL;
static const int MAXLOGLINES = 200;
struct logline { int len; char buf[LOGSTRLEN]; };
static queue<logline, MAXLOGLINES> loglines;

static void cleanupsystemtray()
{
    NOTIFYICONDATA nid;
    memset(&nid, 0, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = appwindow;
    nid.uID = IDI_ICON1;
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

static bool setupsystemtray(UINT uCallbackMessage)
{
	NOTIFYICONDATA nid;
	memset(&nid, 0, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = appwindow;
	nid.uID = IDI_ICON1;
	nid.uCallbackMessage = uCallbackMessage;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.hIcon = appicon;
	strcpy(nid.szTip, apptip);
	if(Shell_NotifyIcon(NIM_ADD, &nid) != TRUE)
        return false;
    atexit(cleanupsystemtray);
    return true;
}

#if 0
static bool modifysystemtray()
{
	NOTIFYICONDATA nid;
	memset(&nid, 0, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = appwindow;
	nid.uID = IDI_ICON1;
	nid.uFlags = NIF_TIP;
	strcpy(nid.szTip, apptip);
	return Shell_NotifyIcon(NIM_MODIFY, &nid) == TRUE;
}
#endif

static void cleanupwindow()
{
	if(!appinstance) return;
	if(appmenu)
	{
		DestroyMenu(appmenu);
		appmenu = NULL;
	}
	if(wndclass)
	{
		UnregisterClass(MAKEINTATOM(wndclass), appinstance);
		wndclass = 0;
	}
}

static BOOL WINAPI consolehandler(DWORD dwCtrlType)
{
    switch(dwCtrlType)
    {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
            exit(EXIT_SUCCESS);
            return TRUE;
    }
    return FALSE;
}

static void writeline(logline &line)
{
    static uchar ubuf[512];
    size_t len = strlen(line.buf), carry = 0;
    while(carry < len)
    {
        size_t numu = encodeutf8(ubuf, sizeof(ubuf), &((uchar *)line.buf)[carry], len - carry, &carry);
        DWORD written = 0;
        WriteConsole(outhandle, ubuf, numu, &written, NULL);
    }     
}

static void setupconsole()
{
	if(conwindow) return;
    if(!AllocConsole()) return;
	SetConsoleCtrlHandler(consolehandler, TRUE);
	conwindow = GetConsoleWindow();
    SetConsoleTitle(apptip);
	//SendMessage(conwindow, WM_SETICON, ICON_SMALL, (LPARAM)appicon);
	SendMessage(conwindow, WM_SETICON, ICON_BIG, (LPARAM)appicon);
    outhandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    GetConsoleScreenBufferInfo(outhandle, &coninfo);
    coninfo.dwSize.Y = MAXLOGLINES;
    SetConsoleScreenBufferSize(outhandle, coninfo.dwSize);
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    loopv(loglines) writeline(loglines[i]);
}

enum
{
	MENU_OPENCONSOLE = 0,
	MENU_SHOWCONSOLE,
	MENU_HIDECONSOLE,
	MENU_EXIT
};

static LRESULT CALLBACK handlemessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_APP:
			SetForegroundWindow(hWnd);
			switch(lParam)
			{
				//case WM_MOUSEMOVE:
				//	break;
				case WM_LBUTTONUP:
				case WM_RBUTTONUP:
				{
					POINT pos;
					GetCursorPos(&pos);
					TrackPopupMenu(appmenu, TPM_CENTERALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON, pos.x, pos.y, 0, hWnd, NULL);
					PostMessage(hWnd, WM_NULL, 0, 0);
					break;
				}
			}
			return 0;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
                case MENU_OPENCONSOLE:
					setupconsole();
					if(conwindow) ModifyMenu(appmenu, 0, MF_BYPOSITION|MF_STRING, MENU_HIDECONSOLE, "Hide Console");
                    break;
				case MENU_SHOWCONSOLE:
					ShowWindow(conwindow, SW_SHOWNORMAL);
					ModifyMenu(appmenu, 0, MF_BYPOSITION|MF_STRING, MENU_HIDECONSOLE, "Hide Console"); 
					break;
				case MENU_HIDECONSOLE:
					ShowWindow(conwindow, SW_HIDE);
					ModifyMenu(appmenu, 0, MF_BYPOSITION|MF_STRING, MENU_SHOWCONSOLE, "Show Console");
					break;
				case MENU_EXIT:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
			}
			return 0;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static void setupwindow(const char *title)
{
	copystring(apptip, title);
	//appinstance = GetModuleHandle(NULL);
	if(!appinstance) fatal("failed getting application instance");
	appicon = LoadIcon(appinstance, MAKEINTRESOURCE(IDI_ICON1));//(HICON)LoadImage(appinstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
	if(!appicon) fatal("failed loading icon");

	appmenu = CreatePopupMenu();
	if(!appmenu) fatal("failed creating popup menu");
    AppendMenu(appmenu, MF_STRING, MENU_OPENCONSOLE, "Open Console");
    AppendMenu(appmenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(appmenu, MF_STRING, MENU_EXIT, "Exit");
	//SetMenuDefaultItem(appmenu, 0, FALSE);

	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	wc.hCursor = NULL; //LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = appicon;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = title;
	wc.style = 0;
	wc.hInstance = appinstance;
	wc.lpfnWndProc = handlemessages;
	wc.cbWndExtra = 0;
	wc.cbClsExtra = 0;
	wndclass = RegisterClass(&wc);
	if(!wndclass) fatal("failed registering window class");
	
	appwindow = CreateWindow(MAKEINTATOM(wndclass), title, 0, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, HWND_MESSAGE, NULL, appinstance, NULL);
	if(!appwindow) fatal("failed creating window");

	atexit(cleanupwindow);

    if(!setupsystemtray(WM_APP)) fatal("failed adding to system tray");
}

static char *parsecommandline(const char *src, vector<char *> &args)
{
    char *buf = new char[strlen(src) + 1], *dst = buf;
    for(;;)
    {
        while(isspace(*src)) src++;
        if(!*src) break;
        args.add(dst);
		for(bool quoted = false; *src && (quoted || !isspace(*src)); src++)
        {
            if(*src != '"') *dst++ = *src;
			else if(dst > buf && src[-1] == '\\') dst[-1] = '"';
			else quoted = !quoted;
		}
		*dst++ = '\0';
    }
    args.add(NULL);
    return buf;
}
                

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
    vector<char *> args;
    char *buf = parsecommandline(GetCommandLine(), args);
	appinstance = hInst;
#ifdef STANDALONE
    int standalonemain(int argc, char **argv);
    int status = standalonemain(args.length()-1, args.getbuf());
    #define main standalonemain
#else
    SDL_SetMainReady();
    int status = SDL_main(args.length()-1, args.getbuf());
#endif
    delete[] buf;
    exit(status);
    return 0;
}

void logoutfv(const char *fmt, va_list args)
{
    if(appwindow)
    {
        logline &line = loglines.add();
        vformatstring(line.buf, fmt, args, sizeof(line.buf));
        if(logfile) writelog(logfile, line.buf);
        line.len = min(strlen(line.buf), sizeof(line.buf)-2);
        line.buf[line.len++] = '\n';
        line.buf[line.len] = '\0';
        if(outhandle) writeline(line);
    }
    else if(logfile) writelogv(logfile, fmt, args);
}

#else

void logoutfv(const char *fmt, va_list args)
{
    FILE *f = getlogfile();
    if(f) writelogv(f, fmt, args);
}

#endif

static bool dedicatedserver = false;

bool isdedicatedserver() { return dedicatedserver; }

void rundedicatedserver()
{
    dedicatedserver = true;
    logoutf("dedicated server started, waiting for clients...");
#ifdef WIN32
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	for(;;)
	{
		MSG msg;
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT) exit(EXIT_SUCCESS);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		serverslice(true, 5);
	}
#else
    for(;;) serverslice(true, 5);
#endif
    dedicatedserver = false;
}

bool servererror(bool dedicated, const char *desc)
{
#ifndef STANDALONE
    if(!dedicated)
    {
        conoutf(CON_ERROR, "%s", desc);
        cleanupserver();
    }
    else
#endif
        fatal("%s", desc);
    return false;
}
  
ENetAddress serveraddress = { ENET_HOST_ANY, ENET_PORT_ANY };

bool setuplistenserver(bool dedicated)
{
    ENetAddress address = { ENET_HOST_ANY, enet_uint16(serverport <= 0 ? server::serverport() : serverport) };
    if(*serverip)
    {
        if(enet_address_set_host(&address, serverip)<0) conoutf(CON_WARN, "WARNING: server ip not resolved");
        else serveraddress.host = address.host;
    }
    serverhost = enet_host_create(&address, min(maxclients + server::reserveclients(), MAXCLIENTS), server::numchannels(), 0, serveruprate);
    if(!serverhost) return servererror(dedicated, "could not create server host");
    serverhost->duplicatePeers = maxdupclients ? maxdupclients : MAXCLIENTS;
    address.port = server::serverinfoport(serverport > 0 ? serverport : -1);
    pongsock = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
    if(pongsock != ENET_SOCKET_NULL && enet_socket_bind(pongsock, &address) < 0)
    {
        enet_socket_destroy(pongsock);
        pongsock = ENET_SOCKET_NULL;
    }
    if(pongsock == ENET_SOCKET_NULL) return servererror(dedicated, "could not create server info socket");
    else enet_socket_set_option(pongsock, ENET_SOCKOPT_NONBLOCK, 1);
    address.port = server::laninfoport();
    lansock = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
    if(lansock != ENET_SOCKET_NULL && (enet_socket_set_option(lansock, ENET_SOCKOPT_REUSEADDR, 1) < 0 || enet_socket_bind(lansock, &address) < 0))
    {
        enet_socket_destroy(lansock);
        lansock = ENET_SOCKET_NULL;
    }
    if(lansock == ENET_SOCKET_NULL) conoutf(CON_WARN, "WARNING: could not create LAN server info socket");
    else enet_socket_set_option(lansock, ENET_SOCKOPT_NONBLOCK, 1);
    if(!master && mastername && masterport)
    {
        master = &authservers[""];
        copystring(master->hostname, mastername);
        master->port = masterport;
        master->privilege = PRIV_AUTH;
        masterlikeservers.add(master);
    }
    return true;
}

#ifdef ANTICHEAT
MOD(VAR, anticheatenabled, 1, 0, 0);
#endif

void initserver(bool listen, bool dedicated)
{
    if(dedicated) 
    {
#ifdef WIN32
        setupwindow("Cube 2: Sauerbraten server");
#endif
        migratep1xbraten();
    }
    
    execfile("server-init.cfg", false);

    if(listen) setuplistenserver(dedicated);

    server::serverinit();

    if(listen)
    {
#ifdef ANTICHEAT
        if(anticheatenabled) server::initializeanticheat();
#endif
        dedicatedserver = dedicated;
        updatemasterserver();
        if(dedicated) rundedicatedserver(); // never returns
#ifndef STANDALONE
        else conoutf("listen server started");
#endif
    }
}

#ifndef STANDALONE
void startlistenserver(int *usemaster)
{
    if(serverhost) { conoutf(CON_ERROR, "listen server is already running"); return; }

    allowupdatemaster = *usemaster>0 ? 1 : 0;
 
    if(!setuplistenserver(false)) return;
    
    updatemasterserver();
   
    conoutf("listen server started for %d clients%s", maxclients, allowupdatemaster ? " and listed with master server" : ""); 
}
COMMAND(startlistenserver, "i");

void stoplistenserver()
{
    if(!serverhost) { conoutf(CON_ERROR, "listen server is not running"); return; }

    kicknonlocalclients();
    enet_host_flush(serverhost);
    cleanupserver();

    conoutf("listen server stopped");
}
COMMAND(stoplistenserver, "");
#endif

extern char *mastername;
bool serveroption(char *opt)
{
    switch(opt[1])
    {
        case 'u': setvar("serveruprate", atoi(opt+2)); return true;
        case 'c': setvar("maxclients", atoi(opt+2)); return true;
        case 'i': setsvar("serverip", opt+2); return true;
        case 'j': setvar("serverport", atoi(opt+2)); return true; 
        case 'm': setsvar("mastername", opt+2); setvar("updatemaster", mastername[0] ? 1 : 0); return true;
#ifdef STANDALONE
        case 'q': logoutf("Using home directory: %s", opt); sethomedir(opt+2); return true;
        case 'k': logoutf("Adding package directory: %s", opt); addpackagedir(opt+2); return true;
        case 'g': logoutf("Setting log file: %s", opt); setlogfile(opt+2); return true;
#ifdef ANTICHEAT
        case 'e': anticheatenabled = 1; return true;
#endif
#endif
        default: return false;
    }
}

vector<const char *> gameargs;

#ifdef STANDALONE
int main(int argc, char **argv)
{   
    setlogfile(NULL);
    if(enet_initialize()<0) fatal("Unable to initialise network module");
    atexit(enet_deinitialize);
    enet_time_set(0);
    for(int i = 1; i<argc; i++) if(argv[i][0]!='-' || !serveroption(argv[i])) gameargs.add(argv[i]);
    game::parseoptions(gameargs);
    initserver(true, true);
    return EXIT_SUCCESS;
}
#endif
