CXXFLAGS= -O3 -fomit-frame-pointer -ffast-math
override CXXFLAGS+= -Wall -fsigned-char -fno-exceptions -fno-rtti

PLATFORM= $(shell uname -s | tr '[:lower:]' '[:upper:]')
PLATFORM_PREFIX= native

INCLUDES= -Ishared -Iengine -Ifpsgame -Ip1xbraten -Ienet/include

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
CLIENT_LIBS+= -lcurl
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
CC= gcc-8 # gcc because of https://github.com/sauerbraten/p1xbraten/issues/26, 8 because that's the latest version available on AppVeyor
CXX= gcc-8
OSXMIN= 10.9
override CC+= -arch x86_64 -mmacosx-version-min=$(OSXMIN)
override CXX+= -arch x86_64 -mmacosx-version-min=$(OSXMIN)
CLIENT_INCLUDES= $(INCLUDES) -Iinclude
CLIENT_LIBS= -F../sauerbraten.app/Contents/Frameworks/ -framework SDL2 -framework SDL2_image
CLIENT_LIBS+= -framework SDL2_mixer -framework CoreAudio -framework AudioToolbox
CLIENT_LIBS+= -framework AudioUnit -framework OpenGL -framework Cocoa -lz -Lenet -lenet -lc++
CLIENT_LIBS+= -lcurl
else
CLIENT_INCLUDES= $(INCLUDES) -I/usr/X11R6/include `sdl2-config --cflags`
CLIENT_LIBS= -Lenet -lenet -L/usr/X11R6/lib -lX11 `sdl2-config --libs` -lSDL2_image -lSDL2_mixer -lz -lGL
CLIENT_LIBS+= -lcurl
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
	p1xbraten/checkupdate.o \
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
	fpsgame/embedded_cfgs.o \
	p1xbraten/clientdemo.o \
	fpsgame/entities.o \
	fpsgame/fps.o \
	fpsgame/fragmessages.o \
	fpsgame/monster.o \
	fpsgame/movable.o \
	fpsgame/p1xbraten_detection.o \
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
ifneq (,$(findstring DARWIN,$(PLATFORM)))
SERVER_LIBS+= -lc++
endif
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
shared/crypto.o: shared/command.h shared/colors.h shared/glexts.h
shared/crypto.o: shared/glemu.h shared/iengine.h shared/igame.h
shared/crypto.o: shared/authservers.h shared/masterserver.h
shared/geom.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/geom.o: shared/command.h shared/colors.h shared/glexts.h
shared/geom.o: shared/glemu.h shared/iengine.h shared/igame.h
shared/geom.o: shared/authservers.h shared/masterserver.h
shared/glemu.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/glemu.o: shared/command.h shared/colors.h shared/glexts.h
shared/glemu.o: shared/glemu.h shared/iengine.h shared/igame.h
shared/glemu.o: shared/authservers.h shared/masterserver.h
shared/stream.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/stream.o: shared/command.h shared/colors.h shared/glexts.h
shared/stream.o: shared/glemu.h shared/iengine.h shared/igame.h
shared/stream.o: shared/authservers.h shared/masterserver.h
shared/tools.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/tools.o: shared/command.h shared/colors.h shared/glexts.h
shared/tools.o: shared/glemu.h shared/iengine.h shared/igame.h
shared/tools.o: shared/authservers.h shared/masterserver.h
shared/zip.o: shared/cube.h shared/tools.h shared/geom.h shared/ents.h
shared/zip.o: shared/command.h shared/colors.h shared/glexts.h shared/glemu.h
shared/zip.o: shared/iengine.h shared/igame.h shared/authservers.h
shared/zip.o: shared/masterserver.h
engine/3dgui.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/3dgui.o: shared/ents.h shared/command.h shared/colors.h
engine/3dgui.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/3dgui.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/3dgui.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/3dgui.o: engine/texture.h engine/model.h engine/textedit.h
engine/bih.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/bih.o: shared/ents.h shared/command.h shared/colors.h shared/glexts.h
engine/bih.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/bih.o: shared/authservers.h shared/masterserver.h engine/world.h
engine/bih.o: engine/octa.h engine/lightmap.h engine/bih.h engine/texture.h
engine/bih.o: engine/model.h
engine/blend.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/blend.o: shared/ents.h shared/command.h shared/colors.h
engine/blend.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/blend.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/blend.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/blend.o: engine/texture.h engine/model.h
engine/blob.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/blob.o: shared/ents.h shared/command.h shared/colors.h shared/glexts.h
engine/blob.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/blob.o: shared/authservers.h shared/masterserver.h engine/world.h
engine/blob.o: engine/octa.h engine/lightmap.h engine/bih.h engine/texture.h
engine/blob.o: engine/model.h
p1xbraten/checkupdate.o: engine/engine.h shared/cube.h shared/tools.h
p1xbraten/checkupdate.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/checkupdate.o: shared/colors.h shared/glexts.h shared/glemu.h
p1xbraten/checkupdate.o: shared/iengine.h shared/igame.h shared/authservers.h
p1xbraten/checkupdate.o: shared/masterserver.h engine/world.h engine/octa.h
p1xbraten/checkupdate.o: engine/lightmap.h engine/bih.h engine/texture.h
p1xbraten/checkupdate.o: engine/model.h
engine/client.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/client.o: shared/ents.h shared/command.h shared/colors.h
engine/client.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/client.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/client.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/client.o: engine/texture.h engine/model.h
engine/command.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/command.o: shared/ents.h shared/command.h shared/colors.h
engine/command.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/command.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/command.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/command.o: engine/texture.h engine/model.h
engine/console.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/console.o: shared/ents.h shared/command.h shared/colors.h
engine/console.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/console.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/console.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/console.o: engine/texture.h engine/model.h
engine/cubeloader.o: engine/engine.h shared/cube.h shared/tools.h
engine/cubeloader.o: shared/geom.h shared/ents.h shared/command.h
engine/cubeloader.o: shared/colors.h shared/glexts.h shared/glemu.h
engine/cubeloader.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/cubeloader.o: shared/masterserver.h engine/world.h engine/octa.h
engine/cubeloader.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/cubeloader.o: engine/model.h
engine/decal.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/decal.o: shared/ents.h shared/command.h shared/colors.h
engine/decal.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/decal.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/decal.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/decal.o: engine/texture.h engine/model.h
engine/dynlight.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/dynlight.o: shared/ents.h shared/command.h shared/colors.h
engine/dynlight.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/dynlight.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/dynlight.o: engine/world.h engine/octa.h engine/lightmap.h
engine/dynlight.o: engine/bih.h engine/texture.h engine/model.h
engine/glare.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/glare.o: shared/ents.h shared/command.h shared/colors.h
engine/glare.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/glare.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/glare.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/glare.o: engine/texture.h engine/model.h engine/rendertarget.h
engine/grass.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/grass.o: shared/ents.h shared/command.h shared/colors.h
engine/grass.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/grass.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/grass.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/grass.o: engine/texture.h engine/model.h
engine/lightmap.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/lightmap.o: shared/ents.h shared/command.h shared/colors.h
engine/lightmap.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/lightmap.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/lightmap.o: engine/world.h engine/octa.h engine/lightmap.h
engine/lightmap.o: engine/bih.h engine/texture.h engine/model.h
engine/main.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/main.o: shared/ents.h shared/command.h shared/colors.h shared/glexts.h
engine/main.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/main.o: shared/authservers.h shared/masterserver.h engine/world.h
engine/main.o: engine/octa.h engine/lightmap.h engine/bih.h engine/texture.h
engine/main.o: engine/model.h
engine/material.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/material.o: shared/ents.h shared/command.h shared/colors.h
engine/material.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/material.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/material.o: engine/world.h engine/octa.h engine/lightmap.h
engine/material.o: engine/bih.h engine/texture.h engine/model.h
engine/menus.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/menus.o: shared/ents.h shared/command.h shared/colors.h
engine/menus.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/menus.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/menus.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/menus.o: engine/texture.h engine/model.h
engine/movie.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/movie.o: shared/ents.h shared/command.h shared/colors.h
engine/movie.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/movie.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/movie.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/movie.o: engine/texture.h engine/model.h
engine/normal.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/normal.o: shared/ents.h shared/command.h shared/colors.h
engine/normal.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/normal.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/normal.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/normal.o: engine/texture.h engine/model.h
engine/octa.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/octa.o: shared/ents.h shared/command.h shared/colors.h shared/glexts.h
engine/octa.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/octa.o: shared/authservers.h shared/masterserver.h engine/world.h
engine/octa.o: engine/octa.h engine/lightmap.h engine/bih.h engine/texture.h
engine/octa.o: engine/model.h
engine/octaedit.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/octaedit.o: shared/ents.h shared/command.h shared/colors.h
engine/octaedit.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/octaedit.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/octaedit.o: engine/world.h engine/octa.h engine/lightmap.h
engine/octaedit.o: engine/bih.h engine/texture.h engine/model.h
engine/octarender.o: engine/engine.h shared/cube.h shared/tools.h
engine/octarender.o: shared/geom.h shared/ents.h shared/command.h
engine/octarender.o: shared/colors.h shared/glexts.h shared/glemu.h
engine/octarender.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/octarender.o: shared/masterserver.h engine/world.h engine/octa.h
engine/octarender.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/octarender.o: engine/model.h
engine/p1xbraten_version.o: engine/engine.h shared/cube.h shared/tools.h
engine/p1xbraten_version.o: shared/geom.h shared/ents.h shared/command.h
engine/p1xbraten_version.o: shared/colors.h shared/glexts.h shared/glemu.h
engine/p1xbraten_version.o: shared/iengine.h shared/igame.h
engine/p1xbraten_version.o: shared/authservers.h shared/masterserver.h
engine/p1xbraten_version.o: engine/world.h engine/octa.h engine/lightmap.h
engine/p1xbraten_version.o: engine/bih.h engine/texture.h engine/model.h
engine/physics.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/physics.o: shared/ents.h shared/command.h shared/colors.h
engine/physics.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/physics.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/physics.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/physics.o: engine/texture.h engine/model.h engine/mpr.h
engine/pvs.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/pvs.o: shared/ents.h shared/command.h shared/colors.h shared/glexts.h
engine/pvs.o: shared/glemu.h shared/iengine.h shared/igame.h
engine/pvs.o: shared/authservers.h shared/masterserver.h engine/world.h
engine/pvs.o: engine/octa.h engine/lightmap.h engine/bih.h engine/texture.h
engine/pvs.o: engine/model.h
engine/rendergl.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/rendergl.o: shared/ents.h shared/command.h shared/colors.h
engine/rendergl.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/rendergl.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/rendergl.o: engine/world.h engine/octa.h engine/lightmap.h
engine/rendergl.o: engine/bih.h engine/texture.h engine/model.h
engine/rendermodel.o: engine/engine.h shared/cube.h shared/tools.h
engine/rendermodel.o: shared/geom.h shared/ents.h shared/command.h
engine/rendermodel.o: shared/colors.h shared/glexts.h shared/glemu.h
engine/rendermodel.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/rendermodel.o: shared/masterserver.h engine/world.h engine/octa.h
engine/rendermodel.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/rendermodel.o: engine/model.h engine/ragdoll.h engine/animmodel.h
engine/rendermodel.o: engine/vertmodel.h engine/skelmodel.h engine/md2.h
engine/rendermodel.o: engine/md3.h engine/md5.h engine/obj.h engine/smd.h
engine/rendermodel.o: engine/iqm.h
engine/renderparticles.o: engine/engine.h shared/cube.h shared/tools.h
engine/renderparticles.o: shared/geom.h shared/ents.h shared/command.h
engine/renderparticles.o: shared/colors.h shared/glexts.h shared/glemu.h
engine/renderparticles.o: shared/iengine.h shared/igame.h
engine/renderparticles.o: shared/authservers.h shared/masterserver.h
engine/renderparticles.o: engine/world.h engine/octa.h engine/lightmap.h
engine/renderparticles.o: engine/bih.h engine/texture.h engine/model.h
engine/renderparticles.o: engine/rendertarget.h engine/depthfx.h
engine/renderparticles.o: engine/explosion.h engine/lensflare.h
engine/renderparticles.o: engine/lightning.h
engine/rendersky.o: engine/engine.h shared/cube.h shared/tools.h
engine/rendersky.o: shared/geom.h shared/ents.h shared/command.h
engine/rendersky.o: shared/colors.h shared/glexts.h shared/glemu.h
engine/rendersky.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/rendersky.o: shared/masterserver.h engine/world.h engine/octa.h
engine/rendersky.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/rendersky.o: engine/model.h
engine/rendertext.o: engine/engine.h shared/cube.h shared/tools.h
engine/rendertext.o: shared/geom.h shared/ents.h shared/command.h
engine/rendertext.o: shared/colors.h shared/glexts.h shared/glemu.h
engine/rendertext.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/rendertext.o: shared/masterserver.h engine/world.h engine/octa.h
engine/rendertext.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/rendertext.o: engine/model.h
engine/renderva.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/renderva.o: shared/ents.h shared/command.h shared/colors.h
engine/renderva.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/renderva.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/renderva.o: engine/world.h engine/octa.h engine/lightmap.h
engine/renderva.o: engine/bih.h engine/texture.h engine/model.h
engine/server.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/server.o: shared/ents.h shared/command.h shared/colors.h
engine/server.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/server.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/server.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/server.o: engine/texture.h engine/model.h
engine/serverbrowser.o: engine/engine.h shared/cube.h shared/tools.h
engine/serverbrowser.o: shared/geom.h shared/ents.h shared/command.h
engine/serverbrowser.o: shared/colors.h shared/glexts.h shared/glemu.h
engine/serverbrowser.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/serverbrowser.o: shared/masterserver.h engine/world.h engine/octa.h
engine/serverbrowser.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/serverbrowser.o: engine/model.h
engine/shader.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/shader.o: shared/ents.h shared/command.h shared/colors.h
engine/shader.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/shader.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/shader.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/shader.o: engine/texture.h engine/model.h
engine/shadowmap.o: engine/engine.h shared/cube.h shared/tools.h
engine/shadowmap.o: shared/geom.h shared/ents.h shared/command.h
engine/shadowmap.o: shared/colors.h shared/glexts.h shared/glemu.h
engine/shadowmap.o: shared/iengine.h shared/igame.h shared/authservers.h
engine/shadowmap.o: shared/masterserver.h engine/world.h engine/octa.h
engine/shadowmap.o: engine/lightmap.h engine/bih.h engine/texture.h
engine/shadowmap.o: engine/model.h engine/rendertarget.h
engine/sound.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/sound.o: shared/ents.h shared/command.h shared/colors.h
engine/sound.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/sound.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/sound.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/sound.o: engine/texture.h engine/model.h
engine/texture.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/texture.o: shared/ents.h shared/command.h shared/colors.h
engine/texture.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/texture.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/texture.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/texture.o: engine/texture.h engine/model.h
engine/water.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/water.o: shared/ents.h shared/command.h shared/colors.h
engine/water.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/water.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/water.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/water.o: engine/texture.h engine/model.h
engine/world.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/world.o: shared/ents.h shared/command.h shared/colors.h
engine/world.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/world.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/world.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/world.o: engine/texture.h engine/model.h
engine/worldio.o: engine/engine.h shared/cube.h shared/tools.h shared/geom.h
engine/worldio.o: shared/ents.h shared/command.h shared/colors.h
engine/worldio.o: shared/glexts.h shared/glemu.h shared/iengine.h
engine/worldio.o: shared/igame.h shared/authservers.h shared/masterserver.h
engine/worldio.o: engine/world.h engine/octa.h engine/lightmap.h engine/bih.h
engine/worldio.o: engine/texture.h engine/model.h
fpsgame/ai.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/ai.o: shared/ents.h shared/command.h shared/colors.h shared/glexts.h
fpsgame/ai.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/ai.o: shared/authservers.h shared/masterserver.h fpsgame/ai.h
fpsgame/ai.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/client.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/client.o: shared/ents.h shared/command.h shared/colors.h
fpsgame/client.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/client.o: shared/igame.h shared/authservers.h shared/masterserver.h
fpsgame/client.o: fpsgame/ai.h fpsgame/weaponstats_type.h
fpsgame/client.o: fpsgame/fragmessages.h fpsgame/weaponstats.h
fpsgame/client.o: fpsgame/capture.h fpsgame/ctf.h fpsgame/collect.h
fpsgame/embedded_cfgs.o: shared/cube.h shared/tools.h shared/geom.h
fpsgame/embedded_cfgs.o: shared/ents.h shared/command.h shared/colors.h
fpsgame/embedded_cfgs.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/embedded_cfgs.o: shared/igame.h shared/authservers.h
fpsgame/embedded_cfgs.o: shared/masterserver.h fpsgame/embedded_cfgs.h
fpsgame/embedded_cfgs.o: ../data/p1xbraten/menus.cfg.gz.xxd
fpsgame/embedded_cfgs.o: ../data/p1xbraten/master.cfg.gz.xxd
fpsgame/embedded_cfgs.o: ../data/p1xbraten/gamehud.cfg.gz.xxd
fpsgame/embedded_cfgs.o: ../data/p1xbraten/keymap.cfg.gz.xxd
p1xbraten/clientdemo.o: fpsgame/game.h shared/cube.h shared/tools.h
p1xbraten/clientdemo.o: shared/geom.h shared/ents.h shared/command.h
p1xbraten/clientdemo.o: shared/colors.h shared/glexts.h shared/glemu.h
p1xbraten/clientdemo.o: shared/iengine.h shared/igame.h shared/authservers.h
p1xbraten/clientdemo.o: shared/masterserver.h fpsgame/ai.h
p1xbraten/clientdemo.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/entities.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/entities.o: shared/ents.h shared/command.h shared/colors.h
fpsgame/entities.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/entities.o: shared/igame.h shared/authservers.h shared/masterserver.h
fpsgame/entities.o: fpsgame/ai.h fpsgame/weaponstats_type.h
fpsgame/entities.o: fpsgame/fragmessages.h
fpsgame/fps.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/fps.o: shared/ents.h shared/command.h shared/colors.h shared/glexts.h
fpsgame/fps.o: shared/glemu.h shared/iengine.h shared/igame.h
fpsgame/fps.o: shared/authservers.h shared/masterserver.h fpsgame/ai.h
fpsgame/fps.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/fps.o: fpsgame/weaponstats.h
fpsgame/fragmessages.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/fragmessages.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/fragmessages.o: shared/colors.h shared/glexts.h shared/glemu.h
fpsgame/fragmessages.o: shared/iengine.h shared/igame.h shared/authservers.h
fpsgame/fragmessages.o: shared/masterserver.h fpsgame/ai.h
fpsgame/fragmessages.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/monster.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/monster.o: shared/ents.h shared/command.h shared/colors.h
fpsgame/monster.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/monster.o: shared/igame.h shared/authservers.h shared/masterserver.h
fpsgame/monster.o: fpsgame/ai.h fpsgame/weaponstats_type.h
fpsgame/monster.o: fpsgame/fragmessages.h
fpsgame/movable.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/movable.o: shared/ents.h shared/command.h shared/colors.h
fpsgame/movable.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/movable.o: shared/igame.h shared/authservers.h shared/masterserver.h
fpsgame/movable.o: fpsgame/ai.h fpsgame/weaponstats_type.h
fpsgame/movable.o: fpsgame/fragmessages.h
fpsgame/p1xbraten_detection.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/p1xbraten_detection.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/p1xbraten_detection.o: shared/colors.h shared/glexts.h shared/glemu.h
fpsgame/p1xbraten_detection.o: shared/iengine.h shared/igame.h
fpsgame/p1xbraten_detection.o: shared/authservers.h shared/masterserver.h
fpsgame/p1xbraten_detection.o: fpsgame/ai.h fpsgame/weaponstats_type.h
fpsgame/p1xbraten_detection.o: fpsgame/fragmessages.h
fpsgame/render.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/render.o: shared/ents.h shared/command.h shared/colors.h
fpsgame/render.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/render.o: shared/igame.h shared/authservers.h shared/masterserver.h
fpsgame/render.o: fpsgame/ai.h fpsgame/weaponstats_type.h
fpsgame/render.o: fpsgame/fragmessages.h
fpsgame/scoreboard.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/scoreboard.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/scoreboard.o: shared/colors.h shared/glexts.h shared/glemu.h
fpsgame/scoreboard.o: shared/iengine.h shared/igame.h shared/authservers.h
fpsgame/scoreboard.o: shared/masterserver.h fpsgame/ai.h
fpsgame/scoreboard.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/scoreboard.o: fpsgame/weaponstats.h
fpsgame/server.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/server.o: shared/ents.h shared/command.h shared/colors.h
fpsgame/server.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/server.o: shared/igame.h shared/authservers.h shared/masterserver.h
fpsgame/server.o: fpsgame/ai.h fpsgame/weaponstats_type.h
fpsgame/server.o: fpsgame/fragmessages.h fpsgame/capture.h fpsgame/ctf.h
fpsgame/server.o: fpsgame/collect.h fpsgame/extinfo.h fpsgame/aiman.h
fpsgame/weaponstats.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/weaponstats.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/weaponstats.o: shared/colors.h shared/glexts.h shared/glemu.h
fpsgame/weaponstats.o: shared/iengine.h shared/igame.h shared/authservers.h
fpsgame/weaponstats.o: shared/masterserver.h fpsgame/ai.h
fpsgame/weaponstats.o: fpsgame/weaponstats_type.h fpsgame/fragmessages.h
fpsgame/weaponstats.o: fpsgame/weaponstats.h
fpsgame/waypoint.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/waypoint.o: shared/ents.h shared/command.h shared/colors.h
fpsgame/waypoint.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/waypoint.o: shared/igame.h shared/authservers.h shared/masterserver.h
fpsgame/waypoint.o: fpsgame/ai.h fpsgame/weaponstats_type.h
fpsgame/waypoint.o: fpsgame/fragmessages.h
fpsgame/weapon.o: fpsgame/game.h shared/cube.h shared/tools.h shared/geom.h
fpsgame/weapon.o: shared/ents.h shared/command.h shared/colors.h
fpsgame/weapon.o: shared/glexts.h shared/glemu.h shared/iengine.h
fpsgame/weapon.o: shared/igame.h shared/authservers.h shared/masterserver.h
fpsgame/weapon.o: fpsgame/ai.h fpsgame/weaponstats_type.h
fpsgame/weapon.o: fpsgame/fragmessages.h fpsgame/weaponstats.h

shared/cube.h.gch: shared/tools.h shared/geom.h shared/ents.h
shared/cube.h.gch: shared/command.h shared/colors.h shared/glexts.h
shared/cube.h.gch: shared/glemu.h shared/iengine.h shared/igame.h
shared/cube.h.gch: shared/authservers.h shared/masterserver.h shared/cube.h
engine/engine.h.gch: shared/cube.h engine/world.h engine/octa.h
engine/engine.h.gch: engine/lightmap.h engine/bih.h engine/texture.h
engine/engine.h.gch: engine/model.h
fpsgame/game.h.gch: shared/cube.h fpsgame/ai.h fpsgame/weaponstats_type.h
fpsgame/game.h.gch: fpsgame/fragmessages.h fpsgame/game.h

shared/crypto-standalone.o: shared/cube.h shared/tools.h shared/geom.h
shared/crypto-standalone.o: shared/ents.h shared/command.h shared/colors.h
shared/crypto-standalone.o: shared/iengine.h shared/igame.h
shared/crypto-standalone.o: shared/authservers.h shared/masterserver.h
shared/stream-standalone.o: shared/cube.h shared/tools.h shared/geom.h
shared/stream-standalone.o: shared/ents.h shared/command.h shared/colors.h
shared/stream-standalone.o: shared/iengine.h shared/igame.h
shared/stream-standalone.o: shared/authservers.h shared/masterserver.h
shared/tools-standalone.o: shared/cube.h shared/tools.h shared/geom.h
shared/tools-standalone.o: shared/ents.h shared/command.h shared/colors.h
shared/tools-standalone.o: shared/iengine.h shared/igame.h
shared/tools-standalone.o: shared/authservers.h shared/masterserver.h
engine/command-standalone.o: engine/engine.h shared/cube.h shared/tools.h
engine/command-standalone.o: shared/geom.h shared/ents.h shared/command.h
engine/command-standalone.o: shared/colors.h shared/iengine.h shared/igame.h
engine/command-standalone.o: shared/authservers.h shared/masterserver.h
engine/command-standalone.o: engine/world.h
engine/server-standalone.o: engine/engine.h shared/cube.h shared/tools.h
engine/server-standalone.o: shared/geom.h shared/ents.h shared/command.h
engine/server-standalone.o: shared/colors.h shared/iengine.h shared/igame.h
engine/server-standalone.o: shared/authservers.h shared/masterserver.h
engine/server-standalone.o: engine/world.h
engine/worldio-standalone.o: engine/engine.h shared/cube.h shared/tools.h
engine/worldio-standalone.o: shared/geom.h shared/ents.h shared/command.h
engine/worldio-standalone.o: shared/colors.h shared/iengine.h shared/igame.h
engine/worldio-standalone.o: shared/authservers.h shared/masterserver.h
engine/worldio-standalone.o: engine/world.h
fpsgame/entities-standalone.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/entities-standalone.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/entities-standalone.o: shared/colors.h shared/iengine.h
fpsgame/entities-standalone.o: shared/igame.h shared/authservers.h
fpsgame/entities-standalone.o: shared/masterserver.h fpsgame/ai.h
fpsgame/entities-standalone.o: fpsgame/weaponstats_type.h
fpsgame/entities-standalone.o: fpsgame/fragmessages.h
fpsgame/server-standalone.o: fpsgame/game.h shared/cube.h shared/tools.h
fpsgame/server-standalone.o: shared/geom.h shared/ents.h shared/command.h
fpsgame/server-standalone.o: shared/colors.h shared/iengine.h shared/igame.h
fpsgame/server-standalone.o: shared/authservers.h shared/masterserver.h
fpsgame/server-standalone.o: fpsgame/ai.h fpsgame/weaponstats_type.h
fpsgame/server-standalone.o: fpsgame/fragmessages.h fpsgame/capture.h
fpsgame/server-standalone.o: fpsgame/ctf.h fpsgame/collect.h
fpsgame/server-standalone.o: fpsgame/extinfo.h fpsgame/aiman.h
engine/master-standalone.o: shared/cube.h shared/tools.h shared/geom.h
engine/master-standalone.o: shared/ents.h shared/command.h shared/colors.h
engine/master-standalone.o: shared/iengine.h shared/igame.h
engine/master-standalone.o: shared/authservers.h shared/masterserver.h
