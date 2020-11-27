.PHONY: install clean-sauer update-src check-env

PATCH=patch --strip=0 --remove-empty-files --ignore-whitespace

build: update-src apply-patches
	cd src && make client

install:
	cd src && make install
	cp -R data ~/.p1xbraten/

clean:
	cd src && make clean
	cd src/enet && make clean

apply-patches:
	$(PATCH) < patches/moviehud.patch
	$(PATCH) < patches/clean_scoreboard.patch
	dos2unix src/vcpp/sauerbraten.vcxproj
	$(PATCH) < patches/weaponstats.patch
	unix2dos src/vcpp/sauerbraten.vcxproj
	$(PATCH) < patches/macos_builds.patch

undo-patches:
	$(PATCH) --reverse < patches/macos_builds.patch
	dos2unix src/vcpp/sauerbraten.vcxproj
	$(PATCH) --reverse < patches/weaponstats.patch
	unix2dos src/vcpp/sauerbraten.vcxproj
	$(PATCH) --reverse < patches/clean_scoreboard.patch
	$(PATCH) --reverse < patches/moviehud.patch

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
