#!/bin/bash

productId="36e0587a4c3544e8b635f7f55e7ccbfe"

function createHashCatalogue() {
    local binDir=$1
    local configFile=$2
    # clean up hash catalogue
    rm -rf "$binDir/EasyAntiCheat/Certificates"
    pushd integritytool
    echo ./anticheat_integritytool -productid "$productId" -target_game_dir "../$binDir" "$configFile"
    ./anticheat_integritytool -productid "$productId" -target_game_dir "../$binDir" "$configFile"
    popd
}

function pack() {
    local platform=$1
    local target="$platform"
    shift
    if [[ "0" != "$#" ]]; then
        target=$1
    fi

    # make binaries
    make $target

    # create hash catalogue
    local binDir="bin_unix"
    local configFile="linux.cfg"
    case "$platform" in
        windows)
            binDir="bin64"
            configFile="windows.cfg"
            ;;
        macos)
            binDir="sauerbraten.app/Contents/MacOS"
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
            local version=$(git describe)
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
            rm -rf "dist/$platform.zip"
            zip -r "dist/$platform.zip" "${files[@]}"
            ;;
    esac
}

platform=$1

case "$platform" in
    linux)
        pack "$platform" "debian"
        ;;
    windows | macos)
        pack "$platform"
        ;;
    all)
        pack "linux" "debian"
        pack "windows"
        pack "macos"
        ;;
    *)
        echo "unknown platform $platform"
        ;;
esac
