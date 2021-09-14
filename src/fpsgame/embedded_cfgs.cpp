#include "cube.h"
#include "embedded_cfgs.h"

namespace game {
    struct embeddedfile<16139> menuscfg = {
        "data/p1xbraten/menus.cfg.gz",
        0xf109a603,
        {
            #include "../../data/p1xbraten/menus.cfg.gz.xxd"
        }
    };

    struct embeddedfile<1042> mastercfg = {
        "data/p1xbraten/master.cfg.gz",
        0x629e73bb,
        {
            #include "../../data/p1xbraten/master.cfg.gz.xxd"
        }
    };

    struct embeddedfile<538> gamehudcfg = {
        "data/p1xbraten/gamehud.cfg.gz",
        0xbdf9b4e3,
        {
            #include "../../data/p1xbraten/gamehud.cfg.gz.xxd"
        }
    };

    struct embeddedfile<843> keymapcfg = {
        "data/p1xbraten/keymap.cfg.gz",
        0xdb3d498e,
        {
            #include "../../data/p1xbraten/keymap.cfg.gz.xxd"
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

    int writeembeddedcfgs()
    {
        int err = update(&menuscfg);
        if(err) { conoutf("\f6error updating %s (error code %d)!", menuscfg.name, err); return err; }

        err = update(&mastercfg);
        if(err) { conoutf("\f6error updating %s(error code %d)!", mastercfg.name, err); return err; }

        err = update(&gamehudcfg);
        if(err) { conoutf("\f6error updating %s(error code %d)!", gamehudcfg.name, err); return err; }

        err = update(&keymapcfg);
        if(err) { conoutf("\f6error updating %s(error code %d)!", keymapcfg.name, err); return err; }

        return 0;
    }

    MOD(VARFP, usep1xbratenmenus, 0, 1, 1, {
        if(usep1xbratenmenus) execfile(menuscfg.name);
        else execfile("data/menus.cfg");
    });
}
