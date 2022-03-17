#include "game.h"

namespace game
{
    char *completeword = NULL;               // substring of commandbuf/s to be completed
    const char *lastcompletealphanum = NULL; // points to alphanum version of last suggested ci->name
    size_t lastcompletelen = 0;              // strlen of last suggested ci->name
    extern int playersearch;                 // same threshold as parseplayer()

    void complete(char *s, int cursor, int maxlen) // completes client names
    {
        if(!completeword && cursor>-1 && s[cursor]!=' ') return; // only start suggesting when followed by space

        static string alphanumword = ""; // alphanum version of completeword
        static size_t comparelen = 0;       // strlen of alphanumword
        if(!completeword) // true on first Tab press
        {
            char prevchar;
            if(cursor>=0) { prevchar = s[cursor]; s[cursor] = 0; } // temporarily shorten s to end at cursor
            completeword = strrchr(s, ' ');
            if(!completeword) completeword = s;     // no space in front of cursor -> use whole commandbuf
            else completeword++;                    // move to first char, behind the space we found
            lastcompletelen = strlen(completeword); // we will replace this many chars with our first suggestion
            filternonalphanum(alphanumword, completeword, maxlen); // used for matching
            comparelen = strlen(alphanumword);                     // used for matching
            if(cursor>=0) { s[cursor] = prevchar; } // restore s
        }

        const char *nextcomplete = NULL, *nextcompletealphanum = NULL; // will contain ci->name / ci->alphanumname of next suggestion
        static bool skipprefixcheck = false; // to remember wether we are completing by substring yet
        if(!skipprefixcheck) loopv(clients) // try matching prefix (ignoring case)
        {
            fpsent *ci = clients[i];
            if(!ci) continue;
            if(cubecaseequal(ci->alphanumname, alphanumword, comparelen) &&                         // match prefix
               (!lastcompletealphanum || cubecasecmp(ci->alphanumname, lastcompletealphanum) > 0) && // ensure it (alphabetically) comes after last suggestion
               (!nextcompletealphanum || cubecasecmp(ci->alphanumname, nextcompletealphanum) < 0)    // only pick as next suggestion when it comes before current pick
            )
            {
                nextcompletealphanum = ci->alphanumname;
                nextcomplete = ci->name;
            }
        }
        if(!skipprefixcheck) lastcompletealphanum = NULL;
        if(!nextcomplete && (int)comparelen>=playersearch) loopv(clients) // only if prefix matching didn't produce a new suggestion, and enough chars given: try matching substring (ignoring case)
        {
            fpsent *ci = clients[i];
            if(!ci) continue;
            if(
                cubecasefind(ci->alphanumname, alphanumword) &&                                      // match substring
                (!lastcompletealphanum || cubecasecmp(ci->alphanumname, lastcompletealphanum) > 0) && // ensure it (alphabetically) comes after last suggestion
                (!nextcompletealphanum || cubecasecmp(ci->alphanumname, nextcompletealphanum) < 0)    // only pick as next suggestion when it comes before current pick
            )
            {
                nextcompletealphanum = ci->alphanumname;
                nextcomplete = ci->name;
                skipprefixcheck = true; // next suggestion should also come from substring (not prefix) matching
            }
        }
        lastcompletealphanum = NULL;
        if(!nextcomplete) skipprefixcheck = false; // nothing found -> start prefix matching again
        if(nextcomplete)
        {
            size_t nextcompletelen = strlen(nextcomplete);
            size_t suffixlen = strlen(completeword+lastcompletelen); // length of text after completeword
            size_t prefixlen = completeword-s;
            if(suffixlen>0) memmove(completeword+nextcompletelen, completeword+lastcompletelen, min(suffixlen, maxlen-prefixlen)); // make sure we don't overwrite text after completeword
            memcpy(completeword, nextcomplete, min(nextcompletelen, maxlen-prefixlen)); // copy suggestion into commandbuf
            s[prefixlen+nextcompletelen+suffixlen] = 0;
            lastcompletealphanum = nextcompletealphanum;
            lastcompletelen = nextcompletelen;
        }
    }

    void resetcomplete() { completeword = NULL; lastcompletealphanum = NULL; }

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
