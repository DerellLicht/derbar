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
CFLAGS += -Ider_libs

ifeq ($(USE_UNICODE),YES)
CFLAGS += -DUNICODE -D_UNICODE
endif

ifeq ($(USE_64BIT),YES)
CFLAGS += -DUSE_64BIT
endif

CPPSRC=derbar.cpp login_lsa.cpp config.cpp system.cpp about.cpp options.cpp \
lv_ifaces.cpp images.cpp ClearIcon.cpp \
der_libs/common_funcs.cpp \
der_libs/hyperlinks.cpp \
der_libs/winmsgs.cpp \
der_libs/systray.cpp \
der_libs/tooltips.cpp 

OBJS = $(CPPSRC:.cpp=.o) rc.o

BINS=derbar.exe

%.o: %.cpp
	g++ $(CFLAGS) -Weffc++ -c $< -o $@

#**************************************************************
#  generic build rules
#**************************************************************
all: $(BINS)

clean:
	rm -f $(BINS) *.o der_libs/*.o *.bak *.zip

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

derbar.o: resource.h version.h derbar.h images.h
login_lsa.o: derbar.h
config.o: derbar.h
system.o: derbar.h ip_iface.h PdhMsg.h
about.o: resource.h version.h
options.o: resource.h derbar.h
lv_ifaces.o: resource.h derbar.h images.h ip_iface.h
images.o: resource.h images.h derbar.h
ClearIcon.o: derbar.h
der_libs/common_funcs.o: der_libs/common.h
der_libs/hyperlinks.o: der_libs/iface_32_64.h der_libs/hyperlinks.h
der_libs/systray.o: der_libs/common.h der_libs/systray.h
der_libs/tooltips.o: der_libs/iface_32_64.h der_libs/common.h
der_libs/tooltips.o: der_libs/tooltips.h
