#include "cube.h"

namespace game {

    string menuscfgfname = "data/p1xbraten/menus.cfg.gz";
    const uchar menuscfggz[] = {
        #include "../../data/p1xbraten/menus.cfg.gz.xxd"
    };

    const size_t menuscfggzlen = 16051;
    string mastercfgfname = "data/p1xbraten/master.cfg.gz";
    const uchar mastercfggz[] = {
        #include "../../data/p1xbraten/master.cfg.gz.xxd"
    };

    const size_t mastercfggzlen = 1031;
    string gamehudcfgfname = "data/p1xbraten/gamehud.cfg.gz";
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

        stream *f = openrawfile(path(menuscfgfname), "wb");
        if(!f) return 2;
        f->write(menuscfggz, menuscfggzlen);
        delete f;

        f = openrawfile(path(mastercfgfname), "wb");
        if(!f) return 2;
        f->write(mastercfggz, mastercfggzlen);
        delete f;

        f = openrawfile(path(gamehudcfgfname), "wb");
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
