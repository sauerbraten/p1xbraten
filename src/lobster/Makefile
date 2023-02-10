override CXXFLAGS+= -std=c++17 -Iinclude -Ilibtcc

OBJS= \
	builtins.o \
	compiler.o \
	disasm.o \
	file.o \
	lobsterreader.o \
	platform.o \
	stdafx.o \
	tccbind.o \
	tocpp.o \
	vm.o \
	vmdata.o

EXTERNALS= \
	flatbuffers/idl_gen_text.o \
	flatbuffers/idl_parser.o \
	flatbuffers/util.o \
	libtcc/libtcc.o

liblobster.a: $(OBJS) $(EXTERNALS)
	$(AR) rcs $@ $(OBJS) $(EXTERNALS)

$(OBJS): CXXFLAGS+= -DLOBSTER_ENGINE=0 -DLOBSTER_TOCPP=0 -DLOBSTER_JIT=1

libtcc/libtcc.o: override CFLAGS+= -DHAVE_SELINUX=1

lobsterrun: main.o $(OBJS) $(EXTERNALS)
	$(CXX) $(CXXFLAGS) -o lobsterrun main.o $(OBJS) $(EXTERNALS)

clean:
	-$(RM) liblobster.a $(OBJS) $(EXTERNALS) main.o lobsterrun
