
#
# Makefile for Wetspot 2
#

NAME      = wetspot2
CC        = gcc
CFLAGS    = -g -O2 -Wall -Wextra -Wno-unused -std=c99 -fms-extensions \
	    -DSCALE_SCREEN
INCS      = -I.
LDFLAGS   =
LIBS      = -lm

ifdef EMSCRIPTEN
NAME     := wetspot2.js
CC       := emcc
CFLAGS   += -DUSE_SDL2 -s USE_SDL=2 -s USE_SDL_MIXER=2 -s USE_SDL_GFX=2 \
	    -s SDL2_MIXER_FORMATS=wav,mid -s SDL2_IMAGE_FORMATS=bmp
LDFLAGS  += -s ASYNCIFY --use-preload-plugins --embed-file data \
	    --embed-file world -sALLOW_MEMORY_GROWTH -lSDL2 -lSDL2_gfx \
		-lSDL2_mixer
else ifndef SDL2
# SDL1.2 defines
CFLAGS   += `sdl-config --cflags`
LIBS     += -lSDL -lSDL_gfx -lSDL_mixer
else
# SDL2 defines
CFLAGS   += `sdl2-config --cflags` -DUSE_SDL2 
LIBS     += -lSDL2 -lSDL2_gfx -lSDL2_mixer
endif

# Object files
OBJS      = wetspot2.o font.o palette.o timer.o logo.o sprites.o \
	    menu.o world.o input.o sound.o theend.o hiscore.o

# midiplay
OBJS     += midiplay/mid_core.o midiplay/mid_drv.o midiplay/mid_opl3.o \
	   midiplay/ymf262.o

# Magic starts here

ifeq ($(OS),Windows_NT)
NAME = wetspot2.exe
endif

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCS) $< -o $@

clean:
	rm -f $(OBJS) $(NAME)
