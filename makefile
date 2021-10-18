SNET = -w

ifdef win
CROSS = x86_64-w64-mingw32.static-
SNET = -w -lws2_32 -liphlpapi
endif

CC=$(CROSS)g++
LD=$(CROSS)ld
AR=$(CROSS)ar
PKG_CONFIG=$(CROSS)pkg-config
SDL2_CONFIG=$(CROSS)sdl2-config

all : main.cc
	$(CC) -g -Wall -Wno-write-strings `$(SDL2_CONFIG) --cflags` `$(PKG_CONFIG) --cflags glew` `$(PKG_CONFIG) --cflags SDL2_image` main.cc `$(SDL2_CONFIG) --libs` `$(PKG_CONFIG) --libs SDL2_image` `$(PKG_CONFIG) --libs glew` -o lighting_tale.exe
