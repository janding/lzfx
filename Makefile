# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2023 Jeffrey H. Johnson <trnsz@pobox.com>

CC         ?= gcc
CFLAGS     ?= -Wall -std=c90 -Wextra -Wpedantic -march=native -Ofast -flto=auto -Wno-declaration-after-statement

# Target: all
.PHONY: all
all: lzfxs-util

# Avoid building intermediate objects
.PHONY: lzfxs-util.o lzfxs.o lzfxs
lzfxs-util.o lzfxs.o lzfxs: lzfxs-util

# Target: lzfxs-util
lzfxs-util: lzfxs-util.c lzfxs.c lzfxs.h
	$(CC) -o lzfxs-util $(CFLAGS) -I. lzfxs.c lzfxs-util.c

# Target: strip
.PHONY: strip
strip: lzfxs-util
	-strip lzfxs-util

# Target: clean
.PHONY: clean distclean
clean distclean:
	$(RM) lzfxs-util *.o core *.core a.out
