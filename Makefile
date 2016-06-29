USE_DEBUG = NO

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -ggdb -O
LFLAGS=-mwindows
else
CFLAGS=-Wall -O2
LFLAGS=-s -mwindows
endif

CPPSRC=derbar.cpp config.cpp common_funcs.cpp system.cpp about.cpp options.cpp \
lv_ifaces.cpp images.cpp hyperlinks.cpp

OBJS = $(CPPSRC:.cpp=.o) rc.o

BINS=derbar.exe

%.o: %.cpp
	g++ $(CFLAGS) -c $<

#**************************************************************
#  generic build rules
#**************************************************************
all: $(BINS)

clean:
	rm -f $(BINS) *.o *.bak *.zip

depend:
	makedepend $(CPPSRC)

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) $(LiFLAGS) +fcp -ic:\lint9 mingw.lnt -os(_lint.tmp) lintdefs.cpp *.rc $(CPPSRC)"

source:
	zip -D DerBar.src.zip *

dist:
	zip DerBar.zip derbar.exe

#**************************************************************
#  build rules for executables                           
#**************************************************************
derbar.exe: $(OBJS)
	g++ $(CFLAGS) $(OBJS) $(LFLAGS) -o $@ -lcomctl32 -liphlpapi -lpdh

#**************************************************************
#  build rules for libraries and other components
#**************************************************************
rc.o: derbar.rc
	windres -O COFF $^ -o $@

# DO NOT DELETE

derbar.o: resource.h wcommon.h derbar.h images.h
config.o: wcommon.h derbar.h
common_funcs.o: wcommon.h
system.o: wcommon.h derbar.h ip_iface.h PdhMsg.h
about.o: resource.h hyperlinks.h
options.o: resource.h wcommon.h derbar.h
lv_ifaces.o: resource.h wcommon.h derbar.h images.h ip_iface.h
images.o: resource.h images.h wcommon.h derbar.h
hyperlinks.o: hyperlinks.h
