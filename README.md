# p1xbraten

This repository contains the source for my client mod, as well as the patches applied to the vanilla Sauerbraten source to get there.

- [Installation](#installation)
  - [Windows](#windows)
  - [macOS](#macos)
  - [Linux](#linux)
- [Menu](#menu)
- [Patches](#patches)
  - [upstreamed to SVN](#upstreamed-to-svn)
  - [moviehud.patch](#moviehudpatch)
  - [scoreboard.patch](#scoreboardpatch)
  - [hudfragmessages.patch](#hudfragmessagespatch)
  - [fullconsole.patch](#fullconsolepatch)
  - [hudscore.patch](#hudscorepatch)
  - [serverbrowser.patch](#serverbrowserpatch)
  - [listteams.patch](#listteamspatch)
  - [tex\_commands.patch](#tex_commandspatch)
  - [decouple\_framedrawing.patch](#decouple_framedrawingpatch)
  - [crosshaircolor.patch](#crosshaircolorpatch)
  - [zenmode.patch](#zenmodepatch)
  - [gamehud.patch](#gamehudpatch)
  - [chat\_highlight\_words.patch](#chat_highlight_wordspatch)
  - [modversion.patch](#modversionpatch)
  - [minimizedframes.patch](#minimizedframespatch)
  - [hasflag.patch](#hasflagpatch)
  - [playerspeed.patch](#playerspeedpatch)
  - [up\_down\_hover.patch](#up_down_hoverpatch)
  - [paused\_spec\_movement.patch](#paused_spec_movementpatch)
  - [clientdemo.patch](#clientdemopatch)
  - [colored\_weapon\_trails.patch](#colored_weapon_trailspatch)
  - [crosshairreloadfade.patch](#crosshairreloadfadepatch)
  - [better\_console.patch](#better_consolepatch)
  - [nextfollowteam.patch](#nextfollowteampatch)
  - [anticheat.patch](#anticheatpatch)
  - [setfont.patch](#setfontpatch)
  - [filterservers.patch](#filterserverspatch)
- [Server Patches](#server-patches)
  - [authservers.patch](#authserverspatch)
  - [serverlogging.patch](#serverloggingpatch)
  - [server\_ogzs.patch](#server_ogzspatch)
  - [managed\_games.patch](#managed_gamespatch)
  - [autoauthdomains.patch](#autoauthdomainspatch)
  - [proxy\_setip.patch](#proxy_setippatch)
  - [anticheat.patch](#anticheatpatch-1)
  - [serverbotbalanceearly.patch](#serverbotbalanceearlypatch)
  - [bans.patch](#banspatch)
- [Project Structure](#project-structure)
- [Building your own binary](#building-your-own-binary)
  - [Build dependencies](#build-dependencies)
  - [Building](#building)
  - [Using fresh upstream sources](#using-fresh-upstream-sources)
  - [Debugging](#debugging)


## Installation

The latest release is always at https://github.com/sauerbraten/p1xbraten/releases/latest.

### Windows

Download and run p1xbraten_setup_\<version\>.exe from the link above. p1xbraten will be installed alongside your existing Sauerbraten installation.

### macOS

1. Download macos.zip from the link above, open your Downloads folder in Finder and double-click the zip file to extract it.
2. Navigate into sauerbraten.app
3. Open a second Finder window
4. Go to Applications, right click Sauerbraten.app, select "Show Package Contents"
5. Make a copy of `Contents/MacOS/` (in case you want to go back to the vanilla client in the future)
6. Drag the `Contents` folder from the zip file into the Finder window, merging it with the existing folder and replacing any existing files.
7. Open a Terminal and type `chmod +x ` (note the space at the end), then drag the `p1xbraten_x86_64_client` file from Finder into the Terminal window
8. When it says `chmod +x [...]/Contents/MacOS/p1xbraten_x86_64_client`, press Enter to execute
9. In Finder, right click start_protected_game.app, select "Show Package Contents", go into `Contents`, then into `MacOS`
10. In the terminal, type `chmod +x ` again, then drag the `start_protected_game` file from Finder into the Terminal window
11. When it says `chmod +x [...]/Contents/MacOS/start_protected_game.app/Contents/MacOS/start_protected_game`, press Enter to execute

If this seems like a lot of work that's because it is. I blame Apple.

### Linux

Download and extract linux.zip from the link above and put its contents into your Sauerbraten directory, then `chmod +x bin_unix/p1xbraten_x86_64_client bin_unix/start_protected_game`. Use p1xbraten.sh/anticheat.sh to launch.


## Menu

For easy configuration of the new features, an updated version of the `menus.cfg` file is automatically installed when you start p1xbraten for the first time. The new file brings UI options for added features and also includes various cleanups of the vanilla GUI.

If you do not want to use the p1xbraten menus, run `/usep1xbratenmenus 0`.


## Patches

a.k.a. Features

### upstreamed to SVN

- "fixes" `nextfollow` to start at the first/last player when cycling forwards/backwards, instead of second or second to last respectively
- allows using <kbd>Ctrl</kbd>-<kbd>Left</kbd>/<kbd>Right</kbd> (<kbd>Option</kbd> on Mac) to jump over words in the console input
- allows using <kbd>Ctrl</kbd>-<kbd>Backspace</kbd>/<kbd>Delete</kbd> (<kbd>Option</kbd> on Mac) to delete entire words in the console input
- improves player name matching in `setmaster`, `setteam`, `ignore`, `kick`, `spectator`, `follow` and `goto` commands: after the normal full matching, (if `parseplayer`>0) the argument is tried to be matched as a case-insensitive prefix, then a case-insensitive substring of a player's name

### [moviehud.patch](./patches/moviehud.patch)

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

- makes the following built-in cubescript commands take an optional player CN as argument:
    - `getfrags`
    - `getflags`
    - `getdeaths`
    - `getaccuracy`
    - `gettotaldamage`
    - `gettotalshots`
    - `getweapon`

  Omitting the argument will return the information about `hudplayer`, i.e. yourself (if playing) or the player you're currently spectating.

- adds damage-related cubescript commands:
    - `getdamagepotential` (same as `gettotalshots` when not filtering by weapon)
    - `getdamagedealt` (same as `gettotaldamage` when not filtering by weapon)
    - `getdamagereceived`
    - `getdamagewasted` (= potential − dealt)
    - `getnetdamage` (= dealt − received)

  All of the new damage-related commands (as well as `getaccuracy`) default to showing `hudplayer`'s stats across all weapons. However, they all take two optional integer arguments to query stats of a certain player and optionally for a specific weapon: `/<cmd> [cn] [weapon]` (use -1 as CN to query specific weapon stats about `hudplayer`).

  To show comed-like statistics in the lower right corner, copy the following into your `autoexec.cfg`:
  ```
  gamehud = [
    format "^f7SG: ^f1%1%% ^f7CG: ^f1%2%% ^f7RL: ^f1%3%% ^f7RI: ^f1%4%% ^f7GL: ^f1%5%% ^n^f7frags: ^f0%6 ^f7deaths: ^f3%7 ^f7acc: ^f2%8%% ^f7kpd: ^f5%9" (
      round (getaccuracy -1 1) 0.1                    )(
      round (getaccuracy -1 2) 0.1                    )(
      round (getaccuracy -1 3) 0.1                    )(
      round (getaccuracy -1 4) 0.1                    )(
      round (getaccuracy -1 5) 0.1                    )(
      getfrags                                        )(
      getdeaths                                       )(
      round (getaccuracy) 0.1                         )(
      round (divf (getfrags) (max (getdeaths) 1)) 0.1	)
  ]
  ```

### [hudfragmessages.patch](./patches/hudfragmessages.patch)

- enables frag messages showing the weapon used to complete the frag (on by default)
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
    - `hudfragmessageforcecolors`: when 1 (default), uses colored names, even in non-team modes

You can easily configure the hud frag messages using the [improved menu](#menu) (options → hud → adjust hud frag messages).

![fragmessages](https://i.imgur.com/K4GL6oB.png)

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

### [decouple_framedrawing.patch](./patches/decouple_framedrawing.patch)

- removes fps-induced limiting of the main loop (for example, input & network processing were affected by `maxfps`)
- introduce `maxtps` var to limit the main loop independently from `maxfps` (for example to save power): 0 to disable limiting (default), 100..1000 to set how many ticks per second are allowed; `maxfps` overrides `maxtps` if `maxfps > maxtps`

This patch allows using `maxfps` without compromising on the frequency of input polling and network event processing. In vanilla Sauerbraten, `maxfps` limits the whole game's main loop. This patch removes the main loop limiting and instead skips only frame drawing until a new frame is needed, but still processes network events and player input on every main loop iteration.

While vanilla Sauerbraten is truly unlimited after `/maxfps 0`, p1xbraten's mainloop can additionally be limited using the `maxtps` variable. Setting `maxfps` to any value other than 0 also forces a maximum tick frequency of 1000 (but respects lower settings), even if you set `maxtps 0`, so that p1xbraten can prioritise frame draw timing. Set both `maxfps` and `maxtps` to 0 to get vanilla's unlimited behavior back.

Using `maxfps` and `maxtps`, you can optimize for different goals:

- for lowest average latency, use `/maxtps 0` and `/vsync 0`
- for lowest consistent latency, use `/maxfps 0`, `/vsync 0`, and set `maxtps` to the highest value that gives you a stable fps counter on your system
- to use fewer resources and save laptop battery, use `/maxfps 0`, and set `maxtps` to the lowest value that gives you smooth graphics

### [crosshaircolor.patch](./patches/crosshaircolor.patch)

- adds `crosshaircolor` variable to set a base crosshair color (for example, `/crosshaircolor 0 255 0` for pleasant green)

### [zenmode.patch](./patches/zenmode.patch)

- adds `zenmode` variable: if 1, hides non-essential console messages:
  - server messages
  - chat and team chat messages from spectators
  - joins (all), leaves and renames (of spectators)

### [gamehud.patch](./patches/gamehud.patch)

- properly right-justifies gamehud, wallclock, showfps, and/or showfpsrange lines
- adds a useful playerlist showing who's alive vs. dead when spectating in the lower right corner
- adds `isdead <cn>` command to check if a player is currently dead (only works when you are spectating)

<div align="center"><img alt="gamehud with player state" src="https://i.imgur.com/8kfmhSK.gif" /></div>

### [chat_highlight_words.patch](./patches/chat_highlight_words.patch)

- lets you define words that trigger a sound when they appear in chat or team chat
- adds `addchathighlightword <word>` command (for example, put `addchathighlightword pix` and `addchathighlightword p1x` into autoexec.cfg to receive a highlight on both spellings)
- adds `chathighlightsound` variable to set the sound to play (default: `free/itempick`)

### [modversion.patch](./patches/modversion.patch)

- adds the `p1xbratenversion` variable (read-only): contains the version of your installation as a string
- adds the `getclientp1xbratenversion <cn>` command: returns the version of p1xbraten the player is running
- adds the `enablep1xbratendetection` variable: set to 0 to disable broadcasting your p1xbraten version (and detecting other p1xbraten players yourself)

This lets you see wether a player connected to the same server as you is using p1xbraten, and what version. It relies on p1xbraten clients sending additional packets to the server, which the server then broadcasts to all other clients. This means other players can tell you're using p1xbraten (and what version). No information other than "I use p1xbraten, version X.Y.Z" is sent. No information is sent to me (p1x) or any third party. Opt out if you are firefly!

For an overview of who uses p1xbraten, you can run `/showgui p1xbratenusage`.

### [minimizedframes.patch](./patches/minimizedframes.patch)

- adds the `minimizedframes` toggle: when 1, frames will always be drawn, even when they would usually be skipped because p1xbraten is minimized

### [hasflag.patch](./patches/hasflag.patch)

- adds the `hasflag <cn>` command: returns 1 if the specfied player carries a flag

### [playerspeed.patch](./patches/playerspeed.patch)

- adds the `playerspeed` variable (read-only): returns the hud player's speed (in units per second)

### [up_down_hover.patch](./patches/up_down_hover.patch)

- adds the `up`, `down` and `hover` movement commands:
  - `up`/`down` induce movement up or down relative to the player's view
  - `hover` prevents any change in height from forward/backward movement and making `up` and `down` work along the world's Z axis

  all three commands only work while editing or spectating

### [paused_spec_movement.patch](./patches/paused_spec_movement.patch)

- enables free movement as spectator while the game is paused

### [clientdemo.patch](./patches/clientdemo.patch)

- adds the `recordclientdemo <0|2>` command: 0 is the same as `stopclientdemo`; 2 starts recording a demo immediately
- adds the `stopclientdemo` command: stops an ongoing demo recording

Client-side demos record the game as you experience it, and work independently of server-side demo recording. They also work locally, without being connected to any server. Your own position, movement and view is recorded at 125 frames per second.

### [colored_weapon_trails.patch](./patches/colored_weapon_trails.patch)

- adds the `trailcolor[shotgun|chaingun|rocketlauncher|rifle|grenadelauncher|pistol]` vars that change the respective weapon's projectile/particle trail
- adds the `settrailcolor <SG|CG|RL|RI|GL|PI> <color>` command (for convenience: changing the variables directly also works)

You can also adjust the trail colors in the options menu.

### [crosshairreloadfade.patch](./patches/crosshairreloadfade.patch)

- adds `crosshairreloadfade` variable: set to 0 to disable the "crosshair-goes-dark-while-weapon-reloads" effect

### [better_console.patch](./patches/better_console.patch)

- adds name completion to chat console (press <kbd>Tab</kbd>)

Name completion tries old-style prefix matching first, then will also try substring matching to suggest more names. In addition to vanilla's matching at the end of the line, it also works in the middle of the input, as long as the cursor is placed in front of a space character.

If you want to keep using <kbd>Tab</kbd> to complete commands in the chat prompt (without explicitly typing '/' at the beginning), use this bind:
```
bind "T" [inputcommand "" "" "" "c"] // instead of saycommand
```

Also, you can put the following into your autoexec.cfg for cleaner command prompts (replace the keys to your liking):
```
bind "SLASH"     [inputcommand "" [$commandbuf] "/" "cx"]
bind "BACKQUOTE" [inputcommand "" [$commandbuf] "/" "cx"]
bind "CARET"     [inputcommand "" [$commandbuf] "/" "cx"] // BACKQUOTE on German keyboards
bind "BACKSLASH" [inputcommand "" [servcmd $commandbuf] "#"] // works for #commands on zeromod, spaghetti, and remod
bind "HASH"      [inputcommand "" [servcmd $commandbuf] "#"] // only works with German keyboard layout
```

### [nextfollowteam.patch](./patches/nextfollowteam.patch)

- adds the `nextfollowteam` command that will cycle through players grouped by team and falls back to vanilla's `nextfollow` in non-team modes (which just iterates through CNs from low to high)

To use it, put the following lines into your autoexec.cfg (and remember you'll have to change them back to `nextfollow` before using the autoexec.cfg with other clients!):
```
specbind "MOUSE1" [nextfollowteam]
specbind "MOUSE2" [nextfollowteam -1]
```

### [anticheat.patch](./patches/anticheat.patch)

Integrates Epic's Online Services SDK and Anti-Cheat framework to provide protected game sessions on p1xbraten anticheat servers.

- adds the `-e` command line flag to enable anticheat support (anticheat launchers set this for you)
- adds the `anticheatenabled` read-only variable (only available when using the anticheat launcher, and should never be 0)

### [setfont.patch](./patches/setfont.patch)

- adds the `setfont <name>` command to set the UI font after registering the font using `font <name> ...` (`setfont "default"` to reset)

### [filterservers.patch](./patches/filterservers.patch)

- adds the `filterservers` string variable to only show servers in the serverbrowser whose mode, map or description matches (using case-insensitive substring matching)

The p1xbraten menu gives you a simple search field on the serverbrowser using this variable.


## Server Patches

These are the patches that make p1x.pw different from other servers.

### [authservers.patch](./patches/authservers.patch)

- adds `addauthserver <key_domain> <hostname> <port> <privilege>` server command to add an additional auth backend (other than the master server)

For example, you can put `addauthserver "p1x.pw" "p1x.pw" 28787 "m"` into your `server-init.cfg` to allow users registered with my master server to claim auth on your server.

### [serverlogging.patch](./patches/serverlogging.patch)

Improves logging when running a dedicated server:

- adds `logtime` variable: if 1, all log messages are prepended with ISO date and time (e.g. `[2021-04-06 17:12:05]`)
- includes CN in connect, disconnect, chat, team chat log messages
- logs a "join" message including CN and player name
- logs map changes
- logs all privilege changes

### [server_ogzs.patch](./patches/server_ogzs.patch)

- allows using slim .ogz files (see https://github.com/sauerbraten/genserverogz) on the server without getting `checkmaps` errors

### [managed_games.patch](./patches/managed_games.patch)

- adds the `#competitive [MM[:SS]]` remote command, that will (for the next match only):
  - enable server demo recording and request client demo recording from p1xbraten users
  - optionally, set the time to MM:SS
  - wait for all players to have loaded the map before starting
  - respawn all players on game start
  - auto-pause the game when a player disconnects or goes to spec
  - show a count down before resuming
  - accept client demo uploads at intermission
  - save all demo files of the match to disk
  - not auto-load the next map
- adds the (admin-only) `#do` remote command to evaluate arbitrary cubescript on the server, for example `#do persistteams 1`, `#do servermotd "happy fragging"` or `#do concat $serverauth`

### [autoauthdomains.patch](./patches/autoauthdomains.patch)

- adds the `addautoauthdomain` command to request an auth key automatically right after a player joined (*not* before joining; this won't let the player connect in private mode: only the server auth domain can be used for that!)

### [proxy_setip.patch](./patches/proxy_setip.patch)

- adds the `addtrustedproxyip` command to declare a packet source IP as belonging to a trusted proxy

This patch adds support for the [SauerDuels proxy](https://github.com/sauerduels/sauer-proxy) which can be used to obfuscate the IP of tournament servers. The `setip` protocol extension allows the proxy to forward the client's real IP, replacing the proxy's IP.

### [anticheat.patch](./patches/anticheat.patch)

- adds the `-e` command line flag to enable anticheat support
- adds the `forceanticheatclient` toggle: if enabled, players are forced-to-spec on connect, and have to be using a p1xbraten anticheat client in order to play

### [serverbotbalanceearly.patch](./patches/serverbotbalanceearly.patch)

- adds the `serverbotbalanceearly` variable: if 1 (and `serverbotbalance` is also enabled), the server will equalize the player count of all teams by adding/reassigning/removing bots

This is a more aggressive form of auto-balance than vanilla's `serverbotbalance`, which only reassigns bots, and never adds or removes bots on its own. It's intended for public play and disables manually adding/removing bots.

### [bans.patch](./patches/bans.patch)

- adds the `getip <cn>` command to return a client's IP
- adds the `ban <ip> <minutes>` command to ban an IP for the specified duration (or 30 minutes if not specified)
- adds the `unban <ip>` command to remove any ban created earlier
- adds the `listbans` command to list all currently active IP bans

All of these commands are admin-only. You can use these from cubescript or as a remote command (e.g. `#listbans`).


## Project Structure

- `patches/` has all the patch files that turn vanilla Sauerbraten into p1xbraten
- `src/` has the p1xbraten source tree
- `Makefile` has commands to work with the patches and an SVN checkout of vanilla
- `data/` contains p1xbraten specific cubescript files: each files exist as plain text .cfg file as well as gzipped, xxd'ed file for embedding during compilation
- `bin_unix/`, `bin64/`, and `sauerbraten.app/` contain OS-specific runtime dependencies
- `p1xbraten.bat` and `p1xbraten.sh` are launch scripts for Windows/Linux
- `anticheat.bat`/`anticheat.sh` launch p1xbraten with anti-cheat support enabled
- `server.sh`, `start.sh` and `start_anticheat.sh` are my custom launch scripts (to facilitate launching the binary in this repo, but using content from ~/sauerbraten-code)

## Building your own binary

*You don't have to do this if you already followed the installation instructions above and just want to play!*

### Build dependencies

- development headers of the ususal Sauerbraten build dependencies like libz, libSDL2[-image|-mixer]
- libarchive-zip-perl / perl-Archive-Zip
- development headers of libcurl (when checkupdate.patch is included and you're building targets other than linux64)

### Building

On Linux or macOS, just run `make install` (given you installed the usual Sauerbraten dependencies). Building on Windows is not supported.

This will put the resulting binary into the usual place inside this repo. To use it, you have to copy it over to the same place in your actual Sauerbraten installation. On Linux, the [start.sh](./start.sh) script will launch the new binary from inside this repository, using the Sauerbraten files in $SAUER_DIR and `~/.p1xbraten` as user data directory.

### Using fresh upstream sources

You can build my client using fresh vanilla sources. Set $SAUER_DIR to the path of your Sauerbraten directory, then use `make from-patches` and `make install`:

```
export SAUER_DIR=~/sauerbraten-code
make from-patches
make install
```

A fresh source tree will be copied from the Sauerbraten directory, then all my patches are applied on top of that before buildling and installing a new binary.

### Debugging

On Linux, I replace `-O3` with `-g -rdynamic` in the [Makefile](./src/Makefile), and `exec` with `exec gdb --args` in [start.sh](./start.sh) to set up debugging. Then:

```
make clean && make && make install
./start.sh
```

In gdb, typing `run` starts p1xbraten.