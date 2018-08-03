
# Mininum version required: 2.6 because of CMAKE_SYSTEM_NAME variable usage
cmake_minimum_required (VERSION 2.6)

# Project name
project (Wetspot2)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
PKG_SEARCH_MODULE(SDL2_MIXER REQUIRED SDL2_mixer)
INCLUDE_DIRECTORIES(${SDL_MIXER_INCLUDE_DIRS})
PKG_SEARCH_MODULE(SDL2_GFX REQUIRED SDL2_gfx)
INCLUDE_DIRECTORIES(${SDL2_GFX_INCLUDE_DIRS})

LINK_DIRECTORIES(${SDL2_LIBRARY_DIRS})

# Hack for GNUC
if(MINGW OR CYGWIN OR UNIX)
	set(CMAKE_C_FLAGS "-g -O2 -Wall -Wextra -Wno-unused -std=c99 -fms-extensions")
endif(MINGW OR CYGWIN OR UNIX)

set(CUSTOM_CFLAGS "-DSCALE_SCREEN")
ADD_DEFINITIONS(${CUSTOM_CFLAGS})

# Set the list of source files for the main application
set(SRCS font.c hiscore.c input.c logo.c menu.c palette.c sound.c sprites.c theend.c timer.c wetspot2.c world.c midiplay/mid_core.c midiplay/mid_drv.c midiplay/mid_opl3.c midiplay/ymf262.c)

# Set the executable, along with its source files
add_executable(wetspot2 ${SRCS})

# Set the libraries for the executable
target_link_libraries(wetspot2 ${SDL2_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${SDL2_GFX_LIBRARIES})
