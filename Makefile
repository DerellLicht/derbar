#  SHELL=cmd.exe
USE_DEBUG = NO
USE_64BIT = NO
USE_UNICODE = NO

# Note about 64-bit mingw/tdm builds:
# Working include files are in:
# D:\tdm64/x86_64-w64-mingw32/include
# 
# Note: selection of 64-bit options in code can be done via:
# ifdef _WIN64

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

ifeq ($(USE_UNICODE),YES)
CFLAGS += -DUNICODE -D_UNICODE
endif

CPPSRC=derbar.cpp config.cpp common_funcs.cpp system.cpp about.cpp options.cpp \
lv_ifaces.cpp images.cpp hyperlinks.cpp systray.cpp winmsgs.cpp ClearIcon.cpp \
login_lsa.cpp tooltips.cpp

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
	$(TOOLS)\g++ $(CFLAGS) $(OBJS) $(LFLAGS) -o $@ -lcomctl32 -liphlpapi -lpdh

#**************************************************************
#  build rules for libraries and other components
#**************************************************************
rc.o: derbar.rc
	$(TOOLS)\windres -O COFF $^ -o $@

# DO NOT DELETE

derbar.o: iface_32_64.h resource.h version.h common.h derbar.h images.h
derbar.o: winmsgs.h systray.h tooltips.h
config.o: common.h derbar.h
common_funcs.o: common.h
system.o: common.h derbar.h ip_iface.h PdhMsg.h
about.o: resource.h version.h hyperlinks.h
options.o: resource.h common.h derbar.h winmsgs.h tooltips.h
lv_ifaces.o: resource.h common.h derbar.h images.h ip_iface.h
images.o: resource.h images.h common.h derbar.h
hyperlinks.o: iface_32_64.h hyperlinks.h
systray.o: common.h systray.h
ClearIcon.o: common.h derbar.h
login_lsa.o: common.h derbar.h
tooltips.o: iface_32_64.h common.h tooltips.h
