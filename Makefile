SHELL := /bin/bash

.PHONY: build install clean anticheat linux macos windows apply-patches gzip-cfgs embed-cfgs clean-sauer update-src from-patches changes-to-patch ensure-sauer-dir

# build install: export DEBUG=1
build:
	cd src && make --jobs=8

install:
	cd src && make --jobs=8 install

clean:
	cd src && make clean
	cd src/enet && make clean

key:
	dd if=/dev/random bs=1 count=256 of=src/anticheat/key
	xxd -i src/anticheat/key src/anticheat/key.h

linux macos windows: export ANTICHEAT=1
# linux macos windows: export DEBUG=1
linux macos windows: | clean key
	source secrets.env && go run encrypt_credentials.go < src/anticheat/key && \
	cd src && \
	export TARGET="x86_64-$@" && \
	make --jobs=8 install

debian: clean
	dd if=/dev/random of=Dockertrigger count=64 && \
	podman build \
	--file Dockerfile.debian-11-build \
	--security-opt label=disable \
	--volume=$(PWD)/src:/src \
	--volume=$(PWD)/bin_unix:/bin_unix \
	--volume=$(PWD)/data:/data:ro \
	.

apply-patches: PATCH=patch --strip=0 --remove-empty-files --ignore-whitespace --no-backup-if-mismatch
apply-patches:
	$(PATCH) < patches/modconfig.patch
	$(PATCH) < patches/modversion.patch
	$(PATCH) < patches/moviehud.patch
	$(PATCH) < patches/hasflag.patch
	$(PATCH) < patches/colors.patch
	$(PATCH) < patches/scoreboard.patch
	$(PATCH) < patches/hudfragmessages.patch
	$(PATCH) < patches/fullconsole.patch
	$(PATCH) < patches/hudscore.patch
	$(PATCH) < patches/serverbrowser.patch
	$(PATCH) < patches/listteams.patch
	$(PATCH) < patches/execfile.patch
	$(PATCH) < patches/embedded_cfgs.patch
	$(PATCH) < patches/tex_commands.patch
	$(PATCH) < patches/decouple_framedrawing.patch
	$(PATCH) < patches/crosshaircolor.patch
	$(PATCH) < patches/chat_highlight_words.patch
	$(PATCH) < patches/zenmode.patch
	$(PATCH) < patches/authservers.patch
	$(PATCH) < patches/serverlogging.patch
	$(PATCH) < patches/gamehud.patch
	$(PATCH) < patches/server_ogzs.patch
	$(PATCH) < patches/minimizedframes.patch
	$(PATCH) < patches/playerspeed.patch
	$(PATCH) < patches/up_down_hover.patch
	$(PATCH) < patches/paused_spec_movement.patch
	$(PATCH) < patches/clientdemo.patch
	$(PATCH) < patches/colored_weapon_trails.patch
	$(PATCH) < patches/serverevents.patch
	$(PATCH) < patches/crosshairreloadfade.patch
	$(PATCH) < patches/managed_games.patch
	$(PATCH) < patches/better_console.patch
	$(PATCH) < patches/autoauthdomains.patch
	$(PATCH) < patches/nextfollowteam.patch
	$(PATCH) < patches/proxy_setip.patch
	$(PATCH) < patches/server_demo_name.patch
	$(PATCH) < patches/spec_teleports.patch
	$(PATCH) < patches/demo_info_message.patch
	$(PATCH) < patches/extinfo_mod_id.patch
	$(PATCH) < patches/anticheat.patch
	$(PATCH) < patches/setfont.patch
	$(PATCH) < patches/serverbotbalanceearly.patch
	$(PATCH) < patches/bans.patch
	$(PATCH) < patches/enet_mtu_1300.patch
	$(PATCH) < patches/fullscreenmonitor.patch
	cd src && make depend

gzip-cfgs embed-cfgs: DATA=data/p1xbraten
gzip-cfgs: GZIP=gzip --keep --force --best --no-name
gzip-cfgs: # to avoid C code surrounding the bytes, use: xxd -i - outfile < [infile]
	for f in $(DATA)/*.cfg; do $(GZIP) $${f} && xxd -i - $${f}.gz.xxd < $${f}.gz; done

embed-cfgs: EMBEDS=menus master keymap gamehud
embed-cfgs: TARGET=src/p1xbraten/embedded_cfgs.cpp
embed-cfgs: gzip-cfgs
	for f in $(EMBEDS); do \
		sed -i "s/0,\/\/$${f}_crc/0x$$(crc32 $(DATA)/$${f}.cfg),/" $(TARGET) && \
		sed -i "s/embeddedfile<0> $${f}_cfg/embeddedfile<$$(stat --printf="%s" $(DATA)/$${f}.cfg.gz)> $${f}_cfg/" $(TARGET) \
	; done
	cd src && make depend

ifndef SAUER_DIR
ifneq (,$(wildcard ~/sauerbraten-code))
SAUER_DIR=~/sauerbraten-code
endif
endif

clean-sauer: ensure-sauer-dir
	cd $(SAUER_DIR) && \
		svn cleanup . --remove-unversioned --remove-ignored && \
		svn revert --recursive --remove-added .
	cd $(SAUER_DIR)/src/enet && make clean
	cd $(SAUER_DIR)/src && make clean

VANILLA_SOURCE_FILES=enet,engine,fpsgame,shared
P1XBRATEN_SOURCE_FILES=$(VANILLA_SOURCE_FILES),p1xbraten,anticheat/anticheat.cpp

update-src: clean-sauer
	rm --recursive --force src/{$(P1XBRATEN_SOURCE_FILES)}
	rsync --recursive --ignore-times --times --exclude=".*" $(SAUER_DIR)/src/{$(VANILLA_SOURCE_FILES)} src/

from-patches: | update-src apply-patches embed-cfgs build

changes-to-patch:
	git diff --no-prefix --ignore-all-space --staged data/p1xbraten src/{$(P1XBRATEN_SOURCE_FILES)} > patches/new_changes.patch

ensure-sauer-dir:
ifndef SAUER_DIR
	$(error SAUER_DIR is undefined)
endif
