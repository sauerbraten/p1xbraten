# p1xbraten

This repository contains the data, docs, packages and src directories from SVN, as well as some patches and a top-level Makefile. To build my client, simply use `make` in the repository root directory. This will apply all patches, compile a client binary, and put it into bin_unix/ into .sauerbraten_svn/bin_unix/ (which is where the patched sauerbraten_unix script will look first). To undo all changes to the files tracked by SVN, use `make clean`.

## Patches

- [wayland.patch](./patches/wayland.patch): sets `SDL_VIDEODRIVER=wayland` env var before launching Sauer to get more than 60fps on systems using Gnome 3.38 on Wayland (e.g. Fedora)
- [homedir.patch](./patches/homedir.patch):
    - sets up sauerbraten_unix to use .sauerbraten_svn as the user's content directory (avoids conflicts with an existing .sauerbraten directory from the installed release)
    - makes sauerbraten_unix launch (if it exists) `native_client` from .sauerbraten_svn/bin_unix/ (which is where `make` put the compiled binary)
- [moviehud.patch](./patches/moviehud.patch):
    - adds `hidespecfollow` toggle: when 1, hides the "SPECTATOR" and player name in the lower right of the screen when spectating)
    - adds `namesabovehead` toggle: when 0, hides the names above players' models (usually rendered as particle text), while keeping status icons for health boost, armor and quad
