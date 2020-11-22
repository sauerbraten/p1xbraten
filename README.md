# p1xbraten

This repository contains my patches for the next Sauerbraten release. To build my client, set the path to your Sauerbraten directory (currently, an SVN checkout is required), then use `make`:

```
export SAUER_DIR=~/sauerbraten-code
make
```

This will apply all patches, compile a client binary, and put it into .sauerbraten_svn/bin_unix/ (which is where the patched sauerbraten_unix script will look first). To remove the custom client from your user directory and undo all patches to the Sauer code, use `make clean`.

## Patches

- [wayland.patch](./patches/001_wayland.patch): sets `SDL_VIDEODRIVER=wayland` env var before launching Sauer to get more than 60fps on systems using Gnome 3.38 on Wayland (e.g. Fedora)
- [homedir.patch](./patches/002_homedir.patch):
    - sets up sauerbraten_unix to use .sauerbraten_svn as the user's content directory (avoids conflicts with an existing .sauerbraten directory from the installed release)
    - makes sauerbraten_unix launch (if it exists) `native_client` from .sauerbraten_svn/bin_unix/ (which is where `make` put the compiled binary)
- [moviehud.patch](./patches/003_moviehud.patch):
    - adds `hidespecfollow` toggle: when 1, hides the "SPECTATOR" and player name in the lower right of the screen when spectating)
    - adds `namesabovehead` toggle: when 0, hides the names above players' models (usually rendered as particle text), while keeping status icons for health boost, armor and quad
- [weaponstats.patch](./patches/004_weaponstats.patch):
    - enables detailed per-weapon damage stats recording
    - adds `getdamagepotential`, `getdamagedealt`, `getdamagereceived` commands
    - adds `getdamagewasted` (`getdamagepotential - getdamagedealt`) command
    - adds `netdamage` (`getdamagedealt - getdamagereceived`) command
    - changes `getaccuracy` command
    - all of these commands default to showing your own stats across all weapons
    - however, they all take two optional integer arguments to query by weapon and player: `/<cmd> [weapon] [cn]` (use `-1` to query damage across all weapons)