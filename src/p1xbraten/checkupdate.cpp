#include "engine.h"
#include <curl/curl.h>

string latestversion;
ICOMMAND(latestversion, "", (), stringret(latestversion));

size_t extractlatestversion(char *buffer, size_t size, size_t nitems, void *userdata)
{
    static size_t l = strlen("location: ");
    if(nitems>l && !strncasecmp("location: ", buffer, l))
    {
        // search backwards for last slash
        int i = nitems-1; // nitems -> \n
        while(*(buffer+i) != '/') i--;
        copystring(latestversion, buffer+i+1, nitems-1-i);
    }
    return nitems * size;
}

void checkforp1xbratenupdate()
{
    CURL *curl = curl_easy_init();
    if(!curl) return;
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // HEAD request
    curl_easy_setopt(curl, CURLOPT_URL, "https://github.com/sauerbraten/p1xbraten/releases/latest");
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, extractlatestversion);
    if(curl_easy_perform(curl)) { curl_easy_cleanup(curl); return; }
    long status;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    curl_easy_cleanup(curl);
    if(status!=302L) return;
    if(naturalsort(p1xbratenversion, latestversion) == -1) conoutf("\f6There's a newer version of p1xbraten available: %s", latestversion);
}

ICOMMAND(checkupdate, "", (), checkforp1xbratenupdate());
