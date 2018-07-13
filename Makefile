
#
# Makefile for Wetspot 2
#

NAME      = wetspot2
CC        = gcc
CFLAGS    = -g -O2 -Wall -Wextra -Wno-unused -std=c99 -fms-extensions -DSCALE_SCREEN `pkg-config --cflags sdl2`
INCS      = -I. -Iinclude
LDFLAGS   = -Llib `pkg-config --libs sdl2`
LIBS      = -lm -lSDL2_gfx -lSDL2_mixer

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
