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

$(OBJ): Makefile

$(OUT): $(OBJ)

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
TARGET_EXTENSION=.out
UNITY_ROOT= $(HOME)/src/Unity
TEST_INC  = -I$(UNITY_ROOT)/src -Isrc
TEST_OBJ  =\
		   $(UNITY_ROOT)/src/unity.c \
		   sxhkd.o \
		   grab.o \
		   parse.o \
		   types.o \
		   helpers.o
PATHT=test/
PATHU=$(PATHT)unit/
PATHF=$(PATHT)functional/
PATHB=build/

SRCU=$(wildcard $(PATHU)*.c)
SRCF=$(wildcard $(PATHF)*.c)

UNIT_TESTS    =$(patsubst $(PATHU)test%.c,$(PATHB)test%.out,$(SRCU))
FUNCTION_TESTS=$(patsubst $(PATHF)test%.c,$(PATHB)test%.out,$(SRCF))

$(PATHB)test%.out: $(TEST_OBJ) $(PATHU)test%.c
	$(C_COMPILER) $(CFLAGS) $(TEST_INC) $^ $(LDLIBS) -o $@
	- $@

$(PATHB)test%.out: $(TEST_OBJ) $(PATHF)test%.c
	$(C_COMPILER) $(CFLAGS) $(TEST_INC) $^ $(LDLIBS) -o $@
	- $@

test: $(UNIT_TESTS) $(FUNCTION_TESTS)

clean:
	rm -f $(OBJ) $(OUT) build/*.out

.PHONY: all debug install uninstall doc clean
