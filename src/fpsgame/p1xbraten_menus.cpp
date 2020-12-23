#include "cube.h"

namespace game {

    const char *menuscfgfname = "data/p1xbraten/menus.cfg.gz";
    const uchar menuscfggz[] = {
        #include "../../data/p1xbraten/menus.cfg.gz.xxd"
    };
    const size_t menuscfggzlen = 16051;
    const char *mastercfgfname = "data/p1xbraten/master.cfg.gz";
    const uchar mastercfggz[] = {
        #include "../../data/p1xbraten/master.cfg.gz.xxd"
    };
    const size_t mastercfggzlen = 1031;
    const char *gamehudcfgfname = "data/p1xbraten/gamehud.cfg.gz";
    const uchar gamehudcfggz[] = {
        #include "../../data/p1xbraten/gamehud.cfg.gz.xxd"
    };
    const size_t gamehudcfggzlen = 219;

    extern int usep1xbratenmenus;

    void executep1xbratenmenufiles()
    {
        if(usep1xbratenmenus) execfile(menuscfgfname);
    }

    int writep1xbratenmenufiles()
    {
        if(!usep1xbratenmenus) return 1;

        stream *f = openrawfile(menuscfgfname, "w");
        if(!f) return 2;
        f->write(menuscfggz, menuscfggzlen);
        delete f;

        f = openrawfile(mastercfgfname, "w");
        if(!f) return 2;
        f->write(mastercfggz, mastercfggzlen);
        delete f;

        f = openrawfile(gamehudcfgfname, "w");
        if(!f) return 2;
        f->write(gamehudcfggz, gamehudcfggzlen);
        delete f;

        return 0;
    }

    VARFP(usep1xbratenmenus, 0, 1, 1, {
        if(usep1xbratenmenus)
        {
            writep1xbratenmenufiles();
            executep1xbratenmenufiles();
        }
        else execfile("data/menus.cfg");
    });

}
