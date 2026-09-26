# makefile for derbar
USE_DEBUG = NO
USE_64BIT = NO
USE_UNICODE = YES
USE_CLANG = NO
# sadly, cygwin mingw does not support gdiplus...
USE_CYGWIN = YES

include der_libs\tool_select.mak
include der_libs\release.mak

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -ggdb -O
LFLAGS=-mwindows
else
CFLAGS=-Wall -O2 -Weffc++ -c 
LFLAGS=-s -mwindows
endif
CFLAGS += -Wno-write-strings
CFLAGS += -Ider_libs
LiFLAGS += -Ider_libs

ifeq ($(USE_UNICODE),YES)
CFLAGS += -DUNICODE -D_UNICODE
LiFLAGS += -dUNICODE -d_UNICODE
endif

ifeq ($(USE_STATIC),YES)
LFLAGS += -static
endif

CPPSRC=derbar.cpp login_lsa.cpp config.cpp system.cpp about.cpp options.cpp \
lv_ifaces.cpp images.cpp ClearIcon.cpp \
der_libs/common_funcs.cpp \
der_libs/common_win.cpp \
der_libs/hyperlinks.cpp \
der_libs/winmsgs.cpp \
der_libs/systray.cpp \
der_libs/tooltips.cpp 

OBJS = $(CPPSRC:.cpp=.o) rc.o

BASE=derbar
BINX=$(BASE).exe

LIBS := -lcomctl32 -liphlpapi -lpdh

# VERSION comes from der_libs\release.mak (parsed from CHANGELOG.md)
DIST_ZIP := $(BASE)V$(VERSION).zip

# Force these action-only targets to always run.
# check-clean/notes/release/update/retag/re-release/sha256 come from
# der_libs\release.mak.
.PHONY: dist

#**************************************************************
#  generic build rules
#**************************************************************
%.o: %.cpp
	$(TOOLS)\$(GNAME) $(CFLAGS) $< -o $@

all: $(BINX)

clean:
	rm -f $(BINX) *.o der_libs/*.o *.bak *.zip

depend:
	makedepend $(CPPSRC)

wc:
	wc -l $(CPPSRC) *.rc

clint:
	cmd /C "python ..\ClaudeLint.py --exclude der_libs"
	
check:
	cmd /C "d:\llvm\bin\clang-tidy.exe $(CPPSRC)"

cppc:
	cmd /C "cppcheck --project=compile_commands.json --std=c++14 --suppressions-list=./.suppress.cppcheck"

dist:
	rm -f *.zip
	zip $(DIST_ZIP) $(BINX) readme.md LICENSE.txt CHANGELOG.md

#**************************************************************
#  build rules for executables                           
#**************************************************************
$(BINX): $(OBJS)
	$(TOOLS)/$(GNAME) $(OBJS) $(LFLAGS) -o $(BINX) $(LIBS) 

#**************************************************************
#  build rules for libraries and other components
#**************************************************************
rc.o: derbar.rc
	$(TOOLS)/$(WRNAME) $< -O COFF -o $@

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
der_libs/common_win.o: der_libs/common.h der_libs/commonw.h
der_libs/hyperlinks.o: der_libs/iface_32_64.h der_libs/hyperlinks.h
der_libs/systray.o: der_libs/common.h der_libs/systray.h
der_libs/tooltips.o: der_libs/iface_32_64.h der_libs/common.h
der_libs/tooltips.o: der_libs/tooltips.h
