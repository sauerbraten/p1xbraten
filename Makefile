.PHONY: build apply-patches install uninstall undo-patches build-sauer clean-sauer purge update-src all check-env

SAUER_USER_DIR=${HOME}/.sauerbraten_svn

PATCH=patch --directory=${SAUER_DIR} --strip=0

build: apply-patches build-sauer

apply-patches: check-env
	${PATCH} < patches/001_wayland.patch
	$(PATCH) < patches/002_homedir.patch
	$(PATCH) < patches/003_moviehud.patch
	$(PATCH) < patches/004_weaponstats.patch

install: check-env
	mkdir --parents $(SAUER_USER_DIR)/bin_unix
	mv $(SAUER_DIR)/src/sauer_client $(SAUER_USER_DIR)/bin_unix/native_client

uninstall: check-env
	rm --force $(SAUER_USER_DIR)/bin_unix/native_client

undo-patches: check-env
	$(PATCH) --reverse < patches/004_weaponstats.patch
	$(PATCH) --reverse < patches/003_moviehud.patch
	$(PATCH) --reverse < patches/002_homedir.patch
	$(PATCH) --reverse < patches/001_wayland.patch

build-sauer: check-env
	cd $(SAUER_DIR)/src && make client

clean-sauer: check-env
	cd $(SAUER_DIR)/src && make clean

purge: uninstall undo-patches clean-sauer

update-src: check-env clean-sauer
	rm --recursive --force src/
	rsync --recursive --times --exclude=".*" $(SAUER_DIR)/src .

all: build install update-src

check-env:
ifndef SAUER_DIR
	$(error SAUER_DIR is undefined)
endif
