#include "game.h"

UNUSED static int janetready = janet_init() ? 0 : 1;
JanetTable *janetenv = janet_core_env(NULL);

void janet_executeret(const char *code, tagval &result)
{
    forcenull(result);

    JanetBuffer *outbuf = janet_buffer(512), *errbuf = janet_buffer(512);
    janet_table_put(janetenv, janet_ckeywordv("err"), janet_wrap_buffer(errbuf));
    janet_table_put(janetenv, janet_ckeywordv("out"), janet_wrap_buffer(outbuf));

    Janet ret;

    int err = janet_dostring(janetenv, code, NULL, &ret);
    if(err) { conoutf(CON_ERROR, "janet: %s", janet_unwrap_string(ret)); return; }

    if(errbuf->count) conoutf(CON_WARN, "%s", janet_to_string(janet_wrap_buffer(errbuf)));
    if(outbuf->count) conoutf("%s", janet_to_string(janet_wrap_buffer(outbuf)));

    if     (janet_checktype(ret, JANET_STRING))  result.setstr(newstring((const char *)janet_unwrap_string(ret)));
    else if(janet_checktype(ret, JANET_SYMBOL))  result.setstr(newstring((const char *)janet_unwrap_symbol(ret)));
    else if(janet_checktype(ret, JANET_KEYWORD)) result.setstr(newstring((const char *)janet_unwrap_keyword(ret)));
    else if(janet_checktype(ret, JANET_NUMBER))  result.setfloat(janet_unwrap_number(ret));
    else if(janet_checktype(ret, JANET_BOOLEAN)) result.setint(janet_unwrap_boolean(ret));
    else conoutf(CON_ERROR, "unhandled janet type '%s'", janet_type_names[janet_type(ret)]);
}
ICOMMAND(dojanet, "s", (const char *code), janet_executeret(code, *commandret));

int janet_execute(const char *code)
{
    tagval result;
    janet_executeret(code, result);
    int i = result.getint();
    freearg(result);
    return i;
}
COMMANDN(execjanet, janet_execute, "s");

#ifndef STANDALONE
namespace game {

    static Janet _janet_binding_getname(int32_t argc, Janet *argv)
    {
        fpsent *c = getclient(janet_optinteger(argv, argc, 0, hudplayer()->clientnum));
        return janet_cstringv(colorname(c ? c : hudplayer()));
    }
    JANETFUN(getcolorclientname, _janet_binding_getname, "Returns the colored name of a client, defaulting to the player whose hud is on-screen.");

    extern const char *getclientname(int cn);
    IJANETFUN(getclientname, "Returns the name of a client, defaulting to the player whose hud is on-screen.",
    {
        int cn = janet_optinteger(argv, argc, 0, hudplayer()->clientnum);
        return janet_cstringv(getclientname(cn));
    });

}
#endif
