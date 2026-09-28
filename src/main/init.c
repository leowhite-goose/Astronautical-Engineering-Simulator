#ifndef INIT_C
#define INIT_C

SDL_AppResult AES_init() {
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
    SDL_SetAppMetadata("Astronautical Engineering Simulator", "0.0.20", "SDL3-Project");

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

    // setting keyboard
    keyboard_scancode_down_state = SDL_GetKeyboardState(&key_count);

    AES_init_opengl();
    if (!root_gl_context) {
        SDL_Log("Couldn't create openGL context: %s", SDL_GetError());
        return SDL_APP_FAILURE;
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

    gui_texture_res = power_of_two(SDL_max(root_window_width, root_window_height));
    root_gui_surface =  SDL_CreateSurface(gui_texture_res, gui_texture_res, SDL_PIXELFORMAT_ABGR8888);
    root_gui_renderer = SDL_CreateSoftwareRenderer(root_gui_surface);
    SDL_SetRenderVSync(root_gui_renderer, 0);
    glGenTextures(1, &root_gui_gl_texture);

    if (!window_icon_surface) {
        SDL_Log("Couldn't set window icon: %s", SDL_GetError());
    }
    SDL_free(icon_path);
    SDL_SetWindowIcon(root_window, window_icon_surface);
    SDL_DestroySurface(window_icon_surface);

    AES_generate_shaders();

    #ifndef __EMSCRIPTEN__
    char model_file_ENT_H[] = "meshes/TOS-Enterprise-G14.xml"; //"meshes/MeshTest-FEMMeshNetgen001.xml"//"meshes/20mm-Cube-4.xml"//"meshes/TOS-rip-FEMMeshGmsh002.xml"//"meshes/TOS-
    #endif
    #ifdef __EMSCRIPTEN__
    char model_file_ENT_H[] = "meshes/TOS-rip-FEMMeshGmsh002.xml"; // fallback model, as model above has too many polygons to be rendered with EMSCRIPTEN/GL4ES(for some reason)
    #endif
    char model_file_ENT_L[] = "meshes/TOS-rip-FEMMeshGmsh002.xml";
    char model_file_SPH_1[] = "meshes/Sphere_Diameter=1.xml";
    char sphere_diameter1[] = "meshes/Sphere_Diameter=1.xml"; // use for stars, planets, etc.

    vec3i128 translate;

    load_fenics_mesh(model_file_ENT_H, &body[0].geo.vert_cnt, &body[0].geo.tetra_cnt, &body[0].geo.tri_cnt, &body[0].geo.vertf, &body[0].geo.vertd, &body[0].geo.vert128, &body[0].geo.tetra, &body[0].geo.tri, &body[0].geo.vert_index_cnt);
        translate.x = 0; translate.y = 0; translate.z = 0;
        vertd_to_vert128_scaled(&body[0].geo.vert_cnt, &body[0].geo.vertd, &body[0].geo.vert128, SCALE);
        vert128_translate(&body[0].geo.vert_cnt, &body[0].geo.vert128, translate);
    load_fenics_mesh(model_file_ENT_L, &body[1].geo.vert_cnt, &body[1].geo.tetra_cnt, &body[1].geo.tri_cnt, &body[1].geo.vertf, &body[1].geo.vertd, &body[1].geo.vert128, &body[1].geo.tetra, &body[1].geo.tri, &body[1].geo.vert_index_cnt);
        translate.x = 200*SCALE; translate.y = 0; translate.z = 0;
        vertd_to_vert128_scaled(&body[1].geo.vert_cnt, &body[1].geo.vertd, &body[1].geo.vert128, SCALE);
        vert128_translate(&body[1].geo.vert_cnt, &body[1].geo.vert128, translate);
    load_fenics_mesh(model_file_SPH_1, &body[2].geo.vert_cnt, &body[2].geo.tetra_cnt, &body[2].geo.tri_cnt, &body[2].geo.vertf, &body[2].geo.vertd, &body[2].geo.vert128, &body[2].geo.tetra, &body[2].geo.tri, &body[2].geo.vert_index_cnt);
        int numexp = 3;
        translate.x = 100*SCALE; translate.y = -0.5*2*SDL_powf(2,numexp)*SCALE; translate.z = 0;
        //vertd_to_vert128_scaled(&body[2].geo.vert_cnt, &body[2].geo.vertd, &body[2].geo.vert128, SCALE*SDL_powf(2,numexp));
        vertd_to_vert128_scaled(&body[2].geo.vert_cnt, &body[2].geo.vertd, &body[2].geo.vert128, SCALE/10);
        vert128_translate(&body[2].geo.vert_cnt, &body[2].geo.vert128, translate);

        //remove_shared_faces(body[0].geo.tri_cnt, &body[0].geo.tri);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    normal_data = SDL_malloc(sizeof(float) * 36 * 8192 * 2); // max of 8192 tetrahedral elements per model*; switch to using sane VBO
    color_data = SDL_malloc(sizeof(float) * 48 * 8192 * 2); // ~1.3 MiB
    vertex_data = SDL_malloc(sizeof(float) * 36 * 8192 * 2); // 1.0 MiB

    vertex_data_c = SDL_malloc(sizeof(float) * 120 * 8192 * 2);

    return SDL_APP_CONTINUE;
}

#endif
