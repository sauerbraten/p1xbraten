#!/bin/bash

productId="36e0587a4c3544e8b635f7f55e7ccbfe"

function createHashCatalogue() {
    local binDir=$1
    local configFile=$2
    # clean up hash catalogue
    rm -rf "$binDir/EasyAntiCheat/Certificates"
    pushd integritytool || exit
    echo ./anticheat_integritytool -productid "$productId" -target_game_dir "../$binDir" "$configFile"
    ./anticheat_integritytool -productid "$productId" -target_game_dir "../$binDir" "$configFile"
    popd || exit
}

function build() {
    local platform=$1
    local target="$platform"
    shift
    if [[ "0" != "$#" ]]; then
        target=$1
    fi

    # set version in source
    cp src/p1xbraten/version.cpp src/p1xbraten/version.cpp.orig
    local version=$(git describe)
    sed -i "s/<git-dev>/$version/" src/p1xbraten/version.cpp

    # make binaries
    make "$target" || exit

    # restore git-dev version
    rm src/p1xbraten/version.cpp
    mv src/p1xbraten/version.cpp.orig src/p1xbraten/version.cpp
}

function pack() {
    local platform=$1
    local version=$(git describe)

    # create hash catalogue
    local binDir="bin_unix"
    local configFile="linux.cfg"
    case "$platform" in
        windows)
            binDir="bin64"
            configFile="windows.cfg"
            ;;
        macos)
            binDir="bin_macos"
            configFile="macos.cfg"
            ;;
    esac
    createHashCatalogue $binDir $configFile

    # zip up files
    local files=(
        "bin_unix"
        "p1xbraten.sh"
        "anticheat.sh"
    )
    case "$platform" in
        windows)
            rm -f dist/p1xbraten_setup*
            cp src/windows/p1xbraten.nsi src/windows/p1xbraten.nsi.orig
            sed -i "s/<git-dev>/$version/" src/windows/p1xbraten.nsi
            makensis src/windows/p1xbraten.nsi
            mv p1xbraten_setup_* dist/
            rm src/windows/p1xbraten.nsi
            mv src/windows/p1xbraten.nsi.orig src/windows/p1xbraten.nsi
            ;; # break
        macos)
            files=(
                "sauerbraten.app/Contents/MacOS"
                "sauerbraten.app/Contents/Libraries"
            )
            ;& # fallthrough
        *) # macos & linux
            mkdir -p dist
            rm -rf dist/p1xbraten_"$platform"_*.zip
            zip -r dist/p1xbraten_"$platform"_"$version".zip "${files[@]}"
            ;;
    esac
}

platform=$1

case "$platform" in
    linux)
        build "$platform" "debian"
        pack "$platform"
        ;;
    windows)
        build "$platform"
        pack "$platform"
        ;;
    macos)
        build "$platform"
        pack "$platform"
        ;;
    all)
        build "linux" "debian"
        pack "linux"
        build "windows"
        pack "windows"
        build "macos"
        pack "macos"
        ;;
    *)
        echo "unknown platform $platform"
        ;;
esac
