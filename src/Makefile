# building ON Windows is not supported, only building FOR Windows (set TARGET before calling make)
HOSTOS=linux
ifneq (,$(findstring Darwin,$(shell uname -s)))
	HOSTOS=macos
endif

ifndef TARGET
	# 32-bit platforms are not supported, and we can't build for arm64/aarch64 (yet) because of SDL2/EOS
	TARGET=x86_64-$(HOSTOS)
endif

ifeq (,$(findstring $(HOSTOS),$(TARGET)))
	CC=zig cc --target=$(TARGET)
	CXX=zig c++ --target=$(TARGET)
	AR=zig ar
else
	CC=clang
	CXX=clang++
endif

CFLAGS=
CXXFLAGS= -fomit-frame-pointer -ffast-math -Wno-deprecated-declarations
LINKERFLAGS=
ifdef DEBUG
	override CFLAGS+= -g3
	override CXXFLAGS+= -O0 -g3 -DDEBUG=$$DEBUG
else
	override CXXFLAGS+= -O3
	override LINKERFLAGS+= -Wl,-s
endif

override CXXFLAGS+= -Wall -fsigned-char -fno-exceptions -fno-rtti
ifneq (,$(findstring macos,$(TARGET)))
	OSXMIN=10.12 # lowest supported by EOS
	override CFLAGS+= -mmacosx-version-min=$(OSXMIN)
	override CXXFLAGS+= -mmacosx-version-min=$(OSXMIN)
	override LINKERFLAGS+= -Wl,-rpath,@executable_path/../Frameworks
endif

ifdef ANTICHEAT
	override CXXFLAGS+= -DANTICHEAT
	ifneq (,$(findstring macos,$(TARGET)))
		override LINKERFLAGS+= -Wl,-rpath,@executable_path/../Libraries
	endif
	ifneq (,$(findstring linux,$(TARGET)))
		override LINKERFLAGS+= -Wl,-rpath,'$$ORIGIN'
	endif
endif

INCLUDES= -Ishared -Iengine -Ifpsgame -Ip1xbraten -Ienet/include
ifdef ANTICHEAT
	INCLUDES+= -Ianticheat -IEOS/include
endif
LIBS= -Lenet -lenet

ifneq (,$(findstring windows,$(TARGET)))
	INCLUDES+= -Iwindows/include/zlib
	LIBS+= -L../bin64 -Lwindows/lib64 -lzlib1 -lws2_32 -lwinmm
	ifdef ANTICHEAT
		LIBS+= -lEOSSDK-Win64-Shipping
	endif
	CLIENT_INCLUDES= -Iwindows/include/SDL2
	CLIENT_LIBS= -lSDL2 -lSDL2_image -lSDL2_mixer -lopengl32
endif

ifneq (,$(findstring macos,$(TARGET)))
	INCLUDES+= -Imacos/sdk_11.3/usr/include
	LIBS+= -Lmacos/sdk_11.3/usr/lib -lz
	ifdef ANTICHEAT
		LIBS+= -L../sauerbraten.app/Contents/Libraries -lEOSSDK-Mac-Shipping
	endif
	CLIENT_INCLUDES= -Imacos/include
	CLIENT_LIBS= -F./macos/sdk_11.3/System/Library/Frameworks -framework OpenGL
	CLIENT_LIBS+= -F../sauerbraten.app/Contents/Frameworks -framework SDL2 -framework SDL2_image -framework SDL2_mixer
endif

ifneq (,$(findstring linux,$(TARGET)))
	INCLUDES+= -I/usr/include
	LIBS+= -L/usr/lib64 -lz
	ifdef ANTICHEAT
		LIBS+= -L../bin_unix -lEOSSDK-Linux-Shipping
	endif
	CLIENT_INCLUDES= `sdl2-config --cflags`
	CLIENT_LIBS= -lX11 `sdl2-config --libs` -lSDL2_image -lSDL2_mixer -lGL -lrt
endif

ifndef CLIENT_INCLUDES
$(error unsupported target $(TARGET))
endif

CLIENT_OBJS= \
	shared/crypto.o \
	shared/geom.o \
	shared/glemu.o \
	shared/stream.o \
	shared/tools.o \
	shared/zip.o \
	engine/3dgui.o \
	engine/bih.o \
	engine/blend.o \
	engine/blob.o \
	engine/client.o	\
	engine/command.o \
	engine/console.o \
	engine/cubeloader.o \
	engine/decal.o \
	engine/dynlight.o \
	engine/glare.o \
	engine/grass.o \
	engine/lightmap.o \
	engine/main.o \
	engine/material.o \
	engine/menus.o \
	engine/movie.o \
	engine/normal.o	\
	engine/octa.o \
	engine/octaedit.o \
	engine/octarender.o \
	engine/physics.o \
	engine/pvs.o \
	engine/rendergl.o \
	engine/rendermodel.o \
	engine/renderparticles.o \
	engine/rendersky.o \
	engine/rendertext.o \
	engine/renderva.o \
	engine/server.o	\
	engine/serverbrowser.o \
	engine/shader.o \
	engine/shadowmap.o \
	engine/sound.o \
	engine/texture.o \
	engine/water.o \
	engine/world.o \
	engine/worldio.o \
	fpsgame/ai.o \
	fpsgame/client.o \
	fpsgame/entities.o \
	fpsgame/fps.o \
	fpsgame/monster.o \
	fpsgame/movable.o \
	fpsgame/render.o \
	fpsgame/scoreboard.o \
	fpsgame/server.o \
	fpsgame/waypoint.o \
	fpsgame/weapon.o \
	p1xbraten/bans.o \
	p1xbraten/capability_probing.o \
	p1xbraten/clientdemo.o \
	p1xbraten/demo_info_message.o \
	p1xbraten/detection.o \
	p1xbraten/embedded_cfgs.o \
	p1xbraten/fragmessages.o \
	p1xbraten/fullscreenmonitor.o \
	p1xbraten/managed_games.o \
	p1xbraten/namecomplete.o \
	p1xbraten/proxy_real_ip.o \
	p1xbraten/remote_commands.o \
	p1xbraten/scoreboard.o \
	p1xbraten/serverbrowser.o \
	p1xbraten/version.o \
	p1xbraten/weaponstats.o
ifdef ANTICHEAT
	override CLIENT_OBJS+= anticheat/anticheat.o
endif

SERVER_OBJS= \
	shared/crypto-standalone.o \
	shared/stream-standalone.o \
	shared/tools-standalone.o \
	engine/command-standalone.o \
	engine/server-standalone.o \
	engine/worldio-standalone.o \
	fpsgame/entities-standalone.o \
	fpsgame/server-standalone.o \
	p1xbraten/bans-standalone.o \
	p1xbraten/demo_info_message-standalone.o \
	p1xbraten/managed_games-standalone.o \
	p1xbraten/proxy_real_ip-standalone.o \
	p1xbraten/remote_commands-standalone.o \
	p1xbraten/version-standalone.o
ifdef ANTICHEAT
	override SERVER_OBJS+= anticheat/anticheat-standalone.o
endif

default: all

all: client server

clean:
	cd enet && make clean && cd ..
	-$(RM) $(CLIENT_OBJS) $(SERVER_OBJS) sauer_client sauer_server

%-standalone.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(CLIENT_OBJS): CXXFLAGS += $(INCLUDES) $(CLIENT_INCLUDES)

$(SERVER_OBJS): CXXFLAGS += $(INCLUDES) -DSTANDALONE

OUTBIN=p1xbraten_x86_64

client:	libenet
client: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(LINKERFLAGS) -o sauer_client $(CLIENT_OBJS) $(LIBS) $(CLIENT_LIBS)

server:	libenet
server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(LINKERFLAGS) -o sauer_server $(SERVER_OBJS) $(LIBS)

install: all
ifneq (,$(findstring windows,$(TARGET)))
	cp sauer_client ../bin64/$(OUTBIN)_client.exe
	cp sauer_server ../bin64/$(OUTBIN)_server.exe
else
ifneq (,$(findstring macos,$(TARGET)))
	cp sauer_client ../sauerbraten.app/Contents/MacOS/$(OUTBIN)_client
	cp sauer_server ../sauerbraten.app/Contents/MacOS/$(OUTBIN)_server
else # linux
	cp sauer_client ../bin_unix/$(OUTBIN)_client
	cp sauer_server ../bin_unix/$(OUTBIN)_server
endif
endif

enet/libenet.a:
	$(MAKE) -C enet CC='$(CC)' AR='$(AR)'
libenet: enet/libenet.a

.PHONY: libenet

depend:
	makedepend -Y -Ishared -Iengine -Ifpsgame -I p1xbraten $(CLIENT_OBJS:.o=.cpp)
	makedepend -a -o-standalone.o -Y -DSTANDALONE -Ishared -Iengine -Ifpsgame -Ip1xbraten $(SERVER_OBJS:-standalone.o=.cpp)

# DO NOT DELETE

shared/crypto.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/crypto.o: shared/command.h p1xbraten/colors.h shared/glexts.h
shared/crypto.o: shared/glemu.h shared/iengine.h shared/igame.h
shared/crypto.o: p1xbraten/authservers.h p1xbraten/masterserver.h
shared/geom.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/geom.o: shared/command.h p1xbraten/colors.h shared/glexts.h
shared/geom.o: shared/glemu.h shared/iengine.h shared/igame.h
shared/geom.o: p1xbraten/authservers.h p1xbraten/masterserver.h
shared/glemu.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/glemu.o: shared/command.h p1xbraten/colors.h shared/glexts.h
shared/glemu.o: shared/glemu.h shared/iengine.h shared/igame.h
shared/glemu.o: p1xbraten/authservers.h p1xbraten/masterserver.h
shared/stream.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/stream.o: shared/command.h p1xbraten/colors.h shared/glexts.h
shared/stream.o: shared/glemu.h shared/iengine.h shared/igame.h
shared/stream.o: p1xbraten/authservers.h p1xbraten/masterserver.h
shared/tools.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/tools.o: shared/command.h p1xbraten/colors.h shared/glexts.h
shared/tools.o: shared/glemu.h shared/iengine.h shared/igame.h
shared/tools.o: p1xbraten/authservers.h p1xbraten/masterserver.h
shared/zip.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/zip.o: shared/command.h p1xbraten/colors.h shared/glexts.h
shared/zip.o: shared/glemu.h shared/iengine.h shared/igame.h
shared/zip.o: p1xbraten/authservers.h p1xbraten/masterserver.h
engine/3dgui.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/3dgui.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/3dgui.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/3dgui.o: shared/igame.h p1xbraten/authservers.h
engine/3dgui.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/3dgui.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/3dgui.o: engine/model.h engine/textedit.h
engine/bih.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/bih.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/bih.o: shared/glexts.h shared/glemu.h shared/iengine.h shared/igame.h
engine/bih.o: p1xbraten/authservers.h p1xbraten/masterserver.h engine/world.h
engine/bih.o: engine/octa.h engine/lightmap.h engine/bih.h engine/texture.h
engine/bih.o: engine/model.h
engine/blend.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/blend.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/blend.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/blend.o: shared/igame.h p1xbraten/authservers.h
engine/blend.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/blend.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/blend.o: engine/model.h
engine/blob.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/blob.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/blob.o: shared/glexts.h shared/glemu.h shared/iengine.h shared/igame.h
engine/blob.o: p1xbraten/authservers.h p1xbraten/masterserver.h
engine/blob.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/blob.o: engine/texture.h engine/model.h
engine/client.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/client.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/client.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/client.o: shared/igame.h p1xbraten/authservers.h
engine/client.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/client.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/client.o: engine/model.h
engine/command.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/command.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/command.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/command.o: shared/igame.h p1xbraten/authservers.h
engine/command.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/command.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/command.o: engine/model.h
engine/console.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/console.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/console.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/console.o: shared/igame.h p1xbraten/authservers.h
engine/console.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/console.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/console.o: engine/model.h
engine/cubeloader.o: engine/engine.h shared/cube.h shared/tools.h
engine/cubeloader.o: shared/geom.h shared/ents.h shared/command.h
engine/cubeloader.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
engine/cubeloader.o: shared/iengine.h shared/igame.h p1xbraten/authservers.h
engine/cubeloader.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/cubeloader.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/cubeloader.o: engine/model.h
engine/decal.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/decal.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/decal.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/decal.o: shared/igame.h p1xbraten/authservers.h
engine/decal.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/decal.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/decal.o: engine/model.h
engine/dynlight.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/dynlight.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/dynlight.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/dynlight.o: shared/igame.h p1xbraten/authservers.h
engine/dynlight.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/dynlight.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/dynlight.o: engine/model.h
engine/glare.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/glare.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/glare.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/glare.o: shared/igame.h p1xbraten/authservers.h
engine/glare.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/glare.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/glare.o: engine/model.h engine/rendertarget.h
engine/grass.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/grass.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/grass.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/grass.o: shared/igame.h p1xbraten/authservers.h
engine/grass.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/grass.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/grass.o: engine/model.h
engine/lightmap.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/lightmap.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/lightmap.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/lightmap.o: shared/igame.h p1xbraten/authservers.h
engine/lightmap.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/lightmap.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/lightmap.o: engine/model.h
engine/main.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/main.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/main.o: shared/glexts.h shared/glemu.h shared/iengine.h shared/igame.h
engine/main.o: p1xbraten/authservers.h p1xbraten/masterserver.h
engine/main.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/main.o: engine/texture.h engine/model.h
engine/material.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/material.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/material.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/material.o: shared/igame.h p1xbraten/authservers.h
engine/material.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/material.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/material.o: engine/model.h
engine/menus.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/menus.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/menus.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/menus.o: shared/igame.h p1xbraten/authservers.h
engine/menus.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/menus.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/menus.o: engine/model.h
engine/movie.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/movie.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/movie.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/movie.o: shared/igame.h p1xbraten/authservers.h
engine/movie.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/movie.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/movie.o: engine/model.h
engine/normal.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/normal.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/normal.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/normal.o: shared/igame.h p1xbraten/authservers.h
engine/normal.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/normal.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/normal.o: engine/model.h
engine/octa.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/octa.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/octa.o: shared/glexts.h shared/glemu.h shared/iengine.h shared/igame.h
engine/octa.o: p1xbraten/authservers.h p1xbraten/masterserver.h
engine/octa.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/octa.o: engine/texture.h engine/model.h
engine/octaedit.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/octaedit.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/octaedit.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/octaedit.o: shared/igame.h p1xbraten/authservers.h
engine/octaedit.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/octaedit.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/octaedit.o: engine/model.h
engine/octarender.o: engine/engine.h shared/cube.h shared/tools.h
engine/octarender.o: shared/geom.h shared/ents.h shared/command.h
engine/octarender.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
engine/octarender.o: shared/iengine.h shared/igame.h p1xbraten/authservers.h
engine/octarender.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/octarender.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/octarender.o: engine/model.h
engine/physics.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/physics.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/physics.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/physics.o: shared/igame.h p1xbraten/authservers.h
engine/physics.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/physics.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/physics.o: engine/model.h engine/mpr.h
engine/pvs.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/pvs.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/pvs.o: shared/glexts.h shared/glemu.h shared/iengine.h shared/igame.h
engine/pvs.o: p1xbraten/authservers.h p1xbraten/masterserver.h engine/world.h
engine/pvs.o: engine/octa.h engine/lightmap.h engine/bih.h engine/texture.h
engine/pvs.o: engine/model.h
engine/rendergl.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/rendergl.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/rendergl.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/rendergl.o: shared/igame.h p1xbraten/authservers.h
engine/rendergl.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/rendergl.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/rendergl.o: engine/model.h
engine/rendermodel.o: engine/engine.h shared/cube.h shared/tools.h
engine/rendermodel.o: shared/geom.h shared/ents.h shared/command.h
engine/rendermodel.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
engine/rendermodel.o: shared/iengine.h shared/igame.h p1xbraten/authservers.h
engine/rendermodel.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/rendermodel.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/rendermodel.o: engine/model.h engine/ragdoll.h engine/animmodel.h
engine/rendermodel.o: engine/vertmodel.h engine/skelmodel.h engine/md2.h
engine/rendermodel.o: engine/md3.h engine/md5.h engine/obj.h engine/smd.h
engine/rendermodel.o: engine/iqm.h
engine/renderparticles.o: engine/engine.h shared/cube.h shared/tools.h
engine/renderparticles.o: shared/geom.h shared/ents.h shared/command.h
engine/renderparticles.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
engine/renderparticles.o: shared/iengine.h shared/igame.h
engine/renderparticles.o: p1xbraten/authservers.h p1xbraten/masterserver.h
engine/renderparticles.o: engine/world.h engine/octa.h engine/lightmap.h
engine/renderparticles.o: engine/bih.h engine/texture.h engine/model.h
engine/renderparticles.o: engine/rendertarget.h engine/depthfx.h
engine/renderparticles.o: engine/explosion.h engine/lensflare.h
engine/renderparticles.o: engine/lightning.h
engine/rendersky.o: engine/engine.h shared/cube.h shared/tools.h
engine/rendersky.o: shared/geom.h shared/ents.h shared/command.h
engine/rendersky.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
engine/rendersky.o: shared/iengine.h shared/igame.h p1xbraten/authservers.h
engine/rendersky.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/rendersky.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/rendersky.o: engine/model.h
engine/rendertext.o: engine/engine.h shared/cube.h shared/tools.h
engine/rendertext.o: shared/geom.h shared/ents.h shared/command.h
engine/rendertext.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
engine/rendertext.o: shared/iengine.h shared/igame.h p1xbraten/authservers.h
engine/rendertext.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/rendertext.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/rendertext.o: engine/model.h
engine/renderva.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/renderva.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/renderva.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/renderva.o: shared/igame.h p1xbraten/authservers.h
engine/renderva.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/renderva.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/renderva.o: engine/model.h
engine/server.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/server.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/server.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/server.o: shared/igame.h p1xbraten/authservers.h
engine/server.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/server.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/server.o: engine/model.h
engine/serverbrowser.o: engine/engine.h shared/cube.h shared/tools.h
engine/serverbrowser.o: shared/geom.h shared/ents.h shared/command.h
engine/serverbrowser.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
engine/serverbrowser.o: shared/iengine.h shared/igame.h
engine/serverbrowser.o: p1xbraten/authservers.h p1xbraten/masterserver.h
engine/serverbrowser.o: engine/world.h engine/octa.h engine/lightmap.h
engine/serverbrowser.o: engine/bih.h engine/texture.h engine/model.h
engine/serverbrowser.o: p1xbraten/serverbrowser.h
engine/shader.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/shader.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/shader.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/shader.o: shared/igame.h p1xbraten/authservers.h
engine/shader.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/shader.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/shader.o: engine/model.h
engine/shadowmap.o: engine/engine.h shared/cube.h shared/tools.h
engine/shadowmap.o: shared/geom.h shared/ents.h shared/command.h
engine/shadowmap.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
engine/shadowmap.o: shared/iengine.h shared/igame.h p1xbraten/authservers.h
engine/shadowmap.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/shadowmap.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/shadowmap.o: engine/model.h engine/rendertarget.h
engine/sound.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/sound.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/sound.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/sound.o: shared/igame.h p1xbraten/authservers.h
engine/sound.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/sound.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/sound.o: engine/model.h
engine/texture.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/texture.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/texture.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/texture.o: shared/igame.h p1xbraten/authservers.h
engine/texture.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/texture.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/texture.o: engine/model.h
engine/water.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/water.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/water.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/water.o: shared/igame.h p1xbraten/authservers.h
engine/water.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/water.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/water.o: engine/model.h
engine/world.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/world.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/world.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/world.o: shared/igame.h p1xbraten/authservers.h
engine/world.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/world.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/world.o: engine/model.h
engine/worldio.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/worldio.o: shared/ents.h shared/command.h p1xbraten/colors.h
engine/worldio.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/worldio.o: shared/igame.h p1xbraten/authservers.h
engine/worldio.o: p1xbraten/masterserver.h engine/world.h engine/octa.h
engine/worldio.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/worldio.o: engine/model.h
fpsgame/ai.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/ai.o: shared/ents.h shared/command.h p1xbraten/colors.h
fpsgame/ai.o: shared/glexts.h shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/ai.o: p1xbraten/authservers.h p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/ai.o: p1xbraten/weaponstats_type.h p1xbraten/fragmessages.h
fpsgame/client.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/client.o: shared/ents.h shared/command.h p1xbraten/colors.h
fpsgame/client.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/client.o: shared/igame.h p1xbraten/authservers.h
fpsgame/client.o: p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/client.o: p1xbraten/weaponstats_type.h p1xbraten/fragmessages.h
fpsgame/client.o: p1xbraten/weaponstats.h fpsgame/capture.h fpsgame/ctf.h
fpsgame/client.o: fpsgame/collect.h
fpsgame/entities.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/entities.o: shared/ents.h shared/command.h p1xbraten/colors.h
fpsgame/entities.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/entities.o: shared/igame.h p1xbraten/authservers.h
fpsgame/entities.o: p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/entities.o: p1xbraten/weaponstats_type.h p1xbraten/fragmessages.h
fpsgame/fps.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/fps.o: shared/ents.h shared/command.h p1xbraten/colors.h
fpsgame/fps.o: shared/glexts.h shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/fps.o: p1xbraten/authservers.h p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/fps.o: p1xbraten/weaponstats_type.h p1xbraten/fragmessages.h
fpsgame/fps.o: p1xbraten/weaponstats.h
fpsgame/monster.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/monster.o: shared/ents.h shared/command.h p1xbraten/colors.h
fpsgame/monster.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/monster.o: shared/igame.h p1xbraten/authservers.h
fpsgame/monster.o: p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/monster.o: p1xbraten/weaponstats_type.h p1xbraten/fragmessages.h
fpsgame/movable.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/movable.o: shared/ents.h shared/command.h p1xbraten/colors.h
fpsgame/movable.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/movable.o: shared/igame.h p1xbraten/authservers.h
fpsgame/movable.o: p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/movable.o: p1xbraten/weaponstats_type.h p1xbraten/fragmessages.h
fpsgame/render.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/render.o: shared/ents.h shared/command.h p1xbraten/colors.h
fpsgame/render.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/render.o: shared/igame.h p1xbraten/authservers.h
fpsgame/render.o: p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/render.o: p1xbraten/weaponstats_type.h p1xbraten/fragmessages.h
fpsgame/scoreboard.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/scoreboard.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/scoreboard.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
fpsgame/scoreboard.o: shared/iengine.h shared/igame.h p1xbraten/authservers.h
fpsgame/scoreboard.o: p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/scoreboard.o: p1xbraten/weaponstats_type.h p1xbraten/fragmessages.h
fpsgame/server.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/server.o: shared/ents.h shared/command.h p1xbraten/colors.h
fpsgame/server.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/server.o: shared/igame.h p1xbraten/authservers.h
fpsgame/server.o: p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/server.o: p1xbraten/weaponstats_type.h p1xbraten/fragmessages.h
fpsgame/server.o: fpsgame/capture.h fpsgame/ctf.h fpsgame/collect.h
fpsgame/server.o: fpsgame/extinfo.h fpsgame/aiman.h
fpsgame/waypoint.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/waypoint.o: shared/ents.h shared/command.h p1xbraten/colors.h
fpsgame/waypoint.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/waypoint.o: shared/igame.h p1xbraten/authservers.h
fpsgame/waypoint.o: p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/waypoint.o: p1xbraten/weaponstats_type.h p1xbraten/fragmessages.h
fpsgame/weapon.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/weapon.o: shared/ents.h shared/command.h p1xbraten/colors.h
fpsgame/weapon.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/weapon.o: shared/igame.h p1xbraten/authservers.h
fpsgame/weapon.o: p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/weapon.o: p1xbraten/weaponstats_type.h p1xbraten/fragmessages.h
fpsgame/weapon.o: p1xbraten/weaponstats.h
p1xbraten/bans.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
p1xbraten/bans.o: shared/ents.h shared/command.h p1xbraten/colors.h
p1xbraten/bans.o: shared/glexts.h shared/glemu.h shared/iengine.h
p1xbraten/bans.o: shared/igame.h p1xbraten/authservers.h
p1xbraten/bans.o: p1xbraten/masterserver.h fpsgame/ai.h
p1xbraten/bans.o: p1xbraten/weaponstats_type.h p1xbraten/fragmessages.h
p1xbraten/capability_probing.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/capability_probing.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/capability_probing.o: p1xbraten/colors.h shared/glexts.h
p1xbraten/capability_probing.o: shared/glemu.h shared/iengine.h
p1xbraten/capability_probing.o: shared/igame.h p1xbraten/authservers.h
p1xbraten/capability_probing.o: p1xbraten/masterserver.h fpsgame/ai.h
p1xbraten/capability_probing.o: p1xbraten/weaponstats_type.h
p1xbraten/capability_probing.o: p1xbraten/fragmessages.h
p1xbraten/clientdemo.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/clientdemo.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/clientdemo.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
p1xbraten/clientdemo.o: shared/iengine.h shared/igame.h
p1xbraten/clientdemo.o: p1xbraten/authservers.h p1xbraten/masterserver.h
p1xbraten/clientdemo.o: fpsgame/ai.h p1xbraten/weaponstats_type.h
p1xbraten/clientdemo.o: p1xbraten/fragmessages.h
p1xbraten/demo_info_message.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/demo_info_message.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/demo_info_message.o: p1xbraten/colors.h shared/glexts.h
p1xbraten/demo_info_message.o: shared/glemu.h shared/iengine.h shared/igame.h
p1xbraten/demo_info_message.o: p1xbraten/authservers.h
p1xbraten/demo_info_message.o: p1xbraten/masterserver.h fpsgame/ai.h
p1xbraten/demo_info_message.o: p1xbraten/weaponstats_type.h
p1xbraten/demo_info_message.o: p1xbraten/fragmessages.h
p1xbraten/detection.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/detection.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/detection.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
p1xbraten/detection.o: shared/iengine.h shared/igame.h
p1xbraten/detection.o: p1xbraten/authservers.h p1xbraten/masterserver.h
p1xbraten/detection.o: fpsgame/ai.h p1xbraten/weaponstats_type.h
p1xbraten/detection.o: p1xbraten/fragmessages.h
p1xbraten/embedded_cfgs.o: shared/cube.h shared/tools.h shared/geom.h
p1xbraten/embedded_cfgs.o: shared/ents.h shared/command.h p1xbraten/colors.h
p1xbraten/embedded_cfgs.o: shared/glexts.h shared/glemu.h shared/iengine.h
p1xbraten/embedded_cfgs.o: shared/igame.h p1xbraten/authservers.h
p1xbraten/embedded_cfgs.o: p1xbraten/masterserver.h p1xbraten/embedded_cfgs.h
p1xbraten/embedded_cfgs.o: ../data/p1xbraten/menus.cfg.gz.xxd
p1xbraten/embedded_cfgs.o: ../data/p1xbraten/master.cfg.gz.xxd
p1xbraten/embedded_cfgs.o: ../data/p1xbraten/gamehud.cfg.gz.xxd
p1xbraten/embedded_cfgs.o: ../data/p1xbraten/keymap.cfg.gz.xxd
p1xbraten/fragmessages.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/fragmessages.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/fragmessages.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
p1xbraten/fragmessages.o: shared/iengine.h shared/igame.h
p1xbraten/fragmessages.o: p1xbraten/authservers.h p1xbraten/masterserver.h
p1xbraten/fragmessages.o: fpsgame/ai.h p1xbraten/weaponstats_type.h
p1xbraten/fragmessages.o: p1xbraten/fragmessages.h
p1xbraten/fullscreenmonitor.o: engine/engine.h shared/cube.h shared/tools.h
p1xbraten/fullscreenmonitor.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/fullscreenmonitor.o: p1xbraten/colors.h shared/glexts.h
p1xbraten/fullscreenmonitor.o: shared/glemu.h shared/iengine.h shared/igame.h
p1xbraten/fullscreenmonitor.o: p1xbraten/authservers.h
p1xbraten/fullscreenmonitor.o: p1xbraten/masterserver.h engine/world.h
p1xbraten/fullscreenmonitor.o: engine/octa.h engine/lightmap.h engine/bih.h
p1xbraten/fullscreenmonitor.o: engine/texture.h engine/model.h
p1xbraten/managed_games.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/managed_games.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/managed_games.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
p1xbraten/managed_games.o: shared/iengine.h shared/igame.h
p1xbraten/managed_games.o: p1xbraten/authservers.h p1xbraten/masterserver.h
p1xbraten/managed_games.o: fpsgame/ai.h p1xbraten/weaponstats_type.h
p1xbraten/managed_games.o: p1xbraten/fragmessages.h
p1xbraten/namecomplete.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/namecomplete.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/namecomplete.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
p1xbraten/namecomplete.o: shared/iengine.h shared/igame.h
p1xbraten/namecomplete.o: p1xbraten/authservers.h p1xbraten/masterserver.h
p1xbraten/namecomplete.o: fpsgame/ai.h p1xbraten/weaponstats_type.h
p1xbraten/namecomplete.o: p1xbraten/fragmessages.h
p1xbraten/proxy_real_ip.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/proxy_real_ip.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/proxy_real_ip.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
p1xbraten/proxy_real_ip.o: shared/iengine.h shared/igame.h
p1xbraten/proxy_real_ip.o: p1xbraten/authservers.h p1xbraten/masterserver.h
p1xbraten/proxy_real_ip.o: fpsgame/ai.h p1xbraten/weaponstats_type.h
p1xbraten/proxy_real_ip.o: p1xbraten/fragmessages.h
p1xbraten/remote_commands.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/remote_commands.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/remote_commands.o: p1xbraten/colors.h shared/glexts.h
p1xbraten/remote_commands.o: shared/glemu.h shared/iengine.h shared/igame.h
p1xbraten/remote_commands.o: p1xbraten/authservers.h p1xbraten/masterserver.h
p1xbraten/remote_commands.o: fpsgame/ai.h p1xbraten/weaponstats_type.h
p1xbraten/remote_commands.o: p1xbraten/fragmessages.h
p1xbraten/scoreboard.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/scoreboard.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/scoreboard.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
p1xbraten/scoreboard.o: shared/iengine.h shared/igame.h
p1xbraten/scoreboard.o: p1xbraten/authservers.h p1xbraten/masterserver.h
p1xbraten/scoreboard.o: fpsgame/ai.h p1xbraten/weaponstats_type.h
p1xbraten/scoreboard.o: p1xbraten/fragmessages.h p1xbraten/weaponstats.h
p1xbraten/serverbrowser.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/serverbrowser.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/serverbrowser.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
p1xbraten/serverbrowser.o: shared/iengine.h shared/igame.h
p1xbraten/serverbrowser.o: p1xbraten/authservers.h p1xbraten/masterserver.h
p1xbraten/serverbrowser.o: fpsgame/ai.h p1xbraten/weaponstats_type.h
p1xbraten/serverbrowser.o: p1xbraten/fragmessages.h p1xbraten/serverbrowser.h
p1xbraten/serverbrowser.o: engine/engine.h engine/world.h engine/octa.h
p1xbraten/serverbrowser.o: engine/lightmap.h engine/bih.h engine/texture.h
p1xbraten/serverbrowser.o: engine/model.h
p1xbraten/version.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
p1xbraten/version.o: shared/command.h p1xbraten/colors.h shared/glexts.h
p1xbraten/version.o: shared/glemu.h shared/iengine.h shared/igame.h
p1xbraten/version.o: p1xbraten/authservers.h p1xbraten/masterserver.h
p1xbraten/weaponstats.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/weaponstats.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/weaponstats.o: p1xbraten/colors.h shared/glexts.h shared/glemu.h
p1xbraten/weaponstats.o: shared/iengine.h shared/igame.h
p1xbraten/weaponstats.o: p1xbraten/authservers.h p1xbraten/masterserver.h
p1xbraten/weaponstats.o: fpsgame/ai.h p1xbraten/weaponstats_type.h
p1xbraten/weaponstats.o: p1xbraten/fragmessages.h p1xbraten/weaponstats.h

shared/crypto-standalone.o: shared/cube.h shared/tools.h shared/geom.h
shared/crypto-standalone.o: shared/ents.h shared/command.h p1xbraten/colors.h
shared/crypto-standalone.o: shared/iengine.h shared/igame.h
shared/crypto-standalone.o: p1xbraten/authservers.h p1xbraten/masterserver.h
shared/stream-standalone.o: shared/cube.h shared/tools.h shared/geom.h
shared/stream-standalone.o: shared/ents.h shared/command.h p1xbraten/colors.h
shared/stream-standalone.o: shared/iengine.h shared/igame.h
shared/stream-standalone.o: p1xbraten/authservers.h p1xbraten/masterserver.h
shared/tools-standalone.o: shared/cube.h shared/tools.h shared/geom.h
shared/tools-standalone.o: shared/ents.h shared/command.h p1xbraten/colors.h
shared/tools-standalone.o: shared/iengine.h shared/igame.h
shared/tools-standalone.o: p1xbraten/authservers.h p1xbraten/masterserver.h
engine/command-standalone.o: engine/engine.h shared/cube.h shared/tools.h
engine/command-standalone.o: shared/geom.h shared/ents.h shared/command.h
engine/command-standalone.o: p1xbraten/colors.h shared/iengine.h
engine/command-standalone.o: shared/igame.h p1xbraten/authservers.h
engine/command-standalone.o: p1xbraten/masterserver.h engine/world.h
engine/server-standalone.o: engine/engine.h shared/cube.h shared/tools.h
engine/server-standalone.o: shared/geom.h shared/ents.h shared/command.h
engine/server-standalone.o: p1xbraten/colors.h shared/iengine.h
engine/server-standalone.o: shared/igame.h p1xbraten/authservers.h
engine/server-standalone.o: p1xbraten/masterserver.h engine/world.h
engine/worldio-standalone.o: engine/engine.h shared/cube.h shared/tools.h
engine/worldio-standalone.o: shared/geom.h shared/ents.h shared/command.h
engine/worldio-standalone.o: p1xbraten/colors.h shared/iengine.h
engine/worldio-standalone.o: shared/igame.h p1xbraten/authservers.h
engine/worldio-standalone.o: p1xbraten/masterserver.h engine/world.h
fpsgame/entities-standalone.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/entities-standalone.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/entities-standalone.o: p1xbraten/colors.h shared/iengine.h
fpsgame/entities-standalone.o: shared/igame.h p1xbraten/authservers.h
fpsgame/entities-standalone.o: p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/entities-standalone.o: p1xbraten/weaponstats_type.h
fpsgame/entities-standalone.o: p1xbraten/fragmessages.h
fpsgame/server-standalone.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/server-standalone.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/server-standalone.o: p1xbraten/colors.h shared/iengine.h
fpsgame/server-standalone.o: shared/igame.h p1xbraten/authservers.h
fpsgame/server-standalone.o: p1xbraten/masterserver.h fpsgame/ai.h
fpsgame/server-standalone.o: p1xbraten/weaponstats_type.h
fpsgame/server-standalone.o: p1xbraten/fragmessages.h fpsgame/capture.h
fpsgame/server-standalone.o: fpsgame/ctf.h fpsgame/collect.h
fpsgame/server-standalone.o: fpsgame/extinfo.h fpsgame/aiman.h
p1xbraten/bans-standalone.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/bans-standalone.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/bans-standalone.o: p1xbraten/colors.h shared/iengine.h
p1xbraten/bans-standalone.o: shared/igame.h p1xbraten/authservers.h
p1xbraten/bans-standalone.o: p1xbraten/masterserver.h fpsgame/ai.h
p1xbraten/bans-standalone.o: p1xbraten/weaponstats_type.h
p1xbraten/bans-standalone.o: p1xbraten/fragmessages.h
p1xbraten/demo_info_message-standalone.o: fpsgame/game.h shared/cube.h
p1xbraten/demo_info_message-standalone.o: shared/tools.h shared/geom.h
p1xbraten/demo_info_message-standalone.o: shared/ents.h shared/command.h
p1xbraten/demo_info_message-standalone.o: p1xbraten/colors.h shared/iengine.h
p1xbraten/demo_info_message-standalone.o: shared/igame.h
p1xbraten/demo_info_message-standalone.o: p1xbraten/authservers.h
p1xbraten/demo_info_message-standalone.o: p1xbraten/masterserver.h
p1xbraten/demo_info_message-standalone.o: fpsgame/ai.h
p1xbraten/demo_info_message-standalone.o: p1xbraten/weaponstats_type.h
p1xbraten/demo_info_message-standalone.o: p1xbraten/fragmessages.h
p1xbraten/managed_games-standalone.o: fpsgame/game.h shared/cube.h
p1xbraten/managed_games-standalone.o: shared/tools.h shared/geom.h
p1xbraten/managed_games-standalone.o: shared/ents.h shared/command.h
p1xbraten/managed_games-standalone.o: p1xbraten/colors.h shared/iengine.h
p1xbraten/managed_games-standalone.o: shared/igame.h p1xbraten/authservers.h
p1xbraten/managed_games-standalone.o: p1xbraten/masterserver.h fpsgame/ai.h
p1xbraten/managed_games-standalone.o: p1xbraten/weaponstats_type.h
p1xbraten/managed_games-standalone.o: p1xbraten/fragmessages.h
p1xbraten/proxy_real_ip-standalone.o: fpsgame/game.h shared/cube.h
p1xbraten/proxy_real_ip-standalone.o: shared/tools.h shared/geom.h
p1xbraten/proxy_real_ip-standalone.o: shared/ents.h shared/command.h
p1xbraten/proxy_real_ip-standalone.o: p1xbraten/colors.h shared/iengine.h
p1xbraten/proxy_real_ip-standalone.o: shared/igame.h p1xbraten/authservers.h
p1xbraten/proxy_real_ip-standalone.o: p1xbraten/masterserver.h fpsgame/ai.h
p1xbraten/proxy_real_ip-standalone.o: p1xbraten/weaponstats_type.h
p1xbraten/proxy_real_ip-standalone.o: p1xbraten/fragmessages.h
p1xbraten/remote_commands-standalone.o: fpsgame/game.h shared/cube.h
p1xbraten/remote_commands-standalone.o: shared/tools.h shared/geom.h
p1xbraten/remote_commands-standalone.o: shared/ents.h shared/command.h
p1xbraten/remote_commands-standalone.o: p1xbraten/colors.h shared/iengine.h
p1xbraten/remote_commands-standalone.o: shared/igame.h
p1xbraten/remote_commands-standalone.o: p1xbraten/authservers.h
p1xbraten/remote_commands-standalone.o: p1xbraten/masterserver.h fpsgame/ai.h
p1xbraten/remote_commands-standalone.o: p1xbraten/weaponstats_type.h
p1xbraten/remote_commands-standalone.o: p1xbraten/fragmessages.h
p1xbraten/version-standalone.o: shared/cube.h shared/tools.h shared/geom.h
p1xbraten/version-standalone.o: shared/ents.h shared/command.h
p1xbraten/version-standalone.o: p1xbraten/colors.h shared/iengine.h
p1xbraten/version-standalone.o: shared/igame.h p1xbraten/authservers.h
p1xbraten/version-standalone.o: p1xbraten/masterserver.h
