.PHONY: all clean clean-sauer purge check-env

SAUER_USER_DIR=${HOME}/.sauerbraten_svn

PATCH=patch --directory=${SAUER_DIR} --strip=0

all: check-env
	${PATCH} < patches/001_wayland.patch
	$(PATCH) < patches/002_homedir.patch
	$(PATCH) < patches/003_moviehud.patch
	$(PATCH) < patches/004_stats.patch
	cd $(SAUER_DIR)/src && make client
	mkdir -p $(SAUER_USER_DIR)/bin_unix
	mv $(SAUER_DIR)/src/sauer_client $(SAUER_USER_DIR)/bin_unix/native_client

clean: check-env
	rm -f $(SAUER_USER_DIR)/bin_unix/native_client
	$(PATCH) --reverse < patches/004_stats.patch
	$(PATCH) --reverse < patches/003_moviehud.patch
	$(PATCH) --reverse < patches/002_homedir.patch
	$(PATCH) --reverse < patches/001_wayland.patch

clean-sauer: check-env
	cd $(SAUER_DIR)/src && make clean

purge: check-env clean-sauer clean

check-env:
ifndef SAUER_DIR
	$(error SAUER_DIR is undefined)
endif
