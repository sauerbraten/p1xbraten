#include "engine.h"

MOD(SVARP, p1xbratenversion, "");

void lockversion()
{
    ident *id = getident("p1xbratenversion");
    if(!id) { conoutf(CON_ERROR, "p1xbratenversion is undefined!"); return; }
    if(!(id->flags&IDF_READONLY)) id->flags += IDF_READONLY;
}

void migratep1xbraten()
{
    const char *version = "<git-dev>"; // replaced in CI build
    if(naturalsort(p1xbratenversion, version) == -1)
    {
        // we're newer, run migrations
        if(naturalsort(p1xbratenversion, "3.0.0") == -1) {
            // activate CON_NONZEN in all consoles
            if(!(confilter&(1<<14))) confilter += 1<<14;
            if(!(fullconfilter&(1<<14))) fullconfilter += 1<<14;
            if(!(miniconfilter&(1<<14))) miniconfilter += 1<<14;
        }
    }
    setsvar("p1xbratenversion", version);
    lockversion();
    conoutf("p1xbraten version: %s", version);
}
