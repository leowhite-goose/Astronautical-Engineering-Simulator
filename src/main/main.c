#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include "GL/gl.h"
#include "gl4esinit.h" // https://deepwiki.com/ptitSeb/gl4es/1.1-getting-started && ***https://github.com/seekerluke/gl4es-web/blob/main/main.c***
#endif

#ifndef __EMSCRIPTEN__
#include <SDL3/SDL_opengl.h>
#endif

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "../common.c"
#include "../global.c" /* order matters: this must be included after common.c in order to see its datatypes */

//#include "../math/algebra/to-do.c"
#include "../math/geometry/vector.c"
#include "../math/geometry/meshes.c"
//#include "../math/calculus/laplace.c"

#include "../graphics/gui.c"
#include "../graphics/render3D.c"

//#include "../physics/mechanics.c"
//#include "../physics/collision.c"
//#include "../physics/relativity.c"
//#include "../physics/gravity.c"
//#include "../physics/thermodynamics.c"
//#include "../physics/materials.c"
//#include "../physics/electromagnetism.c"
#include "../physics/physics_thread.c"

//#include "../tools/visualizers/show_current.c"
//#include "../tools/visualizers/project_forward.c"
//#include "../tools/editor/edit_geometry.c"
//#include "../tools/editor/edit_logic.c"

#include "../main/init.c"
#include "../main/events.c"
#include "../main/mainloop.c"
#include "../main/quit.c"

/*      gcc src/main/main.c -o AES -lSDL3 -lGL         // requires fedora's "SDL3-devel.x86_64/i686*"
*       gcc src/main/main.c -o AES -I lib/ -lGL
*       ./AES
*       emcc src/main/main.c -o ./AES.html -s USE_SDL=3 -s LEGACY_GL_EMULATION=1 -s GL_UNSAFE_OPTS=1 -s GL_FFP_ONLY=1 -DSDL_THREADS=ON --embed-file data/icon.png --embed-file meshes/TOS-rip-FEMMeshGmsh002.xml -sOFFSCREEN_FRAMEBUFFER      // https://github.com/ptitSeb/gl4es/blob/master/COMPILE.md
*       emrun ./AES.html
*
*       WSL --install -d FedoraLinux-44 && sudo dnf install SDL3 mesa-libGL &&* chmod +x ./AES
*       emcc src16/main/main.c -o ./AES16_1.html -Ilib/SDL3-emcc/include -L lib/SDL3-emcc -lSDL3 -sLEGACY_GL_EMULATION=1 -sGL_UNSAFE_OPTS=1 -sGL_FFP_ONLY=1 -DSDL_THREADS-ON --embed-file meshes/TOS-rip-FEMMeshGmsh002.xml -s ALLOW_MEMORY_GROWTH=1 -s MAXIMUM_MEMORY=1gb -s MAXIMUM_MEMORY=1gb
*
*       emcc src16/main/main.c -o ./AES16_1.html -s USE_SDL=3 -Ilib/gl4es-master/include -lGL -DSDL_THREADS-ON --embed-file meshes/TOS-rip-FEMMeshGmsh002.xml -s ALLOW_MEMORY_GROWTH=1 -s MAXIMUM_MEMORY=1gb -s MAXIMUM_MEMORY=1gb -s FULL_ES2=1
*
*       emcc src16/main/main.c lib/gl4es-master/lib/libGL.a -o AES16_1.html -I lib/gl4es-master/include -s FULL_ES2=1 -lGL --use-port=sdl3 -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=64mb -s MAXIMUM_MEMORY=256mb -s TOTAL_STACK=32mb --embed-file data/icon.png --embed-file meshes/TOS-rip-FEMMeshGmsh002.xml -DSDL_THREADS-ON
*
*
emcc \
src16/main/main.c \
lib/gl4es-master/lib/libGL.a \
-o AES16_1.html \
-I lib/gl4es-master/include \
-s FULL_ES2=1 \
-s ALLOW_MEMORY_GROWTH=1 \
-s MAXIMUM_MEMORY=1gb \
-s MAXIMUM_MEMORY=1gb \
-lGL \
--use-port=sdl3 \
--embed-file data/icon.png \
--embed-file meshes/TOS-rip-FEMMeshGmsh002.xml \
-DSDL_THREADS-ON
*/

/*
emcc \
src16/main/main.c \
lib/gl4es-master/lib/libGL.a \
-o AES17.html \
-I lib/gl4es-master/include \
-s FULL_ES2=1 \
-lGL \
--use-port=sdl3 \
-s ALLOW_MEMORY_GROWTH=1 \
-s INITIAL_MEMORY=64mb \
-s MAXIMUM_MEMORY=256mb \
-s TOTAL_STACK=32mb \
--embed-file data/icon.png \
--embed-file meshes/TOS-rip-FEMMeshGmsh002.xml \
-DSDL_THREADS-ON

emcc src16/main/main.c lib/gl4es-master/lib/libGL.a -o AES17.html -I lib/gl4es-master/include -s FULL_ES2=1 -lGL --use-port=sdl3 -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=24mb -s MAXIMUM_MEMORY=384mb -s TOTAL_STACK=16mb --embed-file data/icon.png --embed-file meshes/TOS-rip-FEMMeshGmsh002.xml -DSDL_THREADS-ON -pthread -sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency

emrun ./AES17.html
python3 -m http.server
*/

/* This main function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    return AES_init(); // see "src/main/init.c"
}

/* This main function runs when a new event (mouse movement, low memory, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    return AES_events(event); // see "src/main/events.c"
}

/* This main function runs once per frame. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    return AES_mainloop(); // see "src/main/mainloop.c"
}

/* This main function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    AES_quit(); // see "src/main/quit.c"
}
