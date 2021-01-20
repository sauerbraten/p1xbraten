#include "cube.h"
#include "p1xbraten_menus.h"

namespace game {
    struct embeddedfile<16060> menuscfg = {
        "data/p1xbraten/menus.cfg.gz",
        0x1607c7ba,
        {
            #include "../../data/p1xbraten/menus.cfg.gz.xxd"
        }
    };

    struct embeddedfile<1029> mastercfg = {
        "data/p1xbraten/master.cfg.gz",
        0xe6bf7248,
        {
            #include "../../data/p1xbraten/master.cfg.gz.xxd"
        }
    };

    struct embeddedfile<222> gamehudcfg = {
        "data/p1xbraten/gamehud.cfg.gz",
        0xac5cc9fd,
        {
            #include "../../data/p1xbraten/gamehud.cfg.gz.xxd"
        }
    };

    template<size_t N>
    int update(embeddedfile<N> *i)
    {
        stream *f = opengzfile(path(i->name), "rb");
        if(f)
        {
            uint fcrc = f->getcrc();
            delete f;
            if(fcrc && fcrc == i->crc) return 0; // no need to write
        }

        f = openrawfile(path(i->name), "wb");
        if(!f) return 2;
        f->write(i->contents, N);
        delete f;
        conoutf("updated %s", i->name);
        return 0;
    }

    extern int usep1xbratenmenus;

    void executep1xbratenmenufiles()
    {
        if(usep1xbratenmenus) execfile(menuscfg.name);
    }

    int writep1xbratenmenufiles()
    {
        if(!usep1xbratenmenus) return 1;

        int err = update(&menuscfg);
        if(err) { conoutf("\f6error updating %s (error code %d)!", menuscfg.name, err); return err; }

        err = update(&mastercfg);
        if(err) { conoutf("\f6error updating %s(error code %d)!", mastercfg.name, err); return err; }

        err = update(&gamehudcfg);
        if(err) { conoutf("\f6error updating %s(error code %d)!", gamehudcfg.name, err); return err; }

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
