#include "engine.h"

SVARP(p1xbratenversion, "");

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
    }
    setsvar("p1xbratenversion", version);
    lockversion();
    conoutf("p1xbraten version: %s", version);
}
