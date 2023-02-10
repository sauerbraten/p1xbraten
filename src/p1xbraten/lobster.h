
// #include "lobster/stdafx.h"
#include "lobster/vmdata.h"

namespace lobster {

// #define L_VAR(name, body) nfr("get"#name, "", "", "I", "Returns the value of the "#name" variable.", \
//     [](lobster::StackPtr &, lobster::VM &) { \
//         ident *id = idents.access(name); \
//         if(!id || id->type != ID_VAR) return lobster::NilValue(); \
//         return lobster::Value(*id->storage.i); \
//     }); nfr("set"#name, "nval", "I", "B", "Sets the value of the "#name" variable.", \
//     [](lobster::StackPtr &, lobster::VM &, lobster::Value &nval) { \
//         ident *id = idents.access(name); \
//         if(!id || id->type != ID_VAR) return lobster::Value(false); \
//         setvarchecked(id, nval.ival()); \
//         return lobster::Value(true); \
//     });

#define L_VAR(name, body) nfr("get"#name, "", "", "I", "Returns the value of the "#name" variable.", \
    [](lobster::StackPtr &, lobster::VM &) { \
        return lobster::Value(getvar(#name)); \
    }); nfr("set"#name, "nval", "I", "B", "Sets the value of the "#name" variable.", \
    [](lobster::StackPtr &, lobster::VM &, lobster::Value &nval) { \
        ident *id = getident(#name); \
        if(!id) return; \
        setvarchecked(id, nval.ival()); \
    });

extern bool init(const char *maindir, const char *userdir);
extern void run(const char *source, bool stringiscode);
extern void register_ivar(const char *name);

}
