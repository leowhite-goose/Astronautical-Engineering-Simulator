#ifndef QUIT_C
#define QUIT_C

void AES_quit() {
    // SDL will clean up the window/renderer for us, and (probably) other things too.
    SDL_DetachThread(physics_thread);
    SDL_GL_DestroyContext(root_gl_context);
}

#endif
