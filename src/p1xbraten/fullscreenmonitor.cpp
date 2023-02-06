#include "engine.h"

void setfullscreenmonitor(int);
VARF(fullscreenmonitor, 0, 0, 10, setfullscreenmonitor(fullscreenmonitor));

void setfullscreenmonitor(int monitor)
{
    if(!screen) return;

    int currentmonitor = SDL_GetWindowDisplayIndex(screen);
    if(fullscreenmonitor >= SDL_GetNumVideoDisplays())
    {
        fullscreenmonitor = currentmonitor;
        return;
    }

    extern int fullscreen;
    extern void resetgl();
    if(fullscreen && monitor!=currentmonitor) resetgl();
}

ICOMMAND(getnummonitors, "", (), intret(SDL_GetNumVideoDisplays()));
ICOMMAND(getmonitorname, "i", (int *id),
{
    const char *name = SDL_GetDisplayName(*id);
    result(name ? name : "");
});
