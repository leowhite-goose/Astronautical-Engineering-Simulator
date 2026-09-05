#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include "GL/gl.h"
#include "gl4esinit.h" // ***https://github.com/seekerluke/gl4es-web/blob/main/main.c***
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
