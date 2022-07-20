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
	USEZIG=1
endif

ifdef USEZIG
	CC=zig cc
	CXX=zig c++
	AR=zig ar
else
	CC=clang
	CXX=clang++
endif

CFLAGS= --target=$(TARGET)
CXXFLAGS= --target=$(TARGET) -fomit-frame-pointer -ffast-math
ifdef DEBUG
	override CXXFLAGS+= -O0 -g -DDEBUG=$$DEBUG
else
	override CXXFLAGS+= -O3
endif
LINKERFLAGS=

override CXXFLAGS+= -Wall -fsigned-char -fno-exceptions -fno-rtti
ifneq (,$(findstring macos,$(TARGET)))
	OSXMIN=10.12 # lowest supported by EOS
	override CFLAGS+= -mmacosx-version-min=$(OSXMIN)
	override CXXFLAGS+= -mmacosx-version-min=$(OSXMIN)
	override LINKERFLAGS+= -Wl,-rpath,@executable_path/../Frameworks
endif

ifdef ANTICHEAT
	override CXXFLAGS+= -DANTICHEAT
	LINKERFLAGS= -Wl,-s
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

ifneq (,$(findstring windows,$(TARGET)))
	INCLUDES+= -Iwindows/include/zlib
	LIBS= -L../bin64 -Lwindows/lib64 -lzlib1 -lenet -lws2_32 -lwinmm
	ifdef ANTICHEAT
		LIBS+= -lEOSSDK-Win64-Shipping
	endif
	CLIENT_INCLUDES= -Iwindows/include/SDL2
	CLIENT_LIBS= -lSDL2 -lSDL2_image -lSDL2_mixer -lopengl32
endif

ifneq (,$(findstring macos,$(TARGET)))
	INCLUDES+= -Imacos/sdk_11.3/usr/include
	LIBS= -Lmacos/sdk_11.3/usr/lib -lz -Lmacos/lib -lenet
	ifdef ANTICHEAT
		LIBS+= -L../sauerbraten.app/Contents/Libraries -lEOSSDK-Mac-Shipping
	endif
	CLIENT_INCLUDES= -Imacos/include
	CLIENT_LIBS= -F./macos/sdk_11.3/System/Library/Frameworks -framework OpenGL
	CLIENT_LIBS+= -F../sauerbraten.app/Contents/Frameworks -framework SDL2 -framework SDL2_image -framework SDL2_mixer
endif

ifneq (,$(findstring linux,$(TARGET)))
	INCLUDES+= -I/usr/include
	LIBS= -Lenet -lenet -L/usr/lib64 -lz
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
	p1xbraten/capability_probing.o \
	p1xbraten/clientdemo.o \
	p1xbraten/demo_info_message.o \
	p1xbraten/detection.o \
	p1xbraten/embedded_cfgs.o \
	p1xbraten/fragmessages.o \
	p1xbraten/managed_games.o \
	p1xbraten/namecomplete.o \
	p1xbraten/proxy_real_ip.o \
	p1xbraten/remote_commands.o \
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
	p1xbraten/demo_info_message-standalone.o \
	p1xbraten/managed_games-standalone.o \
	p1xbraten/proxy_real_ip.o \
	p1xbraten/remote_commands-standalone.o
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

ifneq (,$(findstring linux,$(TARGET)))
client:	libenet
endif
client: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(LINKERFLAGS) -o sauer_client $(CLIENT_OBJS) $(LIBS) $(CLIENT_LIBS)

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
	$(MAKE) -C enet CC='$(CC)' CFLAGS='$(CFLAGS)' AR='$(AR)'
libenet: enet/libenet.a

.PHONY: libenet

depend:
	makedepend -Y -Ishared -Iengine -Ifpsgame $(CLIENT_OBJS:.o=.cpp)
	makedepend -a -o-standalone.o -Y -DSTANDALONE -Ishared -Iengine -Ifpsgame $(SERVER_OBJS:-standalone.o=.cpp)

# DO NOT DELETE

shared/crypto.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/crypto.o: shared/command.h shared/glexts.h shared/glemu.h
shared/crypto.o: shared/iengine.h shared/igame.h
shared/geom.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/geom.o: shared/command.h shared/glexts.h shared/glemu.h
shared/geom.o: shared/iengine.h shared/igame.h
shared/glemu.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/glemu.o: shared/command.h shared/glexts.h shared/glemu.h
shared/glemu.o: shared/iengine.h shared/igame.h
shared/stream.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/stream.o: shared/command.h shared/glexts.h shared/glemu.h
shared/stream.o: shared/iengine.h shared/igame.h
shared/tools.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/tools.o: shared/command.h shared/glexts.h shared/glemu.h
shared/tools.o: shared/iengine.h shared/igame.h
shared/zip.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/zip.o: shared/command.h shared/glexts.h shared/glemu.h
shared/zip.o: shared/iengine.h shared/igame.h
engine/3dgui.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/3dgui.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/3dgui.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/3dgui.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/3dgui.o: engine/model.h engine/textedit.h
engine/bih.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/bih.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/bih.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/bih.o: engine/lightmap.h engine/bih.h engine/texture.h engine/model.h
engine/blend.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/blend.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/blend.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/blend.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/blend.o: engine/model.h
engine/blob.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/blob.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/blob.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/blob.o: engine/lightmap.h engine/bih.h engine/texture.h engine/model.h
engine/client.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/client.o: shared/ents.h shared/command.h shared/glexts.h
engine/client.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/client.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/client.o: engine/texture.h engine/model.h
engine/command.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/command.o: shared/ents.h shared/command.h shared/glexts.h
engine/command.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/command.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/command.o: engine/texture.h engine/model.h
engine/console.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/console.o: shared/ents.h shared/command.h shared/glexts.h
engine/console.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/console.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/console.o: engine/texture.h engine/model.h
engine/cubeloader.o: engine/engine.h shared/cube.h shared/tools.h
engine/cubeloader.o: shared/geom.h shared/ents.h shared/command.h
engine/cubeloader.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/cubeloader.o: shared/igame.h engine/world.h engine/octa.h
engine/cubeloader.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/cubeloader.o: engine/model.h
engine/decal.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/decal.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/decal.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/decal.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/decal.o: engine/model.h
engine/dynlight.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/dynlight.o: shared/ents.h shared/command.h shared/glexts.h
engine/dynlight.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/dynlight.o: engine/world.h engine/octa.h engine/lightmap.h
engine/dynlight.o: engine/bih.h engine/texture.h engine/model.h
engine/glare.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/glare.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/glare.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/glare.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/glare.o: engine/model.h engine/rendertarget.h
engine/grass.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/grass.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/grass.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/grass.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/grass.o: engine/model.h
engine/lightmap.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/lightmap.o: shared/ents.h shared/command.h shared/glexts.h
engine/lightmap.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/lightmap.o: engine/world.h engine/octa.h engine/lightmap.h
engine/lightmap.o: engine/bih.h engine/texture.h engine/model.h
engine/main.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/main.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/main.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/main.o: engine/lightmap.h engine/bih.h engine/texture.h engine/model.h
engine/material.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/material.o: shared/ents.h shared/command.h shared/glexts.h
engine/material.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/material.o: engine/world.h engine/octa.h engine/lightmap.h
engine/material.o: engine/bih.h engine/texture.h engine/model.h
engine/menus.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/menus.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/menus.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/menus.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/menus.o: engine/model.h
engine/movie.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/movie.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/movie.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/movie.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/movie.o: engine/model.h
engine/normal.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/normal.o: shared/ents.h shared/command.h shared/glexts.h
engine/normal.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/normal.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/normal.o: engine/texture.h engine/model.h
engine/octa.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/octa.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/octa.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/octa.o: engine/lightmap.h engine/bih.h engine/texture.h engine/model.h
engine/octaedit.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/octaedit.o: shared/ents.h shared/command.h shared/glexts.h
engine/octaedit.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/octaedit.o: engine/world.h engine/octa.h engine/lightmap.h
engine/octaedit.o: engine/bih.h engine/texture.h engine/model.h
engine/octarender.o: engine/engine.h shared/cube.h shared/tools.h
engine/octarender.o: shared/geom.h shared/ents.h shared/command.h
engine/octarender.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/octarender.o: shared/igame.h engine/world.h engine/octa.h
engine/octarender.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/octarender.o: engine/model.h
engine/physics.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/physics.o: shared/ents.h shared/command.h shared/glexts.h
engine/physics.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/physics.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/physics.o: engine/texture.h engine/model.h engine/mpr.h
engine/pvs.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/pvs.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/pvs.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/pvs.o: engine/lightmap.h engine/bih.h engine/texture.h engine/model.h
engine/rendergl.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/rendergl.o: shared/ents.h shared/command.h shared/glexts.h
engine/rendergl.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/rendergl.o: engine/world.h engine/octa.h engine/lightmap.h
engine/rendergl.o: engine/bih.h engine/texture.h engine/model.h
engine/rendermodel.o: engine/engine.h shared/cube.h shared/tools.h
engine/rendermodel.o: shared/geom.h shared/ents.h shared/command.h
engine/rendermodel.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/rendermodel.o: shared/igame.h engine/world.h engine/octa.h
engine/rendermodel.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/rendermodel.o: engine/model.h engine/ragdoll.h engine/animmodel.h
engine/rendermodel.o: engine/vertmodel.h engine/skelmodel.h engine/md2.h
engine/rendermodel.o: engine/md3.h engine/md5.h engine/obj.h engine/smd.h
engine/rendermodel.o: engine/iqm.h
engine/renderparticles.o: engine/engine.h shared/cube.h shared/tools.h
engine/renderparticles.o: shared/geom.h shared/ents.h shared/command.h
engine/renderparticles.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/renderparticles.o: shared/igame.h engine/world.h engine/octa.h
engine/renderparticles.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/renderparticles.o: engine/model.h engine/rendertarget.h
engine/renderparticles.o: engine/depthfx.h engine/explosion.h
engine/renderparticles.o: engine/lensflare.h engine/lightning.h
engine/rendersky.o: engine/engine.h shared/cube.h shared/tools.h
engine/rendersky.o: shared/geom.h shared/ents.h shared/command.h
engine/rendersky.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/rendersky.o: shared/igame.h engine/world.h engine/octa.h
engine/rendersky.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/rendersky.o: engine/model.h
engine/rendertext.o: engine/engine.h shared/cube.h shared/tools.h
engine/rendertext.o: shared/geom.h shared/ents.h shared/command.h
engine/rendertext.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/rendertext.o: shared/igame.h engine/world.h engine/octa.h
engine/rendertext.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/rendertext.o: engine/model.h
engine/renderva.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/renderva.o: shared/ents.h shared/command.h shared/glexts.h
engine/renderva.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/renderva.o: engine/world.h engine/octa.h engine/lightmap.h
engine/renderva.o: engine/bih.h engine/texture.h engine/model.h
engine/server.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/server.o: shared/ents.h shared/command.h shared/glexts.h
engine/server.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/server.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/server.o: engine/texture.h engine/model.h
engine/serverbrowser.o: engine/engine.h shared/cube.h shared/tools.h
engine/serverbrowser.o: shared/geom.h shared/ents.h shared/command.h
engine/serverbrowser.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/serverbrowser.o: shared/igame.h engine/world.h engine/octa.h
engine/serverbrowser.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/serverbrowser.o: engine/model.h
engine/shader.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/shader.o: shared/ents.h shared/command.h shared/glexts.h
engine/shader.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/shader.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/shader.o: engine/texture.h engine/model.h
engine/shadowmap.o: engine/engine.h shared/cube.h shared/tools.h
engine/shadowmap.o: shared/geom.h shared/ents.h shared/command.h
engine/shadowmap.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/shadowmap.o: shared/igame.h engine/world.h engine/octa.h
engine/shadowmap.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/shadowmap.o: engine/model.h engine/rendertarget.h
engine/sound.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/sound.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/sound.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/sound.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/sound.o: engine/model.h
engine/texture.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/texture.o: shared/ents.h shared/command.h shared/glexts.h
engine/texture.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/texture.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/texture.o: engine/texture.h engine/model.h
engine/water.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/water.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/water.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/water.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/water.o: engine/model.h
engine/world.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/world.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/world.o: shared/iengine.h shared/igame.h engine/world.h engine/octa.h
engine/world.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/world.o: engine/model.h
engine/worldio.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/worldio.o: shared/ents.h shared/command.h shared/glexts.h
engine/worldio.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/worldio.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/worldio.o: engine/texture.h engine/model.h
fpsgame/ai.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/ai.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
fpsgame/ai.o: shared/iengine.h shared/igame.h fpsgame/ai.h
fpsgame/client.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/client.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/client.o: shared/glemu.h shared/iengine.h shared/igame.h fpsgame/ai.h
fpsgame/client.o: fpsgame/capture.h fpsgame/ctf.h fpsgame/collect.h
fpsgame/entities.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/entities.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/entities.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/entities.o: fpsgame/ai.h
fpsgame/fps.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/fps.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
fpsgame/fps.o: shared/iengine.h shared/igame.h fpsgame/ai.h
fpsgame/monster.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/monster.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/monster.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/monster.o: fpsgame/ai.h
fpsgame/movable.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/movable.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/movable.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/movable.o: fpsgame/ai.h
fpsgame/render.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/render.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/render.o: shared/glemu.h shared/iengine.h shared/igame.h fpsgame/ai.h
fpsgame/scoreboard.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/scoreboard.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/scoreboard.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/scoreboard.o: shared/igame.h fpsgame/ai.h
fpsgame/server.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/server.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/server.o: shared/glemu.h shared/iengine.h shared/igame.h fpsgame/ai.h
fpsgame/server.o: fpsgame/capture.h fpsgame/ctf.h fpsgame/collect.h
fpsgame/server.o: fpsgame/extinfo.h fpsgame/aiman.h
fpsgame/waypoint.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/waypoint.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/waypoint.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/waypoint.o: fpsgame/ai.h
fpsgame/weapon.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/weapon.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/weapon.o: shared/glemu.h shared/iengine.h shared/igame.h fpsgame/ai.h
p1xbraten/capability_probing.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/capability_probing.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/capability_probing.o: shared/glexts.h shared/glemu.h
p1xbraten/capability_probing.o: shared/iengine.h shared/igame.h fpsgame/ai.h
p1xbraten/clientdemo.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/clientdemo.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/clientdemo.o: shared/glexts.h shared/glemu.h shared/iengine.h
p1xbraten/clientdemo.o: shared/igame.h fpsgame/ai.h
p1xbraten/demo_info_message.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/demo_info_message.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/demo_info_message.o: shared/glexts.h shared/glemu.h
p1xbraten/demo_info_message.o: shared/iengine.h shared/igame.h fpsgame/ai.h
p1xbraten/detection.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/detection.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/detection.o: shared/glexts.h shared/glemu.h shared/iengine.h
p1xbraten/detection.o: shared/igame.h fpsgame/ai.h
p1xbraten/embedded_cfgs.o: shared/cube.h shared/tools.h shared/geom.h
p1xbraten/embedded_cfgs.o: shared/ents.h shared/command.h shared/glexts.h
p1xbraten/embedded_cfgs.o: shared/glemu.h shared/iengine.h shared/igame.h
p1xbraten/embedded_cfgs.o: p1xbraten/embedded_cfgs.h
p1xbraten/embedded_cfgs.o: ../data/p1xbraten/menus.cfg.gz.xxd
p1xbraten/embedded_cfgs.o: ../data/p1xbraten/master.cfg.gz.xxd
p1xbraten/embedded_cfgs.o: ../data/p1xbraten/gamehud.cfg.gz.xxd
p1xbraten/embedded_cfgs.o: ../data/p1xbraten/keymap.cfg.gz.xxd
p1xbraten/fragmessages.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/fragmessages.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/fragmessages.o: shared/glexts.h shared/glemu.h shared/iengine.h
p1xbraten/fragmessages.o: shared/igame.h fpsgame/ai.h
p1xbraten/managed_games.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/managed_games.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/managed_games.o: shared/glexts.h shared/glemu.h shared/iengine.h
p1xbraten/managed_games.o: shared/igame.h fpsgame/ai.h
p1xbraten/namecomplete.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/namecomplete.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/namecomplete.o: shared/glexts.h shared/glemu.h shared/iengine.h
p1xbraten/namecomplete.o: shared/igame.h fpsgame/ai.h
p1xbraten/proxy_real_ip.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/proxy_real_ip.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/proxy_real_ip.o: shared/glexts.h shared/glemu.h shared/iengine.h
p1xbraten/proxy_real_ip.o: shared/igame.h fpsgame/ai.h
p1xbraten/remote_commands.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/remote_commands.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/remote_commands.o: shared/glexts.h shared/glemu.h shared/iengine.h
p1xbraten/remote_commands.o: shared/igame.h fpsgame/ai.h
p1xbraten/version.o: engine/engine.h shared/cube.h shared/tools.h
p1xbraten/version.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/version.o: shared/glexts.h shared/glemu.h shared/iengine.h
p1xbraten/version.o: shared/igame.h engine/world.h engine/octa.h
p1xbraten/version.o: engine/lightmap.h engine/bih.h engine/texture.h
p1xbraten/version.o: engine/model.h
p1xbraten/weaponstats.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/weaponstats.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/weaponstats.o: shared/glexts.h shared/glemu.h shared/iengine.h
p1xbraten/weaponstats.o: shared/igame.h fpsgame/ai.h p1xbraten/weaponstats.h

shared/crypto-standalone.o: shared/cube.h shared/tools.h shared/geom.h
shared/crypto-standalone.o: shared/ents.h shared/command.h shared/iengine.h
shared/crypto-standalone.o: shared/igame.h
shared/stream-standalone.o: shared/cube.h shared/tools.h shared/geom.h
shared/stream-standalone.o: shared/ents.h shared/command.h shared/iengine.h
shared/stream-standalone.o: shared/igame.h
shared/tools-standalone.o: shared/cube.h shared/tools.h shared/geom.h
shared/tools-standalone.o: shared/ents.h shared/command.h shared/iengine.h
shared/tools-standalone.o: shared/igame.h
engine/command-standalone.o: engine/engine.h shared/cube.h shared/tools.h
engine/command-standalone.o: shared/geom.h shared/ents.h shared/command.h
engine/command-standalone.o: shared/iengine.h shared/igame.h engine/world.h
engine/server-standalone.o: engine/engine.h shared/cube.h shared/tools.h
engine/server-standalone.o: shared/geom.h shared/ents.h shared/command.h
engine/server-standalone.o: shared/iengine.h shared/igame.h engine/world.h
engine/worldio-standalone.o: engine/engine.h shared/cube.h shared/tools.h
engine/worldio-standalone.o: shared/geom.h shared/ents.h shared/command.h
engine/worldio-standalone.o: shared/iengine.h shared/igame.h engine/world.h
fpsgame/entities-standalone.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/entities-standalone.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/entities-standalone.o: shared/iengine.h shared/igame.h fpsgame/ai.h
fpsgame/server-standalone.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/server-standalone.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/server-standalone.o: shared/iengine.h shared/igame.h fpsgame/ai.h
fpsgame/server-standalone.o: fpsgame/capture.h fpsgame/ctf.h
fpsgame/server-standalone.o: fpsgame/collect.h fpsgame/extinfo.h
fpsgame/server-standalone.o: fpsgame/aiman.h
p1xbraten/demo_info_message-standalone.o: fpsgame/game.h shared/cube.h
p1xbraten/demo_info_message-standalone.o: shared/tools.h shared/geom.h
p1xbraten/demo_info_message-standalone.o: shared/ents.h shared/command.h
p1xbraten/demo_info_message-standalone.o: shared/iengine.h shared/igame.h
p1xbraten/demo_info_message-standalone.o: fpsgame/ai.h
p1xbraten/managed_games-standalone.o: fpsgame/game.h shared/cube.h
p1xbraten/managed_games-standalone.o: shared/tools.h shared/geom.h
p1xbraten/managed_games-standalone.o: shared/ents.h shared/command.h
p1xbraten/managed_games-standalone.o: shared/iengine.h shared/igame.h
p1xbraten/managed_games-standalone.o: fpsgame/ai.h
p1xbraten/remote_commands-standalone.o: fpsgame/game.h shared/cube.h
p1xbraten/remote_commands-standalone.o: shared/tools.h shared/geom.h
p1xbraten/remote_commands-standalone.o: shared/ents.h shared/command.h
p1xbraten/remote_commands-standalone.o: shared/iengine.h shared/igame.h
p1xbraten/remote_commands-standalone.o: fpsgame/ai.h
