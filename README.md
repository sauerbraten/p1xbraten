# p1xbraten

This repository contains the source for my client mod, as well as the patches applied to the vanilla source to get there.

## Patches

a.k.a. Features

### [moviehud.patch](./patches/moviehud.patch)

- adds `hidespecfollow` toggle: when 1, hides the "SPECTATOR" and player name in the lower right of the screen when spectating)
- adds `namesabovehead` toggle: when 0, hides the names above players' models (usually rendered as particle text), while keeping status icons for health boost, armor and quad

### [scoreboard.patch](./patches/scoreboard.patch)

- enables detailed per-weapon damage stats recording and cleans up the scoreboard look
- adds scoreboard toggles:
    - `showflags`: when 1, shows the number of flags scored by a player; always hidden in non-flag modes
    - `showkpd`: when 1, shows the players' frags/death ratio
    - `showaccuracy`: when 1, shows the players' overall accuracy
    - `showdamage`: when 1, shows the players' overall damage dealt; when 2, shows the players' overall net damage (= dealt - received); always hidden in insta modes

    ![ectf, duel, multiple teams](https://i.imgur.com/tS9FK1I.gif)

- adds damage-related cubescript commands:
    - `getdamagepotential`
    - `getdamagedealt`
    - `getdamagereceived`
    - `getdamagewasted` (= potential - dealt)
    - `getnetdamage` (= dealt - received)

    All of these commands (as well as `getaccuracy`) default to showing your own stats across all weapons. However, they all take two optional integer arguments to query by weapon and player: `/<cmd> [weapon] [cn]` (use -1 to query damage across all weapons)

    To use these new commands to show comed-like statistics in the game hud in the lower right corner, put the gamehud definition from [this file](./data/once.cfg) into your autoexec.cfg.

### [hudfragmessages.patch](./patches/hudfragmessages.patch)

- enables frag messages showing the weapon used to complete the frag (on by default)

    ![fragmessages](https://i.imgur.com/K4GL6oB.png)

- adds the following variables:
    - `hudfragmessages`: 0 disables all frag messages, 1 enables them for all modes, 2 enables them for non-insta modes only
    - `hudfragmessageduration`: how long each message will be shown, in milliseconds, between 100 (= 0.1s) and 10000 (= 10s)
    - `maxhudfragmessages`: how many messages to show at most (between 1 and 10)
    - `hudfragmessagex`: horizontal position (between 0.0 and 1.0) where messages will appear
    - `hudfragmessagey`: vertical position (between 0.0 and 1.0) where the newest message will appear; older messages will move outwards
    - `hudfragmessagescale`: size of the messages, between 0.0 and 1.0
    - `hudfragmessagefilter`: bitfield filter var (like confilter), e.g. 0x3800 shows all players' frags, suicides, and teamkills

        Until a GUI menu for hud frag messages exists, configure your normal console to show the frags you want to see as hud messages, then execute `/hudfragmessagefilter $confilter` to copy those settings for hud frag messages. You can then change your normal console filter back.
## Installation

The latest builds are always at https://github.com/sauerbraten/p1xbraten/releases/latest. *You do not need to download anything but the correct executable in order to run this client mod!*

### Windows

Download sauerbraten.exe from the link above and put it into bin64/ of your Sauerbraten installation.

### macOS

Download sauerbraten_universal from the link above and put it into /Applications/sauerbraten.app/Contents/MacOS/, then `chmod +x /Applications/sauerbraten.app/Contents/MacOS/sauerbraten_universal`.

### Linux

Download linux_64_client from the link above and put it into bin_unix/ inside of your Sauerbraten directory, then `chmod +x bin_unix/linux_64_client`.

## Building your own binary

*You don't have to do this if you already followed the installation instructions above and just want to play!*

On Linux and macOS, just run `make && make install` **inside the src/ directory** (given you installed the usual Sauerbraten dependencies). On Windows, open src/vcpp/sauerbraten.vcxproj with Visual Studio and build in there.

This will put the resulting binary into the usual place inside this repo. To use it, you have to copy it over to the same place in your actual Sauerbraten installation. On Linux, the [./start.sh](./start.sh) script will launch the new binary from inside this repository, using the Sauerbraten files in $SAUER_DIR and `~/.p1xbraten` as user data directory.

### Fresh upstream sources

On Linux and macOS, you can build my client using fresh vanilla sources. Set $SAUER_DIR to the path of your Sauerbraten directory (currently, an SVN checkout is required), then use `make` and `make install` in the repository root:

```
export SAUER_DIR=~/sauerbraten-code
make
make install
```

`make` will copy the src/ directory from $SAUER_DIR, apply all patches and run `make` inside src/; `make install` will run `make install` inside src/ and copy the contents of data/ to ~/.p1xbraten/data/.
