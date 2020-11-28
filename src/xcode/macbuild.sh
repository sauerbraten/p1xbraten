#!/bin/sh
cd $(dirname "$0")
while [ -n "$1" ]; do
 case "$1" in
  clean)
   xcodebuild -project sauerbraten.xcodeproj clean -configuration Release
   ;;
  build)
   xcodebuild -project sauerbraten.xcodeproj -configuration Release -alltargets
   ;;
  install)
   cp -v build/Release/sauerbraten.app/Contents/MacOS/sauerbraten ../../sauerbraten.app/Contents/MacOS/sauerbraten_universal
   chmod +x ../../sauerbraten.app/Contents/MacOS/sauerbraten_universal
   ;;
  package)
   exec ./package.sh
   ;;
 esac
 shift
done
