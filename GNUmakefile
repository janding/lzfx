# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2023 Jeffrey H. Johnson <trnsz@pobox.com>

###############################################################################
# Configuration

CC         ?= gcc
WARNFLAGS  ?= -Wall -Wextra -Wpedantic -Wno-declaration-after-statement
CFLAGS     ?= -std=c90 -march=native -Ofast -flto=auto
STRIP      ?= strip
PRINTF     ?= printf
SHELL      := /bin/sh
SOURCES    := lzfxs-util.c lzfxs.c
HEADERS    := lzfxs.h
EXECUTABLE := lzfxs-util
OBJECTS    := $(SOURCES:.c=.o)

###############################################################################
# DUMA

ifdef USE_DUMA
  CFLAGS   += -DUSE_DUMA
  DUMALIBS ?= -l:libduma.a
endif # USE_DUMA

###############################################################################
# Defaults

.PHONY: all
all: $(EXECUTABLE)

###############################################################################
# Avoid building intermediate objects

.PHONY: $(OBJECTS) lzfxs
$(OBJECTS) lzfxs: $(EXECUTABLE)

###############################################################################
# Target: lzfxs-util

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) \
		$(WARNFLAGS) \
		$+ -o $@ $(DUMALIBS)

###############################################################################
# Dependencies

$(SOURCES): $(HEADERS)

###############################################################################
# Target: strip

.PHONY: strip
strip: $(EXECUTABLE)
	-OBJECT_MODE=X32_64 $(STRIP) $+

###############################################################################
# Target: clean

.PHONY: clean distclean
clean distclean:
	-@(CLEAN="$(EXECUTABLE) *.o core *.core a.out 2> /dev/null"; \
		export CLEAN && $(PRINTF) '%s\n' "$(RM) $${CLEAN:??}"; \
		eval "$(RM) -- $${CLEAN:??}"; eval "$(RM) $${CLEAN:??}")

###############################################################################
