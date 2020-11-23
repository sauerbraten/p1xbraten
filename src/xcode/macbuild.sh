#!/bin/sh
while [ -n "$1" ]; do
 case "$1" in
  clean)
   xcodebuild -project src/xcode/sauerbraten.xcodeproj clean -configuration Release
   ;;
  all)
   xcodebuild -project src/xcode/sauerbraten.xcodeproj -configuration Release -alltargets
   ;;
  install)
   cp -v src/xcode/build/Release/sauerbraten.app/Contents/MacOS/sauerbraten sauerbraten.app/Contents/MacOS/sauerbraten_universal
   chmod +x sauerbraten.app/Contents/MacOS/sauerbraten_universal
   ;;
 esac
 shift
done
