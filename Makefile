.PHONY: build install clean apply-patches gzip-cfgs embed-cfgs clean-sauer update-src apply-to-vanilla check-env

PATCH=patch --strip=0 --remove-empty-files --ignore-whitespace

ifndef SAUER_DIR
ifneq (,$(wildcard ~/sauerbraten-code))
	SAUER_DIR=~/sauerbraten-code
endif
endif

build: update-src apply-patches embed-cfgs
	cd src && make

install:
	cd src && make install

clean:
	cd src && make clean
	cd src/enet && make clean

apply-patches:
	dos2unix src/vcpp/sauerbraten.vcxproj
	dos2unix src/vcpp/sauerbraten.nsi
	$(PATCH) < patches/modversion.patch
	$(PATCH) < patches/moviehud.patch
	$(PATCH) < patches/getflagholder.patch
	$(PATCH) < patches/scoreboard.patch
	$(PATCH) < patches/macos_builds.patch
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
	$(PATCH) < patches/win_builds.patch
	$(PATCH) < patches/chat_highlight_words.patch
	$(PATCH) < patches/zenmode.patch
	$(PATCH) < patches/authservers.patch
	$(PATCH) < patches/serverlogging.patch
	$(PATCH) < patches/gamehud.patch
	$(PATCH) < patches/server_ogzs.patch
	$(PATCH) < patches/minimizedframes.patch
	unix2dos src/vcpp/sauerbraten.nsi
	unix2dos src/vcpp/sauerbraten.vcxproj
	cd src && make depend

gzip-cfgs:
	gzip --keep --force --best --no-name data/p1xbraten/menus.cfg && xxd -i - data/p1xbraten/menus.cfg.gz.xxd < data/p1xbraten/menus.cfg.gz
	gzip --keep --force --best --no-name data/p1xbraten/master.cfg && xxd -i - data/p1xbraten/master.cfg.gz.xxd < data/p1xbraten/master.cfg.gz
	gzip --keep --force --best --no-name data/p1xbraten/gamehud.cfg && xxd -i - data/p1xbraten/gamehud.cfg.gz.xxd < data/p1xbraten/gamehud.cfg.gz
	gzip --keep --force --best --no-name data/p1xbraten/keymap.cfg && xxd -i - data/p1xbraten/keymap.cfg.gz.xxd < data/p1xbraten/keymap.cfg.gz

embed-cfgs: gzip-cfgs
	sed -i "s/0,\/\/menuscrc/0x$(shell crc32 data/p1xbraten/menus.cfg),/" src/fpsgame/embedded_cfgs.cpp
	sed -i "s/embeddedfile<0> menuscfg/embeddedfile<$(shell stat --printf="%s" data/p1xbraten/menus.cfg.gz)> menuscfg/" src/fpsgame/embedded_cfgs.cpp
	sed -i "s/0,\/\/mastercrc/0x$(shell crc32 data/p1xbraten/master.cfg),/" src/fpsgame/embedded_cfgs.cpp
	sed -i "s/embeddedfile<0> mastercfg/embeddedfile<$(shell stat --printf="%s" data/p1xbraten/master.cfg.gz)> mastercfg/" src/fpsgame/embedded_cfgs.cpp
	sed -i "s/0,\/\/gamehudcrc/0x$(shell crc32 data/p1xbraten/gamehud.cfg),/" src/fpsgame/embedded_cfgs.cpp
	sed -i "s/embeddedfile<0> gamehudcfg/embeddedfile<$(shell stat --printf="%s" data/p1xbraten/gamehud.cfg.gz)> gamehudcfg/" src/fpsgame/embedded_cfgs.cpp
	sed -i "s/0,\/\/keymapcrc/0x$(shell crc32 data/p1xbraten/keymap.cfg),/" src/fpsgame/embedded_cfgs.cpp
	sed -i "s/embeddedfile<0> keymapcfg/embeddedfile<$(shell stat --printf="%s" data/p1xbraten/keymap.cfg.gz)> keymapcfg/" src/fpsgame/embedded_cfgs.cpp

clean-sauer: check-env
	cd $(SAUER_DIR) && \
		svn cleanup . --remove-unversioned --remove-ignored && \
		svn revert --recursive --remove-added .
	cd $(SAUER_DIR)/src/enet && make clean
	cd $(SAUER_DIR)/src && make clean

update-src: clean-sauer
	rm --recursive --force src/
	rsync --recursive --ignore-times --times --exclude=".*" $(SAUER_DIR)/src .

apply-to-vanilla: update-src apply-patches
	rm --recursive --force $(SAUER_DIR)/src/*
	rsync --recursive --ignore-times --times --exclude=".*" ./src/* $(SAUER_DIR)/src/

check-env:
ifndef SAUER_DIR
	$(error SAUER_DIR is undefined)
endif
