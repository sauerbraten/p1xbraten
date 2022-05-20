#!/bin/sh

cd $(dirname $0) # change into script directory in case it's called from somewhere else

# SAUER_OPTIONS contains any command line options you would like to start Sauerbraten with.
#SAUER_OPTIONS="-f"
SAUER_OPTIONS="-q${HOME}/.p1xbraten -k${HOME}/sauerbraten-code"

exec "./bin_unix/p1xbraten_x86_64_server" -e ${SAUER_OPTIONS} "$@"
