OUT      = sxhkd
VERCMD  ?= git describe --tags 2> /dev/null
VERSION := $(shell $(VERCMD) || cat VERSION)

CPPFLAGS += -D_POSIX_C_SOURCE=200112L -DVERSION=\"$(VERSION)\"
CFLAGS   += -std=c99 -pedantic -Wall -Wextra
LDFLAGS  ?=
LDLIBS    = $(LDFLAGS) -lxcb -lxcb-keysyms

PREFIX    ?= /usr/local
BINPREFIX ?= $(PREFIX)/bin
MANPREFIX ?= $(PREFIX)/share/man
DOCPREFIX ?= $(PREFIX)/share/doc/$(OUT)

all: $(OUT)

debug: CFLAGS += -O0 -g
debug: CPPFLAGS += -DDEBUG
debug: $(OUT)

VPATH = src
OBJ   =

include Sourcedeps

C_COMPILER= gcc
UNITY_ROOT= $(HOME)/src/Unity
TEST_INC  = -I$(UNITY_ROOT)/src -I$(UNITY_ROOT)/extras/fixture/src
TEST_SYMB = -DUNITY_FIXTURE_NO_EXTRAS
TEST_SRC  =\
		   $(UNITY_ROOT)/src/unity.c \
		   $(UNITY_ROOT)/extras/fixture/src/unity_fixture.c \
		   sxhkd.o \
		   grab.o \
		   parse.o \
		   types.o \
		   helpers.o \
		   test/test_hotkey_groups.c

$(OBJ): Makefile

$(OUT): $(OBJ)

tests:
	$(C_COMPILER) $(CFLAGS) $(TEST_INC) $(TEST_SYMB) $(TEST_SRC) $(LDLIBS) -o run_tests.out
	- ./run_tests.out

install:
	mkdir -p "$(DESTDIR)$(BINPREFIX)"
	cp -pf $(OUT) "$(DESTDIR)$(BINPREFIX)"
	mkdir -p "$(DESTDIR)$(MANPREFIX)"/man1
	cp -p doc/$(OUT).1 "$(DESTDIR)$(MANPREFIX)"/man1
	mkdir -p "$(DESTDIR)$(DOCPREFIX)"
	cp -pr examples "$(DESTDIR)$(DOCPREFIX)"/examples

uninstall:
	rm -f "$(DESTDIR)$(BINPREFIX)"/$(OUT)
	rm -f "$(DESTDIR)$(MANPREFIX)"/man1/$(OUT).1
	rm -rf "$(DESTDIR)$(DOCPREFIX)"

doc:
	a2x -v -d manpage -f manpage -a revnumber=$(VERSION) doc/$(OUT).1.asciidoc

clean:
	rm -f $(OBJ) $(OUT) run_tests.out

.PHONY: all debug install uninstall doc clean
