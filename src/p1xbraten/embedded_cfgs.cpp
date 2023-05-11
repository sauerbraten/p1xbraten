#include "cube.h"
#include "embedded_cfgs.h"

namespace game {
    struct embeddedfile<17256> menus_cfg = {
        "data/p1xbraten/menus.cfg.gz",
        0x0a3ea587,
        {
            #include "../../data/p1xbraten/menus.cfg.gz.xxd"
        }
    };

    struct embeddedfile<1121> master_cfg = {
        "data/p1xbraten/master.cfg.gz",
        0xc2ae1c43,
        {
            #include "../../data/p1xbraten/master.cfg.gz.xxd"
        }
    };

    struct embeddedfile<640> gamehud_cfg = {
        "data/p1xbraten/gamehud.cfg.gz",
        0x3cdb48db,
        {
            #include "../../data/p1xbraten/gamehud.cfg.gz.xxd"
        }
    };

    struct embeddedfile<838> keymap_cfg = {
        "data/p1xbraten/keymap.cfg.gz",
        0xfd4470e9,
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
        int err = update(&menus_cfg);
        if(err) { conoutf("\f6error updating %s (error code %d)!", menus_cfg.name, err); return err; }

        err = update(&master_cfg);
        if(err) { conoutf("\f6error updating %s (error code %d)!", master_cfg.name, err); return err; }

        err = update(&gamehud_cfg);
        if(err) { conoutf("\f6error updating %s (error code %d)!", gamehud_cfg.name, err); return err; }

        err = update(&keymap_cfg);
        if(err) { conoutf("\f6error updating %s (error code %d)!", keymap_cfg.name, err); return err; }

        return 0;
    }

    MOD(VARFP, usep1xbratenmenus, 0, 1, 1, {
        if(usep1xbratenmenus) execfile(menus_cfg.name);
        else execfile("data/menus.cfg");
    });
}
