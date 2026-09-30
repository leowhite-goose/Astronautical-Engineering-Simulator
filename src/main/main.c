#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL3/SDL_opengles2.h>
#include <SDL3/SDL_opengles2_gl2ext.h>

#define SDL_MAIN_USE_CALLBACKS 1 // use the callbacks instead of main()
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "../common.c"
#include "../global.c" // order matters: this must be included after common.c in order to see its datatypes

#include "../math/geometry/vector.c"
#include "../math/geometry/meshes.c"

#include "../graphics/gui.c"
#include "../graphics/render3D.c"

#include "../physics/gravity.c"
//#include "../physics/mechanics.c"
//#include "../physics/collision.c"
//#include "../physics/thermodynamics.c"
//#include "../physics/materials.c"
//#include "../physics/electromagnetism.c"

#include "../main/events.c"
#include "../physics/physics_thread.c"
#include "../main/init.c"
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
