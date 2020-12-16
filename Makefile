.PHONY: install clean-sauer update-src check-env

PATCH=patch --strip=0 --remove-empty-files --ignore-whitespace

ifndef SAUER_DIR
ifneq (,$(wildcard ~/sauerbraten-code))
	SAUER_DIR=~/sauerbraten-code
endif
endif

build: update-src apply-patches
	cd src && make client

install:
	cd src && make install
	cp -R data ~/.p1xbraten/

clean:
	cd src && make clean
	cd src/enet && make clean

apply-patches:
	dos2unix src/vcpp/sauerbraten.vcxproj
	$(PATCH) < patches/moviehud.patch
	$(PATCH) < patches/scoreboard.patch
	$(PATCH) < patches/macos_builds.patch
	$(PATCH) < patches/hudfragmessages.patch
	$(PATCH) < patches/fullconsole.patch
	unix2dos src/vcpp/sauerbraten.vcxproj

undo-patches:
	dos2unix src/vcpp/sauerbraten.vcxproj
	$(PATCH) --reverse < patches/fullconsole.patch
	$(PATCH) --reverse < patches/hudfragmessages.patch
	$(PATCH) --reverse < patches/macos_builds.patch
	$(PATCH) --reverse < patches/scoreboard.patch
	$(PATCH) --reverse < patches/moviehud.patch
	unix2dos src/vcpp/sauerbraten.vcxproj

clean-sauer: check-env
	cd $(SAUER_DIR)/src/enet && make clean
	cd $(SAUER_DIR)/src && make clean

update-src: clean-sauer
	rm --recursive --force src/
	rsync --recursive --ignore-times --times --exclude=".*" $(SAUER_DIR)/src .

check-env:
ifndef SAUER_DIR
	$(error SAUER_DIR is undefined)
endif
