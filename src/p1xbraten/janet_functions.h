#include "cube.h"
#include "janet.h"

extern JanetTable *janetenv;

static inline bool addjanetfunction(const char *name, JanetCFunction cfun, const char *docstring)
{
    Janet fun = janet_wrap_cfunction((void *)cfun);
    janet_def(janetenv, name, fun, docstring);
    janet_register(name, cfun);
    return false;
}

#define JANETFUN(name, global, docstring) UNUSED static bool __dummy_##global = addjanetfunction(#name, global, "(" #name ")\n\n" docstring);

#define IJANETFUN(name, docstring, b) \
    template<int N> struct _jfun_##name; \
    template<> struct _jfun_##name<__LINE__> { static bool init; static Janet run (int32_t argc, Janet *argv); }; \
    bool _jfun_##name<__LINE__>::init = addjanetfunction(#name, _jfun_##name<__LINE__>::run, "(" #name ")\n\n" docstring); \
    Janet _jfun_##name<__LINE__>::run(int32_t argc, Janet *argv) \
    { b; }

extern int janet_execute(const char *code);
