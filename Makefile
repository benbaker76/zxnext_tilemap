################################################################################
# Ben Baker 2020
# zxnext_tilemap
################################################################################

MKDIR := mkdir -p

RM := rm -rf

CP := cp

ZIP := zip -r -q

BINDIR := bin

DEBUGFLAGS := --list --c-code-in-asm

BUILD_OPT := false

ifeq ($(BUILD_OPT), true)
CFLAGS_OPT := -SO3 --max-allocs-per-node200000
endif

CFLAGS := +zxn -subtype=nex -vn -startup=31 -clib=sdcc_iy -m $(CFLAGS_OPT)

all:
	$(MKDIR) $(BINDIR)
	zcc $(CFLAGS) $(DEBUG) @zproject.lst -o $(BINDIR)/zxnext_tilemap -create-app

debug: DEBUG = $(DEBUGFLAGS)

debug: all

clean:
	$(RM) bin tmp zcc_opt.def zcc_proj.lst src/*.lis
