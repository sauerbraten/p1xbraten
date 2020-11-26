.PHONY: build apply-patches install uninstall undo-patches build-sauer clean-sauer purge update-src all check-env

SAUER_USER_DIR=${HOME}/.sauerbraten_svn

PATCH=patch --directory=${SAUER_DIR} --strip=0 --remove-empty-files --ignore-whitespace

build: apply-patches build-sauer

apply-patches: check-env
	dos2unix $(SAUER_DIR)/src/vcpp/sauerbraten.vcxproj
	${PATCH} < patches/001_wayland.patch
	$(PATCH) < patches/002_homedir.patch
	$(PATCH) < patches/003_moviehud.patch
	$(PATCH) < patches/004_weaponstats.patch
	$(PATCH) < patches/005_macos-builds.patch
	unix2dos $(SAUER_DIR)/src/vcpp/sauerbraten.vcxproj

install: check-env
	mkdir --parents $(SAUER_USER_DIR)/bin_unix
	mv $(SAUER_DIR)/src/sauer_client $(SAUER_USER_DIR)/bin_unix/native_client

uninstall: check-env
	rm --force $(SAUER_USER_DIR)/bin_unix/native_client

undo-patches: check-env
	dos2unix $(SAUER_DIR)/src/vcpp/sauerbraten.vcxproj
	$(PATCH) --reverse < patches/005_macos-builds.patch
	$(PATCH) --reverse < patches/004_weaponstats.patch
	$(PATCH) --reverse < patches/003_moviehud.patch
	$(PATCH) --reverse < patches/002_homedir.patch
	$(PATCH) --reverse < patches/001_wayland.patch
	unix2dos $(SAUER_DIR)/src/vcpp/sauerbraten.vcxproj

build-sauer: check-env
	cd $(SAUER_DIR)/src && make client

clean-sauer: check-env
	cd $(SAUER_DIR)/src/enet && make clean
	cd $(SAUER_DIR)/src && make clean

purge: uninstall undo-patches clean-sauer

update-src: build clean-sauer
	rm --recursive --force src/
	rsync --recursive --times --exclude=".*" $(SAUER_DIR)/src .

all: build install update-src

check-env:
ifndef SAUER_DIR
	$(error SAUER_DIR is undefined)
endif
