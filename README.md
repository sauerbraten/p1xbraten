# p1xbraten

This repository contains the source for my client mod, as well as the patches applied to the vanilla source to get there.

## Installation

The latest builds are always at https://github.com/sauerbraten/p1xbraten/releases/latest.

### Windows

Put sauerbraten.exe into bin64/ of your Sauerbraten installation.

### macOS

Put sauerbraten_universal into sauerbraten.app/Contents/MacOS/, then `chmod +x sauerbraten.app/Contents/MacOS/sauerbraten_universal`.

### Linux

Put linux_64_client into bin_unix/, then `chmod +x bin_unix/linux_64_client`.

## Building your own binary

On Linux and macOS, just run `make && make install` **inside the src/ directory** (given you installed the usual Sauerbraten dependencies). On Linux, the [./start.sh](./start.sh) script will launch the new binary using the Sauerbraten files in $SAUER_DIR and `~/.p1xbraten` as user data directory.

On Windows, open src/vcpp/sauerbraten.vcxproj with Visual Studio and build in there.

### Fresh build

On Linux and macOS, you can easily build my client using fresh vanilla sources. Set $SAUER_DIR to the path of your Sauerbraten directory (currently, an SVN checkout is required), then use `make` and `make install` in the repository root:

```
export SAUER_DIR=~/sauerbraten-code
make
make install
```

`make` will copy the src/ directory from $SAUER_DIR, apply all patches and run `make` inside src/; `make install` will put the compiled binary into the usual place (depending on your OS).

## Patches

### [moviehud.patch](./patches/moviehud.patch)

- adds `hidespecfollow` toggle: when 1, hides the "SPECTATOR" and player name in the lower right of the screen when spectating)
- adds `namesabovehead` toggle: when 0, hides the names above players' models (usually rendered as particle text), while keeping status icons for health boost, armor and quad

### [weaponstats.patch](./patches/weaponstats.patch)

- enables detailed per-weapon damage stats recording
- adds damage-related cubescript commands:
    - `getdamagepotential`
    - `getdamagedealt`
    - `getdamagereceived`
    - `getdamagewasted` (= potential - dealt)
    - `getnetdamage` (= dealt - received)

    all of these commands (as well as `getaccuracy`) default to showing your own stats across all weapons. however, they all take two optional integer arguments to query by weapon and player: `/<cmd> [weapon] [cn]` (use -1 to query damage across all weapons)

- adds scoreboard toggles:
    - `showkpd`: when 1, shows the players' frags/death ratio
    - `showaccuracy`: when 1, shows the players' overall accuracy
    - `showdamage`: when 1, shows the players' overall damage dealt; when 2, shows the players' overall net damage (= dealt - received)
