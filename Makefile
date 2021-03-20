TARGET = zipplugin
OBJS= zipplugin.o danzeff.o ioapi.o unzip.o miniunz.o unrarlib/filestr.o unrarlib/recvol.o unrarlib/rs.o unrarlib/scantree.o unrarlib/rar.o unrarlib/strlist.o unrarlib/strfn.o unrarlib/pathfn.o unrarlib/int64.o unrarlib/savepos.o unrarlib/global.o unrarlib/file.o unrarlib/filefn.o unrarlib/filcreat.o \
	unrarlib/archive.o unrarlib/arcread.o unrarlib/unicode.o unrarlib/system.o unrarlib/isnt.o unrarlib/crypt.o unrarlib/crc.o unrarlib/rawread.o unrarlib/encname.o \
	unrarlib/resource.o unrarlib/match.o unrarlib/timefn.o unrarlib/rdwrfn.o unrarlib/consio.o unrarlib/options.o unrarlib/ulinks.o unrarlib/errhnd.o unrarlib/rarvm.o \
	unrarlib/rijndael.o unrarlib/getbits.o unrarlib/sha1.o unrarlib/extinfo.o unrarlib/extract.o unrarlib/volume.o unrarlib/list.o unrarlib/find.o unrarlib/unpack.o unrarlib/cmddata.o


INCDIR = "/home/Paul/pspgames/freetype/include"
CFLAGS = -O2 -G0 -Wall
CXX = g++
CXXFLAGS = $(CFLAGS) -O2 -G0 -Wall -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lpspdebug -lpspsdk -lc -lstdc++ -lpspgum -lpspgu -lpng -lz -lm -lfreetype
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Tipster Unzip/Unrar Utility
PSP_EBOOT_ICON = "icon0.png"

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

	
