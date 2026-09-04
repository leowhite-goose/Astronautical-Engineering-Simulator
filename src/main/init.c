#ifndef INIT_C
#define INIT_C

SDL_AppResult AES_init() {
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
    SDL_SetAppMetadata("Astronautical Engineering Simulator", "0.0.17", "SDL3-Project");

    // init SDL
    SDL_Init(SDL_INIT_VIDEO); // https://wiki.libsdl.org/SDL3/SDL_Init
    if (!SDL_INIT_VIDEO) {
        SDL_Log("Couldn't initialize SDL video: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    root_window = SDL_CreateWindow("AES - Main Window", root_window_width, root_window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!root_window) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // create root openGL context;
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // https://wiki.libsdl.org/SDL3/SDL_GLAttr
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    root_gl_context = SDL_GL_CreateContext(root_window);
    #ifdef __EMSCRIPTEN__
    initialize_gl4es();
    bool fill = true;
    SDL_SetWindowFillDocument(root_window, fill);
    #endif
    if (!root_gl_context) {
        SDL_Log("Couldn't create openGL context: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_GL_SetSwapInterval(1); // note

    // setting keyboard
    keyboard_scancode_down_state = SDL_GetKeyboardState(&key_count);

    // init system info
    int opengl_major_version, opengl_minor_version, opengl_profile, depth_size;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &opengl_major_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &opengl_minor_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &opengl_profile);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth_size);
    SDL_Log("OpenGL version : %" SDL_PRIu32 ".%" SDL_PRIu32, opengl_major_version, opengl_minor_version); // of 3D openGL context
    SDL_Log("Depth size (bits): %" SDL_PRIu32, depth_size);
    if (opengl_profile == SDL_GL_CONTEXT_PROFILE_CORE) {
        SDL_Log("OpenGL Core Profile");
    }
    if (opengl_profile == SDL_GL_CONTEXT_PROFILE_COMPATIBILITY) {
        SDL_Log("OpenGL Compatibility Profile");
    }
    if (opengl_profile == SDL_GL_CONTEXT_PROFILE_ES) {
        SDL_Log("OpenGL ES Profile");
    }

    // setting up physics thread/loop
    physics_thread = SDL_CreateThread(physics_loop, "PhysicsThread", (void *)NULL);
    if (!physics_thread) {
        SDL_Log("Physics error: %s", SDL_GetError());
    }

    // setting root window icon
    char *icon_path = NULL;
    SDL_asprintf(&icon_path, "%sdata/icon.png", SDL_GetBasePath());
    SDL_Surface *window_icon_surface = SDL_LoadPNG(icon_path);

    root_gui_surface =  SDL_CreateSurface(128, 128, SDL_PIXELFORMAT_ABGR8888);
    root_gui_renderer = SDL_CreateSoftwareRenderer(root_gui_surface);
    SDL_SetRenderVSync(root_gui_renderer, 1);
    glGenTextures(1, &root_gui_gl_texture);

    if (!window_icon_surface) {
        SDL_Log("Couldn't set window icon: %s", SDL_GetError());
    }
    SDL_free(icon_path);
    SDL_SetWindowIcon(root_window, window_icon_surface);
    SDL_DestroySurface(window_icon_surface);

    char model_file[] = "meshes/TOS-rip-FEMMeshGmsh002.xml"; //"meshes/MeshTest-FEMMeshNetgen001.xml" //"meshes/20mm-Cube-4.xml" //"meshes/TOS-rip-FEMMeshGmsh002.xml"
    int vertex_count;
    int tetrahedron_count;
    load_fenics_mesh(model_file, &vertex_count, &tetrahedron_count, &model_vertices, &model_tetrahedra);
    model_cell_count = tetrahedron_count;

    normal_data = SDL_malloc(sizeof(float) * 36 * 8192); // max of 8192 tetrahedral elements per model*
    color_data = SDL_malloc(sizeof(float) * 48 * 8192); // ~1.3 MiB
    vertex_data = SDL_malloc(sizeof(float) * 36 * 8192); // 1.0 MiB

    return SDL_APP_CONTINUE;
}

#endif
