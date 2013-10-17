#
# This will make a Windows executable using MinGW
#
# make -f mingw.mak to create GDI+WINMM build
#
#

APP_NAME = midiplay

# compiler

CC = gcc

CFLAGS = -Wall -O2 -std=c99 -fms-extensions #-masm=intel -save-temps
LFLAGS = -lSDL -s

# source files

SRC = midiplay.c mid_core.c mid_drv.c mid_opl3.c ymf262.c
#SRC := $(SRC) 


INC = $(foreach dir, ., $(wildcard $(dir)/*.h))
OBJ = $(patsubst %.c, %.o, $(SRC))

all : $(APP_NAME).exe

$(APP_NAME).exe : $(OBJ)
	$(CC) $(OBJ) $(LFLAGS) -o $(APP_NAME).exe

%.o : %.c $(INC)
	$(CC) -c $(CFLAGS) $< -o $@