#include "lobster/stdafx.h"
#include "lobster/compiler.h"

using namespace lobster;

NativeRegistry nfr;

bool lobster_init(const char *maindir, const char *userdir)
{
    RegisterCoreLanguageBuiltins(nfr);
    auto loader = EnginePreInit(nfr);
    return InitPlatform(maindir, userdir, false, loader);
}

void lobster_run(const char *source, bool stringiscode)
{
    string_view fn = stringiscode ? "string" : source;
    int runtime_checks = RUNTIME_ASSERT;
    std::string bytecode_buffer;
    Compile(
        nfr,
        fn,
        stringiscode ? source : string_view(),
        bytecode_buffer,
        nullptr,
        nullptr,
        false,
        runtime_checks
    );
    std::string error;
    auto ret = RunTCC(
        nfr,
        bytecode_buffer,
        fn,
        nullptr,
        {},
        TraceMode::OFF,
        false,
        error,
        runtime_checks,
        false
    );
}

#include "cube.h"

void lobster_register_ivar(const char *name, ident *id, const char *help)
{
    // nfr(name, "", "", "I", help,
    // [](lobster::StackPtr &, lobster::VM &) {
    //     return lobster::Value(getvar(name));
    // });

    // string settername = "set" + string(name);
    // string setterhelp = "Sets the value of the "+string(name)+" variable.";

    // nfr("set"#name, "nval", "I", "B", "Sets the value of the "#name" variable.",
    // [](lobster::StackPtr &, lobster::VM &, lobster::Value &nval) {
    //     ident *id = idents.access(name);
    //     if(!id || id->type != ID_VAR) return lobster::Value(false);
    //     setvarchecked(id, nval.ival());
    //     return lobster::Value(true);
    // });
}