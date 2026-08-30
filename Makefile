USE_DEBUG = NO
USE_64BIT = NO
USE_UNICODE = YES
USE_CLANG = NO
# sadly, cygwin mingw does not support gdiplus...
USE_CYGWIN = NO

include der_libs\tool_select.mak

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

# Automatically parse the latest version block
VERSION := $(shell grep -oE '\[[0-9]+\.[0-9]+\]' CHANGELOG.md | head -n 1 | tr -d '[]')
DIST_ZIP := $(BASE)V$(VERSION).zip

# Force these action-only targets to always run
.PHONY: dist release update

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

cppc:
	cmd /C "cppcheck --project=compile_commands.json --std=c++14 --suppressions-list=./.suppress.cppcheck"

check:
	cmd /C "d:\llvm\bin\clang-tidy.exe $(CPPSRC)"

clint:
	cmd /C "python ..\ClaudeLint.py --exclude der_libs"
	
cstale:
	cmd /C "python ..\check_compile_commands_stale.py"

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) $(LiFLAGS) +fcp -ic:\lint9 mingw.lnt -os(_lint.tmp) lintdefs.cpp lintdefs.ref.h *.rc $(CPPSRC)"

dist:
	rm -f *.zip
	zip $(DIST_ZIP) $(BINX) readme.md LICENSE.txt CHANGELOG.md

# Your new automated release workflow
release: dist
	@cmd /C "@echo Preparing GitHub release for v$(VERSION)..."
	sed -n '/## \['$(VERSION)'\]/,/## \[/p' CHANGELOG.md | sed '$$d' > temp_notes.md
	gh release create v$(VERSION) ./$(DIST_ZIP) ./CHANGELOG.md --notes-file temp_notes.md
	rm temp_notes.md
	@cmd /C "@echo Release v$(VERSION) successfully uploaded to GitHub!"
	
# Your new update-in-place pipeline
update: dist
	@cmd /C "@echo Updating assets for existing release v$(VERSION)..."
	@# Uploads and overwrites the .zip file and CHANGELOG.md on GitHub
	gh release upload v$(VERSION) ./$(DIST_ZIP) ./CHANGELOG.md --clobber
	@cmd /C "@echo Release v$(VERSION) assets successfully updated on GitHub!"

#**************************************************************
#  build rules for executables                           
#**************************************************************
$(BINX): $(OBJS)
	$(TOOLS)/$(GNAME) $(OBJS) $(LFLAGS) -o $(BINX) $(LIBS) 

#**************************************************************
#  build rules for libraries and other components
#**************************************************************
rc.o: derbar.rc
	$(TOOLS)\$(WRNAME) $< -O COFF -o $@

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
