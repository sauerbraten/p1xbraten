#include "game.h"

namespace game
{
    int completesize = 0;
    char *lastcompletealphanum = NULL;

    void complete(char *s, int maxlen) // completes client names
    {
        char *word = strrchr(s, ' ');
        if(!word) word = s;
        else word++;
    
        static string alphanumword = "";
        filternonalphanum(alphanumword, word, maxlen); // make word in console comparable

        if(!completesize) { completesize = (int)strlen(word); DELETEA(lastcompletealphanum); }

        const char *nextcomplete = NULL, *nextcompletealphanum = NULL;
        loopv(clients)
        {
            fpsent *ci = clients[i];
            if(!ci) continue;
            if(strncasecmp(ci->alphanumname, alphanumword, completesize)==0 &&
               (!lastcompletealphanum || strcasecmp(ci->alphanumname, lastcompletealphanum) > 0) && (!nextcompletealphanum || strcasecmp(ci->alphanumname, nextcompletealphanum) < 0))
            {
                nextcompletealphanum = ci->alphanumname;
                nextcomplete = ci->name;
            }
        }
        DELETEA(lastcompletealphanum);
        if(nextcomplete)
        {
            copystring(word, nextcomplete, maxlen);
            lastcompletealphanum = newstring(nextcompletealphanum);
        }
    }

    void resetcomplete() { completesize = 0; }

    void filternonalphanum(char *dst, const char *src, size_t len)
    {
        for(int c = uchar(*src); c; c = uchar(*++src))
        {
            if(c == '\f')
            {
                if(!*++src) break;
                continue;
            }
            if(!iscubealnum(c)) continue;
            *dst++ = c;
            if(!--len) break;
        }
        *dst = '\0';
    }
}