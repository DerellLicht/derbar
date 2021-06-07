#  SHELL=cmd.exe
USE_DEBUG = NO
USE_64BIT = NO
USE_UNICODE = NO

ifeq ($(USE_64BIT),YES)
TOOLS=d:\tdm64\bin
else
TOOLS=c:\mingw\bin
endif

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -ggdb -O
LFLAGS=-mwindows
else
CFLAGS=-Wall -O2
LFLAGS=-s -mwindows
endif
CFLAGS += -Wno-write-strings
ifeq ($(USE_64BIT),YES)
CFLAGS += -DUSE_64BIT
endif

ifeq ($(USE_UNICODE),YES)
CFLAGS += -DUNICODE -D_UNICODE
endif

CPPSRC=derbar.cpp config.cpp common_funcs.cpp system.cpp about.cpp options.cpp \
lv_ifaces.cpp images.cpp hyperlinks.cpp systray.cpp winmsgs.cpp ClearIcon.cpp \
login_lsa.cpp

OBJS = $(CPPSRC:.cpp=.o) rc.o

BINS=derbar.exe

%.o: %.cpp
	$(TOOLS)\g++ $(CFLAGS) -c $<

#**************************************************************
#  generic build rules
#**************************************************************
all: $(BINS)

clean:
	rm -f $(BINS) *.o *.bak *.zip

depend:
	makedepend $(CPPSRC)

wc:
	wc -l *.cpp *.rc

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) $(LiFLAGS) +fcp -ic:\lint9 mingw.lnt -os(_lint.tmp) lintdefs.cpp *.rc $(CPPSRC)"

dist:
	zip DerBar.zip derbar.exe

#**************************************************************
#  build rules for executables                           
#**************************************************************
derbar.exe: $(OBJS)
	$(TOOLS)\g++ $(CFLAGS) $(OBJS) $(LFLAGS) -o $@ -lcomctl32 -liphlpapi -lpdh -lsecur32

#**************************************************************
#  build rules for libraries and other components
#**************************************************************
rc.o: derbar.rc
	$(TOOLS)\windres -O COFF $^ -o $@

# DO NOT DELETE

derbar.o: resource.h version.h common.h derbar.h images.h winmsgs.h systray.h
config.o: common.h derbar.h
common_funcs.o: common.h
system.o: common.h derbar.h ip_iface.h PdhMsg.h
about.o: resource.h version.h hyperlinks.h
options.o: resource.h common.h derbar.h winmsgs.h
lv_ifaces.o: resource.h common.h derbar.h images.h ip_iface.h
images.o: resource.h images.h common.h derbar.h
hyperlinks.o: hyperlinks.h
systray.o: resource.h common.h systray.h
ClearIcon.o: common.h derbar.h
login_lsa.o: common.h derbar.h
