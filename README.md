# p1xbraten

This repository contains the source for my client mod, as well as the patches applied to the vanilla source to get there.

- [Patches](#patches)
  - [moviehud.patch](#moviehudpatch)
  - [scoreboard.patch](#scoreboardpatch)
  - [hudfragmessages.patch](#hudfragmessagespatch)
  - [fullconsole.patch](#fullconsolepatch)
  - [hudscore.patch](#hudscorepatch)
  - [serverbrowser.patch](#serverbrowserpatch)
  - [listteams.patch](#listteamspatch)
  - [tex_commands.patch](#tex_commandspatch)
  - [decouple_framedrawing.patch](#decouple_framedrawingpatch)
- [Installation](#installation)
  - [Windows](#windows)
  - [macOS](#macos)
  - [Linux](#linux)
- [Menu](#menu)
- [Building your own binary](#building-your-own-binary)
  - [Using fresh upstream sources](#using-fresh-upstream-sources)

## Patches

a.k.a. Features

### [moviehud.patch](./patches/moviehud.patch)

- adds `hidespecfollow` toggle: when 1, hides the "SPECTATOR" and player name in the lower right of the screen when spectating
- adds `namesabovehead` toggle: when 0, hides the names above players' models (usually rendered as particle text), while keeping status icons for health boost, armor and quad

### [scoreboard.patch](./patches/scoreboard.patch)

- enables suicide and detailed, per-weapon damage recording

  Note that suicide and damage recording is only accurate in games you observed from the beginning. When you join a running game, all previous damage is unknown, and suicides are only initialized from negative frag counts.

- cleans up the scoreboard look (for example, right-justifies numeric scoreboard columns)

- adds scoreboard toggles:
    - `showflags`: when 1, shows the number of flags scored by a player; always hidden in non-flag modes
    - `showsuicides`: when 1, shows how often each player suicided
    - `showkpd`: when 1, shows each player's frags/death ratio
    - `showaccuracy`: when 1, shows each player's overall accuracy
    - `showdamage`: when 1, shows each player's overall damage dealt; when 2, shows each player's overall net damage (= dealt - received); always hidden in insta modes
    - `showdamagereceived`: when 1, shows each player's damage received

  ![example screenshots](https://i.imgur.com/hsCY56E.gif)

- makes the following built-in cubescript commands return `hudplayer`'s stats instead of `player1`:
    - `getfrags`
    - `getflags`
    - `getdeaths`

  This allows you to see the stats of the player you're currently spectating in the gamehud instead of your own.

- adds damage-related cubescript commands:
    - `getdamagepotential`
    - `getdamagedealt`
    - `getdamagereceived`
    - `getdamagewasted` (= potential − dealt)
    - `getnetdamage` (= dealt − received)

  All of the new damage-related commands (as well as `getaccuracy`) default to showing your own (`hudplayer`'s) stats across all weapons. However, they all take two optional integer arguments to query by weapon and player: `/<cmd> [weapon] [cn]` (use -1 to query damage across all weapons)

  To show comed-like statistics in the game hud in the lower right corner, run `/exec data/p1xbraten/gamehud.cfg.gz`.

### [hudfragmessages.patch](./patches/hudfragmessages.patch)

- enables frag messages showing the weapon used to complete the frag (on by default)

  ![fragmessages](https://i.imgur.com/K4GL6oB.png)

- adds the following variables:
    - `hudfragmessages`: 0 disables all frag messages, 1 enables them for all modes, 2 enables them for non-insta modes only
    - `maxhudfragmessages`: how many messages to show at most (between 1 and 10)
    - `hudfragmessageduration`: how long each message will be shown, in seconds, between 0 and 10
    - `hudfragmessagefade`: 0 or 1: whether or not to fade out messages
    - `hudfragmessagex`: horizontal position (between 0.0 and 1.0) where messages will appear
    - `hudfragmessagey`: vertical position (between 0.0 and 1.0) where the newest message will appear
    - `hudfragmessagescale`: size of the messages, between 0.0 and 1.0
    - `hudfragmessagestackdir`: direction in which to stack old messages: -1 to stack upwards, 1 to stack downwards, 0 to stack towards the closes edge of the screen depending on `hudfragmessagey`
    - `hudfragmessagefilter`: bitfield filter var (like confilter), e.g. 0x3800 shows all players' frags, suicides, and teamkills

You can easily configure the hud frag messages using the [improved menu](#menu) (options -> hud -> adjust hud frag messages).

### [fullconsole.patch](./patches/fullconsole.patch)

- removes the guiskin background of fullconsole in favor of a gradient background on the left
- improves interplay of scoreboard and fullconsole
- `fullconsize` is deprecated and unused, instead the console always takes up all available space
- adds `fullconalpha` var: sets the opacity of fullconsole's left edge in percent (from 1 to 100)

### [hudscore.patch](./patches/hudscore.patch)

- hides hudscore when there is just one team/player

### [serverbrowser.patch](./patches/serverbrowser.patch)

- styles the serverbrowser to be consistent with the cleaned up scoreboard
- divides 'players' column into 'players' and 'slots'
- right-justifies ping, players, slots, and port columns

### [listteams.patch](./patches/listteams.patch)

- adds `listteams` command that returns a list of names of non-empty teams (according to filters) when two or more teams exist and an empty string otherwise (`listteams [includeself?] [includebots?] [includespecs?]`)
- extends `listclients` command to allow omitting spectators and only listing clients of a certain team (`listclients [includeself?] [includebots?] [includespecs?] [teamname?]`)

[The updated menu](#menu) includes a better master menu thanks to these changes:

![master menu](https://i.imgur.com/doC2IcB.png)

### [tex_commands.patch](./patches/tex_commands.patch)

- adds `settex <id>` command to apply a texture by ID
- adds `replacetex <old> <new> [insel?]` command to replace a texture with another by specifying their IDs

### [decouple_framedrawing.patch](./patches/decouple_framedrawing.patch)

- removes fps-induced limiting of the main loop (for example, input & network processing were affected by `maxfps`)
- introduce `maxtps` var to limit the main loop independently from `maxfps` (for example to save power): 0 to disable limiting (default), 100..1000 to set how many ticks per second are allowed; `maxfps` overrides `maxtps` if `maxfps > maxtps`

  These two changes allow using `maxfps` without compromising on the frequency of input polling and network event processing. In vanilla Sauerbraten, `maxfps` limits the whole game's main loop if it's set to any value other than 0. This patch removes that limiting behavior and instead processes network events and player input on every main loop iteration, and skips frame drawing until a new frame is required to reach the requests frame rate.

  While vanilla Sauerbraten is truly unlimited after `/maxfps 0`, p1xbraten's mainloop can also be limited by the `maxtps` variable. Setting `maxfps` to any value other than 0 may override `maxtps`, so that p1xbraten can prioritise frame draw timing: if `maxtps == 0`, p1xbraten will limit the main loop to 1000 ticks per second, or if `maxtps < maxfps`, the value of `maxfps` will be used. Set both `maxfps` and `maxtps` to 0 to get vanilla's truly unlimited behavior back.

  Using `maxfps` and `maxtps`, you can optimize for different goals:

  - for lowest average latency, use `/maxtps 0` and set `maxfps` to your screen refresh rate plus ~10% (to make sure there's always a new frame ready), for example `/maxfps 70`, then `/vsync 1`
  - to use fewer resources and save laptop battery, use `/maxfps 0`, `/maxtps 100` and `/vsync 1`
  - for lowest consistent latency, use `/maxfps 0`, `/vsync 0`, and set `maxtps` to the highest value that gives you a stable fps counter on your system (disable vsync to find out)

- adds Wayland detection

  When running on Wayland, p1xbraten ignores `maxfps` and makes use of frame callbacks, so a frame is only rendered when the compositor requests it. This way, no frames are drawn when p1xbraten is in the background, which saves energy and leaves resources to foreground tasks. Because of the decoupling of frame drawing from the main engine loop mentioned above, input and network events are still processed at the frequency dictated by `maxtps`, and you can set a lower `maxtps` value to limit resource use further.

  Because Wayland provides tear-free presentation out of the box, p1xbraten ignores `/vsync 1` when running on Wayland. The fps counter in p1xbraten will show the rate at which your Wayland compositor requests frames, which usually matches your display's refresh rate. Basically, Wayland takes care of vsync for you.

  Most Linux distributions that use Wayland will run Sauerbraten as an Xorg app through Xwayland. **Set `SDL_VIDEODRIVER=wayland` in your launch script** (like in my [start.sh](./start.sh#L62) to force SDL to use Wayland. You can check whether p1xbraten is running as a Wayland process using the `/wayland` variable.

## Installation

The latest builds are always at https://github.com/sauerbraten/p1xbraten/releases/latest.

*You do not need to download anything but the correct executable in order to run this client mod!*

### Windows

Download sauerbraten.exe from the link above and put it into `C:\Program Files (x86)\Sauerbraten\bin64\` (your Sauerbraten installation folder).

### macOS

Download sauerbraten_universal from the link above and put it into `/Applications/sauerbraten.app/Contents/MacOS/`, then execute `chmod +x /Applications/sauerbraten.app/Contents/MacOS/sauerbraten_universal` in a terminal.

### Linux

Download linux_64_client from the link above and put it into `bin_unix/` inside of your Sauerbraten directory, then run `chmod +x bin_unix/linux_64_client` in a terminal inside your installation directory.

## Menu

For easy configuration of the new features, an updated version of the `menus.cfg` file is automatically installed when you start p1xbraten for the first time. The new file brings UI options for added features and also includes various cleanups of the vanilla GUI.

If you do not want to use the p1xbraten menus, execute `/usep1xbratenmenus 0`.

## Building your own binary

*You don't have to do this if you already followed the installation instructions above and just want to play!*

On Linux and macOS, just run `make install` **inside the src/ directory** (given you installed the usual Sauerbraten dependencies). On Windows, open src/vcpp/sauerbraten.vcxproj with Visual Studio and build in there.

This will put the resulting binary into the usual place inside this repo. To use it, you have to copy it over to the same place in your actual Sauerbraten installation. On Linux, the [./start.sh](./start.sh) script will launch the new binary from inside this repository, using the Sauerbraten files in $SAUER_DIR and `~/.p1xbraten` as user data directory.

### Using fresh upstream sources

On Linux and macOS, you can build my client using fresh vanilla sources. Set $SAUER_DIR to the path of your Sauerbraten directory, then use `make` and `make install` **in the repository root**:

```
export SAUER_DIR=~/sauerbraten-code
make
make install
```

`make` will copy the src/ directory from $SAUER_DIR, apply all patches and run `make` inside src/; `make install` will run `make install` inside src/.
