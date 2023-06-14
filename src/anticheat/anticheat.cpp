#ifdef ANTICHEAT
/*
Integrates Epic's Online Services (EOS) SDK in order to provide protected anti-cheat sessions.

In order to use the Anti-Cheat service, we need to have a Platform handle first. Our main
use for the platform handle is to time communication between the SDK and Epic's backend.
(See TickBudgetInMilliseconds and anticheattick().)

Before we can create a Platform instance, we need to initialize the SDK. So when the engine
calls initializeanticheat(), that calls initializesdk() which in turn calls
initializeplatform() to set everything up.

Before using the Anti-Cheat service on the client, we need to have a Product User ID, i.e.
be logged in via the Connect service. triggeranticheatsession() takes care of this for us.

The Anti-Cheat service requires us to provide a mechanism for it to shuttle encrypted messages
between client and server. To do so, we have to register a "message to server" callback in
the client and a "message to client" callback in the server, which the SDK will call whenever
it wants data transmitted. (See *_AddNotify* functions.)
We then pass the message back to the SDK on the other side (see ReceiveMessageFrom* functions).

On top of that, the server needs to provide callbacks for "client's auth status changed" as
well as "client action required" (read "player ain't clean").

A server's anti-cheat session starts when it is launched and only ends when the process is
terminated/killed/whatever. A client's anti-cheat session starts when triggered by a server
and ends when the server says so or the client disconnects.

Regarding this file's code structure:
 - All functions in this file starting with "on" are callbacks and only ever called by the SDK.
 - SDK & Platform setup is the same on client and server and is defined first.
 - Inside the game namespace, first come Connect related things to log the user in, then anti-
   cheat callback functions, then all functions exposed via game.h, then the functions exposed
   via igame.h.
 - The server namespace is organized similarly, just without the Connect stuff at the top.
*/

#include "game.h"
#include "eos_sdk.h"
#include "eos_logging.h"
#include "decrypt_credentials.h"

EOS_HPlatform platform = NULL;

void initializeplatform(bool server = false)
{
    #include "credentials.cpp"

    static EOS_Platform_ClientCredentials creds;
    creds.ClientId = server ? serverId : clientId;
    creds.ClientSecret = server ? serverSecret : clientSecret;

    static EOS_Platform_Options platformoptions = {};
    platformoptions.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
    platformoptions.bIsServer = server ? EOS_TRUE : EOS_FALSE;
    platformoptions.EncryptionKey = NULL; // todo?
    platformoptions.Flags = EOS_PF_DISABLE_OVERLAY;
    platformoptions.CacheDirectory = NULL;
    platformoptions.ProductId = "36e0587a4c3544e8b635f7f55e7ccbfe";
    platformoptions.SandboxId = "7743e9c6960a42c7a32772a6d1b8af60";
    platformoptions.DeploymentId = "011295e7b04049d7978fcaa38d79bf86";
    platformoptions.TickBudgetInMilliseconds = 5;
    platformoptions.ClientCredentials = creds;
    platform = EOS_Platform_Create(&platformoptions);
}

void releaseplatform()
{
    // calling EOS_Platform_Release() makes the client hang for a second
    // on Linux and even longer on Windows, so we just don't...
    // if(platform) EOS_Platform_Release(platform);
}

void logeossdkmessage(const EOS_LogMessage *msg)
{
    int level = CON_INFO;
    string levelname = "[info]";
    switch(msg->Level)
    {
        case EOS_ELogLevel::EOS_LOG_Fatal: level = CON_ERROR; copystring(levelname, "[fatal]"); break;
        case EOS_ELogLevel::EOS_LOG_Error: level = CON_ERROR; copystring(levelname, "[error]"); break;
#ifdef DEBUG
        case EOS_ELogLevel::EOS_LOG_Warning: level = CON_WARN; copystring(levelname, "[warning]"); break;
        case EOS_ELogLevel::EOS_LOG_Info: break;
        default: copystring(levelname, "[debug]"); break;
#else
        default: return; // discard
#endif
    }
    conoutf(level, "\fs\f8[anti-cheat]\fr SDK: %s: %s: %s", levelname, msg->Category, msg->Message);
#ifndef STANDALONE
    static bool warned = false;
    if(!warned && level > CON_WARN)
    {
        conoutf(CON_WARN, "\fs\f8[anti-cheat]\fr \f3something is wrong with your client; it might not get recognized by anti-cheat servers\nplease contact p1x and provide this error log");
        warned = true;
    }
#endif
}

bool eossdkinitialized = false;

int initializesdk(bool server = false)
{
    static EOS_InitializeOptions sdkoptions = {};
    sdkoptions.ApiVersion = EOS_INITIALIZE_API_LATEST;
    sdkoptions.ProductName = "p1xbraten anti-cheat";
    sdkoptions.ProductVersion = "1.0";

    switch (EOS_EResult e = EOS_Initialize(&sdkoptions))
    {
        case EOS_EResult::EOS_Success: eossdkinitialized = true; break;
        case EOS_EResult::EOS_AlreadyConfigured: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr init: SDK is already configured"); return 1;
        case EOS_EResult::EOS_InvalidParameters: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr init: SDK options are invalid"); return 2;
        default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr unexpected return value %d from EOS_Initialize()", (int) e); return 3;
    }
#if defined(DEBUG) || defined(STANDALONE)
    EOS_ELogLevel loglevel = EOS_ELogLevel::EOS_LOG_VeryVerbose;
#else
    EOS_ELogLevel loglevel = EOS_ELogLevel::EOS_LOG_Error;
#endif

    switch (EOS_EResult e = EOS_Logging_SetLogLevel(EOS_ELogCategory::EOS_LC_ALL_CATEGORIES, loglevel))
    {
        case EOS_EResult::EOS_Success: break;
        case EOS_EResult::EOS_NotConfigured: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr setting log level: SDK not configured"); return 1;
        default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr unexpected return value %d from EOS_Logging_SetLogLevel()", (int) e); return 3;
    }

    switch (EOS_EResult e = EOS_Logging_SetCallback(logeossdkmessage))
    {
        case EOS_EResult::EOS_Success: break;
        case EOS_EResult::EOS_NotConfigured: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr setting log callback: SDK not configured"); return 1;
        default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr unexpected return value %d from EOS_Logging_SetCallback()", (int) e); return 3;
    }

    initializeplatform(server);
    return 0;
}

void shutdownsdk()
{
    releaseplatform();
    if(!eossdkinitialized) return;
    switch (EOS_EResult e = EOS_Shutdown())
    {
        case EOS_EResult::EOS_Success: eossdkinitialized = false; break;
        case EOS_EResult::EOS_NotConfigured: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr shutdown: SDK not configured"); return;
        case EOS_EResult::EOS_UnexpectedError: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr shutdown: SDK already shut down"); return;
        default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr unexpected return value %d from EOS_Shutdown()", (int) e); return;
    }
}

void anticheattick()
{
    if(platform) EOS_Platform_Tick(platform);
}

#ifndef STANDALONE

namespace game {

    EOS_ProductUserId eosuserid;

    void startanticheatsession();

    // called when EOS_Connect_CreateUser() completes (after login failed because the user is unknown to Epic)
    void oneosusercreated(const EOS_Connect_CreateUserCallbackInfo *data)
    {
        // conoutf("EOS_Connect_CreateUser() completed with code %d", (int) data->ResultCode);
        switch(EOS_EResult e = data->ResultCode)
        {
            case EOS_EResult::EOS_Success:
                eosuserid = data->LocalUserId;
                // conoutf("EOS_Connect_CreateUser() succeeded");
                startanticheatsession();
                break;
            default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr EOS_Connect_CreateUser() failed with result code %d", (int) e); break;
        }
    }

    EOS_HConnect eosconnect;

    // called when EOS_Connect_Login() completes
    void oneoslogincompleted(const EOS_Connect_LoginCallbackInfo *data)
    {
        // conoutf("EOS_Connect_Login() completed with code %d", (int) data->ResultCode);
        switch(EOS_EResult e = data->ResultCode)
        {
            case EOS_EResult::EOS_Success:
                eosuserid = data->LocalUserId;
                // conoutf("EOS_Connect_Login() succeeded");
                startanticheatsession();
                break;
            case EOS_EResult::EOS_InvalidUser: // user is new to Epic, must be created
            {
                EOS_Connect_CreateUserOptions connectopts;
                connectopts.ApiVersion = EOS_CONNECT_CREATEUSER_API_LATEST;
                connectopts.ContinuanceToken = data->ContinuanceToken;
                EOS_Connect_CreateUser(eosconnect, &connectopts, NULL, oneosusercreated);
                break;
            }
            default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr EOS_Connect_Login() failed with result code %d", (int) e); break;
        }
    }

    void eoslogin(char *eosname)
    {
        if(eosuserid) return;
        // conoutf("initializing EOS user");
        if(!eosconnect) eosconnect = EOS_Platform_GetConnectInterface(platform);
        if(!eosconnect) { conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr failed to get connect interface handle from SDK"); return; }

        // Epic's anti-cheat framework requires a stable identity to be assigned to each client. I use
        // the player's name and client number. In order for Epic to accept this "hack", I configured an
        // OpenID provider as identity provider for this "product" in Epic's dev portal.
        // Instead of a complete OpenID/OAuth implementation, just the so called "userinfo" endpoint is
        // required. I just deployed a server that echos back the "token" submitted in the Authorization
        // header as the user's ID.

        static EOS_Connect_Credentials creds;
        creds.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
        creds.Type = EOS_EExternalCredentialType::EOS_ECT_OPENID_ACCESS_TOKEN;
        creds.Token = eosname;

        static EOS_Connect_LoginOptions loginopts;
        loginopts.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
        loginopts.Credentials = &creds;
        loginopts.UserLoginInfo = NULL;

        EOS_Connect_Login(eosconnect, &loginopts, NULL, oneoslogincompleted);
    }

    #include <eos_anticheatclient.h>

    void onmessagetoserver(const EOS_AntiCheatClient_OnMessageToServerCallbackInfo *data)
    {
        static uchar buf[1024]; // EOS docs say message is up to 256 bytes, but I saw message sizes up to 512
        ucharbuf p(buf, sizeof(buf));
        putint(p, N_P1X_ANTICHEAT_MESSAGE);
        putuint(p, data->MessageDataSizeBytes);
        p.put((const uchar *) data->MessageData, data->MessageDataSizeBytes);
        messages.put(p.getbuf(), p.length());
        p.empty();
    }

    void onintegrityviolation(const EOS_AntiCheatClient_OnClientIntegrityViolatedCallbackInfo* data)
    {
        // conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr violation detected: %s (%d)", data->ViolationMessage, (int) data->ViolationType);
        // violation detected, but we won't tell the client that yet
        addmsg(N_P1X_ANTICHEAT_VIOLATION, "ris",  (int) data->ViolationType, data->ViolationMessage);
    }

    bool anticheatinitialized = false;
    EOS_HAntiCheatClient acc = NULL;
    bool anticheatsessionactive = false;

    bool anticheatready() { return eossdkinitialized; }

    void startanticheatsession()
    {
        static char useridstring[EOS_PRODUCTUSERID_MAX_LENGTH+1];
        int32_t useridstringlen = sizeof(useridstring);
        switch (EOS_EResult e = EOS_ProductUserId_ToString(eosuserid, useridstring, &useridstringlen))
        {
            case EOS_EResult::EOS_Success: /* OK */ break;
            case EOS_EResult::EOS_InvalidParameters: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr serializing EOS product user ID: input data invalid");  return;
            case EOS_EResult::EOS_InvalidUser: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr serializing EOS product user ID: user ID is invalid");        return;
            case EOS_EResult::EOS_LimitExceeded: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr serializing EOS product user ID: output buffer too short"); return;
            default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr EOS_ProductUserId_ToString() failed with result code %d", (int) e);                     return;
        }

        static EOS_AntiCheatClient_BeginSessionOptions sessionopts;
        sessionopts.ApiVersion = EOS_ANTICHEATSERVER_BEGINSESSION_API_LATEST;
        sessionopts.LocalUserId = eosuserid;
        sessionopts.Mode = EOS_EAntiCheatClientMode::EOS_ACCM_ClientServer;
        switch (EOS_EResult e = EOS_AntiCheatClient_BeginSession(acc, &sessionopts))
        {
            case EOS_EResult::EOS_Success: /* conoutf("\fs\f8[anti-cheat]\fr started session"); */ break;
            case EOS_EResult::EOS_InvalidParameters: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr starting session: input data invalid"); return;
            case EOS_EResult::EOS_AntiCheat_InvalidMode: /* conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr starting session: function not supported in current anti-cheat mode"); */ return;
            default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr EOS_AntiCheatClient_BeginSession() failed with result code %d", (int) e); return;
        }

        addmsg(N_P1X_ANTICHEAT_BEGINSESSION, "rs", useridstring);
        anticheatsessionactive = true;
    }

    void triggeranticheatsession()
    {
        // conoutf("\fs\f8[anti-cheat]\fr starting session");
        if(!anticheatinitialized || !acc)
        {
            conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr not initialized\r");
            conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr make sure you launch the game using the anticheat launcher");
            return;
        }
        if(eosuserid) startanticheatsession();
        else
        {
            // conoutf("\fs\f8[anti-cheat]\fr user not logged in, doing that now");
            defformatstring(eosname, "%s (%d)", player1->name, player1->clientnum);
            eoslogin(eosname);
        }
    }

    void receiveanticheatmessage(ucharbuf &p)
    {
        static EOS_AntiCheatClient_ReceiveMessageFromServerOptions messageopts;
        messageopts.ApiVersion = EOS_ANTICHEATCLIENT_RECEIVEMESSAGEFROMSERVER_API_LATEST;
        messageopts.DataLengthBytes = p.remaining();
        messageopts.Data = p.getbuf();

        if(!anticheatinitialized || !acc) { /*conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr received N_P1X_ANTICHEAT_MESSAGE but anti-cheat not initialized");*/ return; }

        switch (EOS_EResult e = EOS_AntiCheatClient_ReceiveMessageFromServer(acc, &messageopts))
        {
            case EOS_EResult::EOS_Success: break;
            case EOS_EResult::EOS_InvalidParameters: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr receiving message from server: input data invalid"); return;
            case EOS_EResult::EOS_AntiCheat_InvalidMode: /* conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr receiving message from server: function not supported in current mode"); */ return;
            default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr EOS_AntiCheatClient_ReceiveMessageFromServer() failed with result code %d", (int) e); return;
        }
    }

    bool endanticheatsession()
    {
        if(!acc || !anticheatsessionactive) return true;

        static EOS_AntiCheatClient_EndSessionOptions sessionopts;
        sessionopts.ApiVersion = EOS_ANTICHEATSERVER_ENDSESSION_API_LATEST;
        switch (EOS_EResult e = EOS_AntiCheatClient_EndSession(acc, &sessionopts))
        {
            case EOS_EResult::EOS_Success: break;
            case EOS_EResult::EOS_InvalidParameters: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr ending session: input data invalid"); return false;
            case EOS_EResult::EOS_AntiCheat_InvalidMode: /* conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr ending session: function not supported in current mode"); */ return false;
            default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr EOS_AntiCheatClient_EndSession() failed with result code %d", (int) e); return false;
        }
        anticheatsessionactive = false;
        return true;
    }

    EOS_NotificationId anticheatmessagetoservercallback;
    EOS_NotificationId anticheatintegrityviolationcallback;

    void initializeanticheat()
    {
        if(anticheatinitialized || (!eossdkinitialized && initializesdk())) return;

        acc = EOS_Platform_GetAntiCheatClientInterface(platform);

        // register c2s function for EOS messages
        static EOS_AntiCheatClient_AddNotifyMessageToServerOptions messagecallbackopts;
        messagecallbackopts.ApiVersion = EOS_ANTICHEATCLIENT_ADDNOTIFYMESSAGETOSERVER_API_LATEST;
        anticheatmessagetoservercallback = EOS_AntiCheatClient_AddNotifyMessageToServer(acc, &messagecallbackopts, NULL, onmessagetoserver);
        if(anticheatmessagetoservercallback==EOS_INVALID_NOTIFICATIONID)
        {
            conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr init: failed to register message-to-server callback");
            return;
        }

        // register callback for detected integrity violations
        static EOS_AntiCheatClient_AddNotifyClientIntegrityViolatedOptions violationcallbackopts;
        violationcallbackopts.ApiVersion = EOS_ANTICHEATCLIENT_ADDNOTIFYPEERAUTHSTATUSCHANGED_API_LATEST;
        anticheatintegrityviolationcallback = EOS_AntiCheatClient_AddNotifyClientIntegrityViolated(acc, &violationcallbackopts, NULL, onintegrityviolation);
        if(anticheatintegrityviolationcallback==EOS_INVALID_NOTIFICATIONID)
        {
            conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr init: failed to register integrity violation callback");
            return;
        }

        anticheatinitialized = true;
        conoutf("\fs\f8[anti-cheat]\fr initialized");
    }

    void shutdownanticheat()
    {
        if(!anticheatinitialized) return;
        // un-register callbacks
        if(acc)
        {
            if(anticheatintegrityviolationcallback!=EOS_INVALID_NOTIFICATIONID) EOS_AntiCheatClient_RemoveNotifyClientIntegrityViolated(acc, anticheatintegrityviolationcallback);
            if(anticheatmessagetoservercallback!=EOS_INVALID_NOTIFICATIONID) EOS_AntiCheatClient_RemoveNotifyMessageToServer(acc, anticheatmessagetoservercallback);
        }
        eosuserid = NULL;
        if(eossdkinitialized) shutdownsdk();
    }
}

#endif

namespace server {

    #include <eos_anticheatserver.h>

    bool anticheatinitialized = false;
    EOS_HAntiCheatServer acs = NULL;

    void onmessagetoclient(const EOS_AntiCheatCommon_OnMessageToClientCallbackInfo *data)
    {
        clientinfo *ci = (clientinfo *) data->ClientHandle;
        if(!ci) return;
        packetbuf p(300, ENET_PACKET_FLAG_RELIABLE); // EOS docs say message is up to 256 bytes
        putint(p, N_P1X_ANTICHEAT_MESSAGE);
        putuint(p, data->MessageDataSizeBytes);
        p.put((const uchar *) data->MessageData, data->MessageDataSizeBytes);
        ENetPacket *packet = p.finalize();
        sendpacket(ci->clientnum, 1, packet);
    }

    MOD(VARF, forceanticheatclient, 0, 0, 1, { loopv(clients) if(shouldspectate(clients[i])) forcespectator(clients[i]); });

    void onclientactionrequired(const EOS_AntiCheatCommon_OnClientActionRequiredCallbackInfo *data)
    {
        clientinfo *ci = (clientinfo *) data->ClientHandle;
        if(!ci)
        {
            conoutf("\fs\f8[anti-cheat]\fr action required: reason %d (details: %s)", (int) data->ActionReasonCode, data->ActionReasonDetailsString);
            return;
        }
        else conoutf("\fs\f8[anti-cheat]\fr action required: client %s, reason %d (details: %s)", colorname(ci), (int) data->ActionReasonCode, data->ActionReasonDetailsString);
        if(data->ClientAction==EOS_EAntiCheatCommonClientAction::EOS_ACCCA_RemovePlayer)
        {
            string msg;
            if(data->ActionReasonCode == EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_ClientViolation)
            {
                formatstring(msg, "\fs\f8[anti-cheat] \f3violation by %s: %s (code: %d)\fr",
                    colorname(ci), data->ActionReasonDetailsString, (int) data->ActionReasonCode
                );
            }
            else
            {
                formatstring(msg, "\fs\f8[anti-cheat] \f3%s should be removed from the game: %s (code: %d)\fr",
                    colorname(ci), data->ActionReasonDetailsString, (int) data->ActionReasonCode
                );
            }
            conoutf(CON_WARN, "%s", msg);
            if(!forceanticheatclient) notifyprivclients(PRIV_AUTH, msg);
            else
            {
                sendf(-1, 1, "ris", N_SERVMSG, msg);
                forcespectator(ci);
                formatstring(msg, "\fs\f8[anti-cheat] \f3forced %s to spectator\fr", colorname(ci));
            }
        }
    }

    static const char *authstatusname(int status)
    {
        static string asname[3] = {"unverified", "locally verified", "verified by Epic's backend"};
        return asname[status];
    }

    void onclientauthstatuschanged(const EOS_AntiCheatCommon_OnClientAuthStatusChangedCallbackInfo *data)
    {
        clientinfo *ci = (clientinfo *) data->ClientHandle;
        if(!ci) return;

        int oldanticheatverified = ci->anticheatverified;
        ci->anticheatverified = (int) data->ClientAuthStatus; // 0, 1, or 2; see authstatusname()

        if(oldanticheatverified > ci->anticheatverified)
        {
            if(forceanticheatclient) forcespectator(ci);
            defformatstring(msg, "\fs\f8[anti-cheat]\fr \f6%s had their auth status downgraded from %d (%s) to %d (%s)\fr",
                colorname(ci),
                oldanticheatverified, authstatusname(oldanticheatverified),
                ci->anticheatverified, authstatusname(ci->anticheatverified)
            );
            conoutf(CON_WARN, "%s", msg);
            notifyprivclients(PRIV_AUTH, msg);
            loopv(clients) if(clients[i]->supportsanticheat && clients[i]->privilege >= PRIV_AUTH)
                sendf(clients[i]->clientnum, 1, "ri3", N_P1X_ANTICHEAT_VERIFIED, ci->clientnum, 0);
        }

        if(oldanticheatverified < ci->anticheatverified)
        {
            defformatstring(msg, "\fs\f8[anti-cheat]\fr %s had their auth status upgraded from %d (%s) to %d (%s)",
                colorname(ci),
                oldanticheatverified, authstatusname(oldanticheatverified),
                ci->anticheatverified, authstatusname(ci->anticheatverified)
            );
            conoutf(CON_WARN, "%s", msg);
            if(ci->anticheatverified==2)
            {
                loopv(clients) if(clients[i]->supportsanticheat)
                    sendf(clients[i]->clientnum, 1, "ri3", N_P1X_ANTICHEAT_VERIFIED, ci->clientnum, 1);
                defformatstring(msg, "\fs\f8[anti-cheat]\fr %s is using the p1xbraten anti-cheat client", colorname(ci));
                sendf(-1, 1, "ris", N_SERVMSG, msg);
                if(ci->state.state==CS_SPECTATOR && mastermode<MM_LOCKED) unspectate(ci);
            }
        }
    }

    void probeforanticheatclient(packetbuf &p)
    {
        putint(p, N_SERVCMD);
        sendstring(CAP_PROBE_ANTICHEAT, p);
    }

    bool registeranticheatclient(clientinfo *ci, const char *useridstring)
    {
        conoutf("\fs\f8[anti-cheat]\fr registering %s (%d) with Epic's backend",  ci->name, ci->clientnum);
        EOS_ProductUserId eosuserid = EOS_ProductUserId_FromString(useridstring);

        static EOS_AntiCheatServer_RegisterClientOptions registeropts;
        registeropts.ApiVersion = EOS_ANTICHEATSERVER_REGISTERCLIENT_API_LATEST;
        registeropts.ClientHandle = (EOS_AntiCheatCommon_ClientHandle) ci;
        registeropts.ClientType = EOS_EAntiCheatCommonClientType::EOS_ACCCT_ProtectedClient;
        registeropts.UserId = eosuserid;
        registeropts.IpAddress = getclienthostname(ci->clientnum);

        switch (EOS_EResult e = EOS_AntiCheatServer_RegisterClient(acs, &registeropts))
        {
            case EOS_EResult::EOS_Success: return true;
            case EOS_EResult::EOS_InvalidParameters: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr registering client: input data invalid"); return false;
            default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr EOS_AntiCheatServer_RegisterClient() failed with result code %d", (int) e); return false;
        }
    }

    void receiveanticheatmessage(clientinfo *ci, ucharbuf &p)
    {
        static EOS_AntiCheatServer_ReceiveMessageFromClientOptions messageopts;
        messageopts.ApiVersion = EOS_ANTICHEATSERVER_RECEIVEMESSAGEFROMCLIENT_API_LATEST;
        messageopts.ClientHandle = (EOS_AntiCheatCommon_ClientHandle) ci;
        messageopts.DataLengthBytes = p.remaining();
        messageopts.Data = p.getbuf();
        if(!anticheatinitialized || !acs) { conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr received N_P1X_ANTICHEAT_MESSAGE but anti-cheat not initialized"); return; }
        switch (EOS_EResult e = EOS_AntiCheatServer_ReceiveMessageFromClient(acs, &messageopts))
        {
            case EOS_EResult::EOS_Success: break;
            case EOS_EResult::EOS_InvalidParameters: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr receiving message from client: input data invalid!"); return;
            default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr EOS_AntiCheatServer_ReceiveMessageFromClient() failed with result code %d", (int) e); return;
        }
    }

    void handleviolation(clientinfo *ci, int code, const char *details)
    {
        defformatstring(msg, "\fs\f3%s self-reported a violation: %s (code: %d)\fr", colorname(ci), details, code);
        conoutf(CON_WARN, "%s", msg);
        notifyprivclients(PRIV_AUTH, msg);
    }

    bool unregisteranticheatclient(clientinfo *ci)
    {
        static EOS_AntiCheatServer_UnregisterClientOptions unregisteropts;
        unregisteropts.ApiVersion = EOS_ANTICHEATSERVER_UNREGISTERCLIENT_API_LATEST;
        unregisteropts.ClientHandle = (EOS_AntiCheatCommon_ClientHandle) ci;

        switch (EOS_EResult e = EOS_AntiCheatServer_UnregisterClient(acs, &unregisteropts))
        {
            case EOS_EResult::EOS_Success: sendf(ci->clientnum, 1, "ri", N_P1X_ANTICHEAT_ENDSESSION); return true;
            case EOS_EResult::EOS_InvalidParameters: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr unregistering client: input data invalid"); return false;
            default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr EOS_AntiCheatServer_UnregisterClient() failed with result code %d", (int) e); return false;
        }
    }

    EOS_NotificationId anticheatmessagetoclientcallback;
    EOS_NotificationId anticheatclientactionrequiredcallback;
    EOS_NotificationId anticheatclientauthstatuschangedcallback;

    void startanticheatsession()
    {
        if(!acs) { conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr not initialized"); return; }
        static EOS_AntiCheatServer_BeginSessionOptions sessionopts;
        sessionopts.ApiVersion = EOS_ANTICHEATSERVER_BEGINSESSION_API_LATEST;
        sessionopts.RegisterTimeoutSeconds = 60;
        string servername;
        filtertext(servername, serverdesc);
        sessionopts.ServerName = servername;
        sessionopts.bEnableGameplayData = EOS_FALSE;
        sessionopts.LocalUserId = NULL;
        switch (EOS_EResult e = EOS_AntiCheatServer_BeginSession(acs, &sessionopts))
        {
            case EOS_EResult::EOS_Success: break;
            case EOS_EResult::EOS_InvalidParameters: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr starting session: input data invalid"); break;
            default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr EOS_AntiCheatServer_BeginSession() failed with result code %d", (int) e); break;
        }
    }

    void initializeanticheat()
    {
        if(anticheatinitialized || (!eossdkinitialized && initializesdk(true))) return;

        acs = EOS_Platform_GetAntiCheatServerInterface(platform);

        // register s2c function for EOS messages
        static EOS_AntiCheatServer_AddNotifyMessageToClientOptions messagecallbackopts;
        messagecallbackopts.ApiVersion = EOS_ANTICHEATSERVER_ADDNOTIFYMESSAGETOCLIENT_API_LATEST;
        anticheatmessagetoclientcallback = EOS_AntiCheatServer_AddNotifyMessageToClient(acs, &messagecallbackopts, NULL, onmessagetoclient);
        if(anticheatmessagetoclientcallback==EOS_INVALID_NOTIFICATIONID) conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr init: failed to register onmessagetoclient callback");

        // register callback for required actions
        static EOS_AntiCheatServer_AddNotifyClientActionRequiredOptions actionrequiredopts;
        actionrequiredopts.ApiVersion = EOS_ANTICHEATSERVER_ADDNOTIFYCLIENTACTIONREQUIRED_API_LATEST;
        anticheatclientactionrequiredcallback = EOS_AntiCheatServer_AddNotifyClientActionRequired(acs, &actionrequiredopts, NULL, onclientactionrequired);
        if(anticheatclientactionrequiredcallback==EOS_INVALID_NOTIFICATIONID) conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr init: failed to register onclientactionrequired callback");

        // register callback for user auth status change
        static EOS_AntiCheatServer_AddNotifyClientAuthStatusChangedOptions authchangedopts;
        authchangedopts.ApiVersion = EOS_ANTICHEATSERVER_ADDNOTIFYCLIENTAUTHSTATUSCHANGED_API_LATEST;
        anticheatclientauthstatuschangedcallback = EOS_AntiCheatServer_AddNotifyClientAuthStatusChanged(acs, &authchangedopts, NULL, onclientauthstatuschanged);
        if(anticheatclientauthstatuschangedcallback==EOS_INVALID_NOTIFICATIONID) conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr init: failed to register onclientauthstatuschanged callback");

        anticheatinitialized = true;
        startanticheatsession();
    }

    bool endanticheatsession()
    {
        if(!acs) return true;

        static EOS_AntiCheatServer_EndSessionOptions sessionopts;
        sessionopts.ApiVersion = EOS_ANTICHEATSERVER_ENDSESSION_API_LATEST;
        switch (EOS_EResult e = EOS_AntiCheatServer_EndSession(acs, &sessionopts))
        {
            case EOS_EResult::EOS_Success: return true;
            case EOS_EResult::EOS_InvalidParameters: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr ending session: input data invalid"); return false;
            default: conoutf(CON_ERROR, "\fs\f8[anti-cheat]\fr EOS_AntiCheatServer_EndSession() failed with result code %d", (int) e); return false;
        }
    }

    void shutdownanticheat()
    {
        if(!anticheatinitialized) return;
        endanticheatsession();
        if(acs)
        {
            // unregister callbacks
            if(anticheatmessagetoclientcallback!=EOS_INVALID_NOTIFICATIONID) EOS_AntiCheatServer_RemoveNotifyMessageToClient(acs, anticheatmessagetoclientcallback);
            if(anticheatclientactionrequiredcallback!=EOS_INVALID_NOTIFICATIONID) EOS_AntiCheatServer_RemoveNotifyClientActionRequired(acs, anticheatclientactionrequiredcallback);
            if(anticheatclientauthstatuschangedcallback!=EOS_INVALID_NOTIFICATIONID) EOS_AntiCheatServer_RemoveNotifyClientAuthStatusChanged(acs, anticheatclientauthstatuschangedcallback);
        }
        if(eossdkinitialized) shutdownsdk();
    }
}

#endif
