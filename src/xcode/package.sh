#!/bin/sh
cd $(dirname "$0")

SAUER_HOME="../.."

#ensure old package is gone
rm -f sauerbraten.dmg

echo creating temporary directory
#make the directory which our disk image will be made of
#use /tmp as a destination because copying ourself (xcode folder) whilst compiling causes the dog to chase its tail
SAUERPKG=`mktemp -d /tmp/sauerpkg.XXXXXX` || exit 1

#leave indicator of where temp directory is in case things break
ln -sf "$SAUERPKG" "build/sauerpkg"

echo copying package
#copy executable
ditto "$SAUER_HOME/sauerbraten.app" "$SAUERPKG/Sauerbraten.app"

GAMEDIR="$SAUERPKG/Sauerbraten.app/Contents/Resources"

#copy readme and data and remove unneccesary stuff
ditto "$SAUER_HOME/README.html" "$SAUERPKG/"
ditto "$SAUER_HOME/docs" "$SAUERPKG/docs"
ditto "$SAUER_HOME/data" "$GAMEDIR/data"
ditto "$SAUER_HOME/packages" "$GAMEDIR/packages"
ditto "$SAUER_HOME/server-init.cfg" "$GAMEDIR/"
ditto "$SAUER_HOME/src" "$SAUERPKG/src"
find -d "$SAUERPKG" -name ".svn" -exec rm -rf {} \;
find "$SAUERPKG" -name ".DS_Store" -exec rm -f {} \;
rm -rf "$SAUERPKG/src/xcode/build"
ln -sf /Applications "$SAUERPKG/Applications"

#finally make a disk image out of the stuff
echo creating dmg
hdiutil create -fs HFS+ -srcfolder "$SAUERPKG" -volname sauerbraten sauerbraten.dmg

echo cleaning up
#cleanup
rm -rf "$SAUERPKG"

echo done!

