CXXFLAGS= -O3 -fomit-frame-pointer -ffast-math
override CXXFLAGS+= -Wall -fsigned-char -fno-exceptions -fno-rtti

PLATFORM= $(shell uname -s | tr '[:lower:]' '[:upper:]')
PLATFORM_PREFIX= native

INCLUDES= -Ishared -Iengine -Ifpsgame -Ienet/include

STRIP=
ifeq (,$(findstring -g,$(CXXFLAGS)))
ifeq (,$(findstring -pg,$(CXXFLAGS)))
  STRIP=strip
endif
endif

MV=mv

ifneq (,$(findstring MINGW,$(PLATFORM)))
WINDRES= windres
ifneq (,$(findstring 64,$(PLATFORM)))
ifneq (,$(findstring CROSS,$(PLATFORM)))
  CXX=x86_64-w64-mingw32-g++
  WINDRES=x86_64-w64-mingw32-windres
ifneq (,$(STRIP))
  STRIP=x86_64-w64-mingw32-strip
endif
endif
WINLIB=lib64
WINBIN=../bin64
override CXX+= -m64
override WINDRES+= -F pe-x86-64
else
ifneq (,$(findstring CROSS,$(PLATFORM)))
  CXX=i686-w64-mingw32-g++
  WINDRES=i686-w64-mingw32-windres
ifneq (,$(STRIP))
  STRIP=i686-w64-mingw32-strip
endif
endif
WINLIB=lib
WINBIN=../bin
override CXX+= -m32
override WINDRES+= -F pe-i386
endif
CLIENT_INCLUDES= $(INCLUDES) -Iinclude
STD_LIBS= -static-libgcc -static-libstdc++
CLIENT_LIBS= -mwindows $(STD_LIBS) -L$(WINBIN) -L$(WINLIB) -lSDL2 -lSDL2_image -lSDL2_mixer -lzlib1 -lopengl32 -lenet -lws2_32 -lwinmm
else
ifneq (,$(findstring DARWIN,$(PLATFORM)))
ifneq (,$(findstring CROSS,$(PLATFORM)))
  TOOLCHAINTARGET= $(shell osxcross-conf | grep -m1 "TARGET=" | cut -b24-)
  TOOLCHAIN= x86_64-apple-$(TOOLCHAINTARGET)-
  AR= $(TOOLCHAIN)ar
  CXX= $(TOOLCHAIN)clang++
  CC= $(TOOLCHAIN)clang
ifneq (,$(STRIP))
  STRIP= $(TOOLCHAIN)strip
endif
endif
OSXMIN= 10.9
override CC+= -arch x86_64 -mmacosx-version-min=$(OSXMIN)
override CXX+= -arch x86_64 -mmacosx-version-min=$(OSXMIN)
CLIENT_INCLUDES= $(INCLUDES) -Iinclude
CLIENT_LIBS= -F../sauerbraten.app/Contents/Frameworks/ -framework SDL2 -framework SDL2_image
CLIENT_LIBS+= -framework SDL2_mixer -framework CoreAudio -framework AudioToolbox
CLIENT_LIBS+= -framework AudioUnit -framework OpenGL -framework Cocoa -lz -Lenet -lenet
else
CLIENT_INCLUDES= $(INCLUDES) -I/usr/X11R6/include `sdl2-config --cflags`
CLIENT_LIBS= -Lenet -lenet -L/usr/X11R6/lib -lX11 `sdl2-config --libs` -lSDL2_image -lSDL2_mixer -lz -lGL
endif
endif
ifeq ($(PLATFORM),LINUX)
CLIENT_LIBS+= -lrt
else
ifneq (,$(findstring GNU,$(PLATFORM)))
CLIENT_LIBS+= -lrt
endif
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
	engine/p1xbraten_version.o \
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
	fpsgame/fragmessages.o \
	fpsgame/monster.o \
	fpsgame/movable.o \
	fpsgame/p1xbraten_menus.o \
	fpsgame/render.o \
	fpsgame/scoreboard.o \
	fpsgame/server.o \
	fpsgame/weaponstats.o \
	fpsgame/waypoint.o \
	fpsgame/weapon.o

CLIENT_PCH= shared/cube.h.gch engine/engine.h.gch fpsgame/game.h.gch

ifneq (,$(findstring MINGW,$(PLATFORM)))
SERVER_INCLUDES= -DSTANDALONE $(INCLUDES) -Iinclude
SERVER_LIBS= -mwindows $(STD_LIBS) -L$(WINBIN) -L$(WINLIB) -lzlib1 -lenet -lws2_32 -lwinmm
MASTER_LIBS= $(STD_LIBS) -L$(WINBIN) -L$(WINLIB) -lzlib1 -lenet -lws2_32 -lwinmm
else
SERVER_INCLUDES= -DSTANDALONE $(INCLUDES)
SERVER_LIBS= -Lenet -lenet -lz
MASTER_LIBS= $(SERVER_LIBS)
endif
SERVER_OBJS= \
	shared/crypto-standalone.o \
	shared/stream-standalone.o \
	shared/tools-standalone.o \
	engine/command-standalone.o \
	engine/server-standalone.o \
	engine/worldio-standalone.o \
	fpsgame/entities-standalone.o \
	fpsgame/server-standalone.o

MASTER_OBJS= \
	shared/crypto-standalone.o \
	shared/stream-standalone.o \
	shared/tools-standalone.o \
	engine/command-standalone.o \
	engine/master-standalone.o

SERVER_MASTER_OBJS= $(SERVER_OBJS) $(filter-out $(SERVER_OBJS),$(MASTER_OBJS))

default: all

all: client server

clean:
	-$(RM) $(CLIENT_PCH) $(CLIENT_OBJS) $(SERVER_MASTER_OBJS) sauer_client sauer_server sauer_master

$(filter-out shared/%,$(CLIENT_PCH)): $(filter shared/%,$(CLIENT_PCH))

%.h.gch: %.h
	$(CXX) $(CXXFLAGS) -x c++-header -o $@.tmp $<
	$(MV) $@.tmp $@

%-standalone.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(CLIENT_OBJS): CXXFLAGS += $(CLIENT_INCLUDES)
$(filter shared/%,$(CLIENT_OBJS)): $(filter shared/%,$(CLIENT_PCH))
$(filter engine/%,$(CLIENT_OBJS)): $(filter engine/%,$(CLIENT_PCH))
$(filter fpsgame/%,$(CLIENT_OBJS)): $(filter fpsgame/%,$(CLIENT_PCH))

$(SERVER_MASTER_OBJS): CXXFLAGS += $(SERVER_INCLUDES)

ifneq (,$(findstring MINGW,$(PLATFORM)))
client: $(CLIENT_OBJS)
	$(WINDRES) -I vcpp -i vcpp/mingw.rc -J rc -o vcpp/mingw.res -O coff
	$(CXX) $(CXXFLAGS) -o $(WINBIN)/sauerbraten.exe vcpp/mingw.res $(CLIENT_OBJS) $(CLIENT_LIBS)

server: $(SERVER_OBJS)
	$(WINDRES) -I vcpp -i vcpp/mingw.rc -J rc -o vcpp/mingw.res -O coff
	$(CXX) $(CXXFLAGS) -o $(WINBIN)/sauer_server.exe vcpp/mingw.res $(SERVER_OBJS) $(SERVER_LIBS)

master: $(MASTER_OBJS)
	$(CXX) $(CXXFLAGS) -o $(WINBIN)/sauer_master.exe $(MASTER_OBJS) $(MASTER_LIBS)

install: all
else
client:	libenet $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o sauer_client $(CLIENT_OBJS) $(CLIENT_LIBS)
ifneq (,$(findstring DARWIN,$(PLATFORM)))
	install_name_tool -add_rpath @executable_path/../Frameworks sauer_client
endif

server:	libenet $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o sauer_server $(SERVER_OBJS) $(SERVER_LIBS)

master: libenet $(MASTER_OBJS)
	$(CXX) $(CXXFLAGS) -o sauer_master $(MASTER_OBJS) $(MASTER_LIBS)

shared/cube2font.o: shared/cube2font.c
	$(CXX) $(CXXFLAGS) -c -o $@ $< `freetype-config --cflags`

cube2font: shared/cube2font.o
	$(CXX) $(CXXFLAGS) -o cube2font shared/cube2font.o `freetype-config --libs` -lz

ifneq (,$(findstring DARWIN,$(PLATFORM)))
install: client
	cp sauer_client ../sauerbraten.app/Contents/MacOS/sauerbraten_universal
ifneq (,$(STRIP))
	$(STRIP) ../sauerbraten.app/Contents/MacOS/sauerbraten_universal
endif
else
install: all
	cp sauer_client	../bin_unix/$(PLATFORM_PREFIX)_client
	cp sauer_server	../bin_unix/$(PLATFORM_PREFIX)_server
ifneq (,$(STRIP))
	$(STRIP) ../bin_unix/$(PLATFORM_PREFIX)_client
	$(STRIP) ../bin_unix/$(PLATFORM_PREFIX)_server
endif
endif
endif

enet/libenet.a:
	$(MAKE) -C enet CC='$(CC)' AR='$(AR)'
libenet: enet/libenet.a

depend:
	makedepend -Y -Ishared -Iengine -Ifpsgame $(CLIENT_OBJS:.o=.cpp)
	makedepend -a -o.h.gch -Y -Ishared -Iengine -Ifpsgame $(CLIENT_PCH:.h.gch=.h)
	makedepend -a -o-standalone.o -Y -DSTANDALONE -Ishared -Iengine -Ifpsgame $(SERVER_MASTER_OBJS:-standalone.o=.cpp)

# DO NOT DELETE

shared/crypto.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/crypto.o: shared/command.h shared/glexts.h shared/glemu.h
shared/crypto.o: shared/iengine.h shared/igame.h shared/authservers.h
shared/geom.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/geom.o: shared/command.h shared/glexts.h shared/glemu.h
shared/geom.o: shared/iengine.h shared/igame.h shared/authservers.h
shared/glemu.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/glemu.o: shared/command.h shared/glexts.h shared/glemu.h
shared/glemu.o: shared/iengine.h shared/igame.h shared/authservers.h
shared/stream.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/stream.o: shared/command.h shared/glexts.h shared/glemu.h
shared/stream.o: shared/iengine.h shared/igame.h shared/authservers.h
shared/tools.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/tools.o: shared/command.h shared/glexts.h shared/glemu.h
shared/tools.o: shared/iengine.h shared/igame.h shared/authservers.h
shared/zip.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/zip.o: shared/command.h shared/glexts.h shared/glemu.h
shared/zip.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/3dgui.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/3dgui.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/3dgui.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/3dgui.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/3dgui.o: engine/texture.h engine/model.h engine/textedit.h
engine/bih.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/bih.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/bih.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/bih.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/bih.o: engine/texture.h engine/model.h
engine/blend.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/blend.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/blend.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/blend.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/blend.o: engine/texture.h engine/model.h
engine/blob.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/blob.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/blob.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/blob.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/blob.o: engine/texture.h engine/model.h
engine/client.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/client.o: shared/ents.h shared/command.h shared/glexts.h
engine/client.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/client.o: shared/authservers.h engine/world.h engine/octa.h
engine/client.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/client.o: engine/model.h
engine/command.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/command.o: shared/ents.h shared/command.h shared/glexts.h
engine/command.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/command.o: shared/authservers.h engine/world.h engine/octa.h
engine/command.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/command.o: engine/model.h
engine/console.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/console.o: shared/ents.h shared/command.h shared/glexts.h
engine/console.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/console.o: shared/authservers.h engine/world.h engine/octa.h
engine/console.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/console.o: engine/model.h
engine/cubeloader.o: engine/engine.h shared/cube.h shared/tools.h
engine/cubeloader.o: shared/geom.h shared/ents.h shared/command.h
engine/cubeloader.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/cubeloader.o: shared/igame.h shared/authservers.h engine/world.h
engine/cubeloader.o: engine/octa.h engine/lightmap.h engine/bih.h
engine/cubeloader.o: engine/texture.h engine/model.h
engine/decal.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/decal.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/decal.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/decal.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/decal.o: engine/texture.h engine/model.h
engine/dynlight.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/dynlight.o: shared/ents.h shared/command.h shared/glexts.h
engine/dynlight.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/dynlight.o: shared/authservers.h engine/world.h engine/octa.h
engine/dynlight.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/dynlight.o: engine/model.h
engine/glare.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/glare.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/glare.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/glare.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/glare.o: engine/texture.h engine/model.h engine/rendertarget.h
engine/grass.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/grass.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/grass.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/grass.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/grass.o: engine/texture.h engine/model.h
engine/lightmap.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/lightmap.o: shared/ents.h shared/command.h shared/glexts.h
engine/lightmap.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/lightmap.o: shared/authservers.h engine/world.h engine/octa.h
engine/lightmap.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/lightmap.o: engine/model.h
engine/main.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/main.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/main.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/main.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/main.o: engine/texture.h engine/model.h
engine/material.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/material.o: shared/ents.h shared/command.h shared/glexts.h
engine/material.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/material.o: shared/authservers.h engine/world.h engine/octa.h
engine/material.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/material.o: engine/model.h
engine/menus.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/menus.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/menus.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/menus.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/menus.o: engine/texture.h engine/model.h
engine/movie.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/movie.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/movie.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/movie.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/movie.o: engine/texture.h engine/model.h
engine/normal.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/normal.o: shared/ents.h shared/command.h shared/glexts.h
engine/normal.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/normal.o: shared/authservers.h engine/world.h engine/octa.h
engine/normal.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/normal.o: engine/model.h
engine/octa.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/octa.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/octa.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/octa.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/octa.o: engine/texture.h engine/model.h
engine/octaedit.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/octaedit.o: shared/ents.h shared/command.h shared/glexts.h
engine/octaedit.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/octaedit.o: shared/authservers.h engine/world.h engine/octa.h
engine/octaedit.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/octaedit.o: engine/model.h
engine/octarender.o: engine/engine.h shared/cube.h shared/tools.h
engine/octarender.o: shared/geom.h shared/ents.h shared/command.h
engine/octarender.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/octarender.o: shared/igame.h shared/authservers.h engine/world.h
engine/octarender.o: engine/octa.h engine/lightmap.h engine/bih.h
engine/octarender.o: engine/texture.h engine/model.h
engine/p1xbraten_version.o: engine/engine.h shared/cube.h shared/tools.h
engine/p1xbraten_version.o: shared/geom.h shared/ents.h shared/command.h
engine/p1xbraten_version.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/p1xbraten_version.o: shared/igame.h shared/authservers.h
engine/p1xbraten_version.o: engine/world.h engine/octa.h engine/lightmap.h
engine/p1xbraten_version.o: engine/bih.h engine/texture.h engine/model.h
engine/physics.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/physics.o: shared/ents.h shared/command.h shared/glexts.h
engine/physics.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/physics.o: shared/authservers.h engine/world.h engine/octa.h
engine/physics.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/physics.o: engine/model.h engine/mpr.h
engine/pvs.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/pvs.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/pvs.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/pvs.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/pvs.o: engine/texture.h engine/model.h
engine/rendergl.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/rendergl.o: shared/ents.h shared/command.h shared/glexts.h
engine/rendergl.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/rendergl.o: shared/authservers.h engine/world.h engine/octa.h
engine/rendergl.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/rendergl.o: engine/model.h
engine/rendermodel.o: engine/engine.h shared/cube.h shared/tools.h
engine/rendermodel.o: shared/geom.h shared/ents.h shared/command.h
engine/rendermodel.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/rendermodel.o: shared/igame.h shared/authservers.h engine/world.h
engine/rendermodel.o: engine/octa.h engine/lightmap.h engine/bih.h
engine/rendermodel.o: engine/texture.h engine/model.h engine/ragdoll.h
engine/rendermodel.o: engine/animmodel.h engine/vertmodel.h
engine/rendermodel.o: engine/skelmodel.h engine/md2.h engine/md3.h
engine/rendermodel.o: engine/md5.h engine/obj.h engine/smd.h engine/iqm.h
engine/renderparticles.o: engine/engine.h shared/cube.h shared/tools.h
engine/renderparticles.o: shared/geom.h shared/ents.h shared/command.h
engine/renderparticles.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/renderparticles.o: shared/igame.h shared/authservers.h engine/world.h
engine/renderparticles.o: engine/octa.h engine/lightmap.h engine/bih.h
engine/renderparticles.o: engine/texture.h engine/model.h
engine/renderparticles.o: engine/rendertarget.h engine/depthfx.h
engine/renderparticles.o: engine/explosion.h engine/lensflare.h
engine/renderparticles.o: engine/lightning.h
engine/rendersky.o: engine/engine.h shared/cube.h shared/tools.h
engine/rendersky.o: shared/geom.h shared/ents.h shared/command.h
engine/rendersky.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/rendersky.o: shared/igame.h shared/authservers.h engine/world.h
engine/rendersky.o: engine/octa.h engine/lightmap.h engine/bih.h
engine/rendersky.o: engine/texture.h engine/model.h
engine/rendertext.o: engine/engine.h shared/cube.h shared/tools.h
engine/rendertext.o: shared/geom.h shared/ents.h shared/command.h
engine/rendertext.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/rendertext.o: shared/igame.h shared/authservers.h engine/world.h
engine/rendertext.o: engine/octa.h engine/lightmap.h engine/bih.h
engine/rendertext.o: engine/texture.h engine/model.h
engine/renderva.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/renderva.o: shared/ents.h shared/command.h shared/glexts.h
engine/renderva.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/renderva.o: shared/authservers.h engine/world.h engine/octa.h
engine/renderva.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/renderva.o: engine/model.h
engine/server.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/server.o: shared/ents.h shared/command.h shared/glexts.h
engine/server.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/server.o: shared/authservers.h engine/world.h engine/octa.h
engine/server.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/server.o: engine/model.h
engine/serverbrowser.o: engine/engine.h shared/cube.h shared/tools.h
engine/serverbrowser.o: shared/geom.h shared/ents.h shared/command.h
engine/serverbrowser.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/serverbrowser.o: shared/igame.h shared/authservers.h engine/world.h
engine/serverbrowser.o: engine/octa.h engine/lightmap.h engine/bih.h
engine/serverbrowser.o: engine/texture.h engine/model.h
engine/shader.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/shader.o: shared/ents.h shared/command.h shared/glexts.h
engine/shader.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/shader.o: shared/authservers.h engine/world.h engine/octa.h
engine/shader.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/shader.o: engine/model.h
engine/shadowmap.o: engine/engine.h shared/cube.h shared/tools.h
engine/shadowmap.o: shared/geom.h shared/ents.h shared/command.h
engine/shadowmap.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/shadowmap.o: shared/igame.h shared/authservers.h engine/world.h
engine/shadowmap.o: engine/octa.h engine/lightmap.h engine/bih.h
engine/shadowmap.o: engine/texture.h engine/model.h engine/rendertarget.h
engine/sound.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/sound.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/sound.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/sound.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/sound.o: engine/texture.h engine/model.h
engine/texture.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/texture.o: shared/ents.h shared/command.h shared/glexts.h
engine/texture.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/texture.o: shared/authservers.h engine/world.h engine/octa.h
engine/texture.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/texture.o: engine/model.h
engine/water.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/water.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/water.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/water.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/water.o: engine/texture.h engine/model.h
engine/world.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/world.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
engine/world.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/world.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/world.o: engine/texture.h engine/model.h
engine/worldio.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/worldio.o: shared/ents.h shared/command.h shared/glexts.h
engine/worldio.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/worldio.o: shared/authservers.h engine/world.h engine/octa.h
engine/worldio.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/worldio.o: engine/model.h
fpsgame/ai.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/ai.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
fpsgame/ai.o: shared/iengine.h shared/igame.h shared/authservers.h
fpsgame/ai.o: fpsgame/ai.h fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/client.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/client.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/client.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/client.o: shared/authservers.h fpsgame/ai.h
fpsgame/client.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/client.o: fpsgame/weaponstats.h fpsgame/capture.h fpsgame/ctf.h
fpsgame/client.o: fpsgame/collect.h
fpsgame/entities.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/entities.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/entities.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/entities.o: shared/authservers.h fpsgame/ai.h
fpsgame/entities.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/fps.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/fps.o: shared/ents.h shared/command.h shared/glexts.h shared/glemu.h
fpsgame/fps.o: shared/iengine.h shared/igame.h shared/authservers.h
fpsgame/fps.o: fpsgame/ai.h fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/fps.o: fpsgame/weaponstats.h fpsgame/colors.h
fpsgame/fragmessages.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/fragmessages.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/fragmessages.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/fragmessages.o: shared/igame.h shared/authservers.h fpsgame/ai.h
fpsgame/fragmessages.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/monster.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/monster.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/monster.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/monster.o: shared/authservers.h fpsgame/ai.h
fpsgame/monster.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/movable.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/movable.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/movable.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/movable.o: shared/authservers.h fpsgame/ai.h
fpsgame/movable.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/p1xbraten_menus.o: shared/cube.h shared/tools.h shared/geom.h
fpsgame/p1xbraten_menus.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/p1xbraten_menus.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/p1xbraten_menus.o: shared/authservers.h fpsgame/p1xbraten_menus.h
fpsgame/p1xbraten_menus.o: ../data/p1xbraten/menus.cfg.gz.xxd
fpsgame/p1xbraten_menus.o: ../data/p1xbraten/master.cfg.gz.xxd
fpsgame/p1xbraten_menus.o: ../data/p1xbraten/gamehud.cfg.gz.xxd
fpsgame/render.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/render.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/render.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/render.o: shared/authservers.h fpsgame/ai.h
fpsgame/render.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/scoreboard.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/scoreboard.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/scoreboard.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/scoreboard.o: shared/igame.h shared/authservers.h fpsgame/ai.h
fpsgame/scoreboard.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/scoreboard.o: fpsgame/weaponstats.h fpsgame/colors.h
fpsgame/server.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/server.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/server.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/server.o: shared/authservers.h fpsgame/ai.h
fpsgame/server.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/server.o: fpsgame/capture.h fpsgame/ctf.h fpsgame/collect.h
fpsgame/server.o: fpsgame/extinfo.h fpsgame/aiman.h
fpsgame/weaponstats.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/weaponstats.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/weaponstats.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/weaponstats.o: shared/igame.h shared/authservers.h fpsgame/ai.h
fpsgame/weaponstats.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/weaponstats.o: fpsgame/weaponstats.h
fpsgame/waypoint.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/waypoint.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/waypoint.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/waypoint.o: shared/authservers.h fpsgame/ai.h
fpsgame/waypoint.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/weapon.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/weapon.o: shared/ents.h shared/command.h shared/glexts.h
fpsgame/weapon.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/weapon.o: shared/authservers.h fpsgame/ai.h
fpsgame/weapon.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/weapon.o: fpsgame/weaponstats.h

shared/cube.h.gch: shared/tools.h shared/geom.h shared/ents.h
shared/cube.h.gch: shared/command.h shared/glexts.h shared/glemu.h
shared/cube.h.gch: shared/iengine.h shared/igame.h shared/authservers.h
shared/cube.h.gch: shared/cube.h
engine/engine.h.gch: shared/cube.h engine/world.h engine/octa.h
engine/engine.h.gch: engine/lightmap.h engine/bih.h engine/texture.h
engine/engine.h.gch: engine/model.h
fpsgame/game.h.gch: shared/cube.h fpsgame/ai.h fpsgame/weaponstats_type.h
fpsgame/game.h.gch: fpsgame/fragmessages.h fpsgame/game.h

shared/crypto-standalone.o: shared/cube.h shared/tools.h shared/geom.h
shared/crypto-standalone.o: shared/ents.h shared/command.h shared/iengine.h
shared/crypto-standalone.o: shared/igame.h shared/authservers.h
shared/stream-standalone.o: shared/cube.h shared/tools.h shared/geom.h
shared/stream-standalone.o: shared/ents.h shared/command.h shared/iengine.h
shared/stream-standalone.o: shared/igame.h shared/authservers.h
shared/tools-standalone.o: shared/cube.h shared/tools.h shared/geom.h
shared/tools-standalone.o: shared/ents.h shared/command.h shared/iengine.h
shared/tools-standalone.o: shared/igame.h shared/authservers.h
engine/command-standalone.o: engine/engine.h shared/cube.h shared/tools.h
engine/command-standalone.o: shared/geom.h shared/ents.h shared/command.h
engine/command-standalone.o: shared/iengine.h shared/igame.h
engine/command-standalone.o: shared/authservers.h engine/world.h
engine/server-standalone.o: engine/engine.h shared/cube.h shared/tools.h
engine/server-standalone.o: shared/geom.h shared/ents.h shared/command.h
engine/server-standalone.o: shared/iengine.h shared/igame.h
engine/server-standalone.o: shared/authservers.h engine/world.h
engine/worldio-standalone.o: engine/engine.h shared/cube.h shared/tools.h
engine/worldio-standalone.o: shared/geom.h shared/ents.h shared/command.h
engine/worldio-standalone.o: shared/iengine.h shared/igame.h
engine/worldio-standalone.o: shared/authservers.h engine/world.h
fpsgame/entities-standalone.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/entities-standalone.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/entities-standalone.o: shared/iengine.h shared/igame.h
fpsgame/entities-standalone.o: shared/authservers.h fpsgame/ai.h
fpsgame/entities-standalone.o: fpsgame/weaponstats_type.h
fpsgame/entities-standalone.o: fpsgame/fragmessages.h
fpsgame/server-standalone.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/server-standalone.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/server-standalone.o: shared/iengine.h shared/igame.h
fpsgame/server-standalone.o: shared/authservers.h fpsgame/ai.h
fpsgame/server-standalone.o: fpsgame/weaponstats_type.h
fpsgame/server-standalone.o: fpsgame/fragmessages.h fpsgame/capture.h
fpsgame/server-standalone.o: fpsgame/ctf.h fpsgame/collect.h
fpsgame/server-standalone.o: fpsgame/extinfo.h fpsgame/aiman.h
engine/master-standalone.o: shared/cube.h shared/tools.h shared/geom.h
engine/master-standalone.o: shared/ents.h shared/command.h shared/iengine.h
engine/master-standalone.o: shared/igame.h shared/authservers.h
