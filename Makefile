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

all: test build

build: $(OUT)

debug: CFLAGS += -O0 -g
debug: CPPFLAGS += -DDEBUG
debug: $(OUT)

VPATH = src
OBJ   =

include Sourcedeps

main.o: main.c sxhkd.c grab.h helpers.h parse.h sxhkd.h types.h

$(OBJ): Makefile

$(OUT): main.o $(OBJ)

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

C_COMPILER=gcc
LINK=gcc
LINK_OPTIONS=-Wl,-nostartfiles,-etest_main
TARGET_EXTENSION=.out
UNITY_ROOT= $(HOME)/src/Unity
TEST_INC  = -I$(UNITY_ROOT)/src -Isrc
TEST_OBJ  = $(UNITY_ROOT)/src/unity.c
PATHT=test/
PATHU=$(PATHT)unit/
PATHF=$(PATHT)functional/
PATHB=build/

SRCU=$(wildcard $(PATHU)*.c)
SRCF=$(wildcard $(PATHF)*.sh)

UNIT_TESTS    =$(patsubst $(PATHU)test%.c,$(PATHB)test%.out,$(SRCU))
FUNCTION_TESTS=$(patsubst $(PATHF)test%.sh,$(PATHB)test%.txt,$(SRCF))

$(PATHB)test%.out: $(OBJ) $(TEST_OBJ) $(PATHU)test%.c
	$(C_COMPILER) $(CFLAGS) $(TEST_INC) $^ $(LDLIBS) -o $@
	- $@ > $@.txt 2>&1

$(PATHB)test%.txt: $(PATHF)test%.sh
	- bash $^ > $@ 2>&1

test: RESULTS


RESULTS: $(UNIT_TESTS) $(FUNCTION_TESTS)
	@echo "-----------------------"
	@echo "TEST RESULTS"
	@cat $(patsubst $(PATHB)test%.out,$(PATHB)test%.out.txt,$(UNIT_TESTS))
	@cat $(FUNCTION_TESTS)

clean:
	rm -f $(OBJ) $(OUT) build/*.out

.PHONY: all debug install uninstall doc clean
