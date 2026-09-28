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

    char *body_files[] = {sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, model_file_ENT_H, model_file_ENT_L};

    vec3i128 translate;
    for (int i = 0; i < 11; i++) {
        load_fenics_mesh(body_files[i], &body[i].geo.vert_cnt, &body[i].geo.tetra_cnt, &body[i].geo.tri_cnt, &body[i].geo.vertf, &body[i].geo.vertd, &body[i].geo.vert128, &body[i].geo.tetra, &body[i].geo.tri, &body[i].geo.vert_index_cnt);
    }
        int scale = 695700000; // radius
        translate.x = 0*SCALE; translate.y = 1e9*SCALE; translate.z = 0;
        vertd_to_vert128_scaled(&body[0].geo.vert_cnt, &body[0].geo.vertd, &body[0].geo.vert128, SCALE*scale*2);
        vert128_translate(&body[0].geo.vert_cnt, &body[0].geo.vert128, translate);
        scale = 2439700;
        translate.x = 1e9*SCALE; translate.y = 1e9*SCALE; translate.z = 0;
        vertd_to_vert128_scaled(&body[1].geo.vert_cnt, &body[1].geo.vertd, &body[1].geo.vert128, SCALE*scale*2);
        vert128_translate(&body[1].geo.vert_cnt, &body[1].geo.vert128, translate);
        scale = 6051800;
        translate.x = 2e9*SCALE; translate.y = 1e9*SCALE; translate.z = 0;
        vertd_to_vert128_scaled(&body[2].geo.vert_cnt, &body[2].geo.vertd, &body[2].geo.vert128, SCALE*scale*2);
        vert128_translate(&body[2].geo.vert_cnt, &body[2].geo.vert128, translate);
        scale = 6371000;
        translate.x = 3e9*SCALE; translate.y = 1e9*SCALE; translate.z = 0;
        vertd_to_vert128_scaled(&body[3].geo.vert_cnt, &body[3].geo.vertd, &body[3].geo.vert128, SCALE*scale*2);
        vert128_translate(&body[3].geo.vert_cnt, &body[3].geo.vert128, translate);
        scale = 3389500;
        translate.x = 4e9*SCALE; translate.y = 1e9*SCALE; translate.z = 0;
        vertd_to_vert128_scaled(&body[4].geo.vert_cnt, &body[4].geo.vertd, &body[4].geo.vert128, SCALE*scale*2);
        vert128_translate(&body[4].geo.vert_cnt, &body[4].geo.vert128, translate);
        scale = 69911000;
        translate.x = 5e9*SCALE; translate.y = 1e9*SCALE; translate.z = 0;
        vertd_to_vert128_scaled(&body[5].geo.vert_cnt, &body[5].geo.vertd, &body[5].geo.vert128, SCALE*scale*2);
        vert128_translate(&body[5].geo.vert_cnt, &body[5].geo.vert128, translate);
        scale = 58232000;
        translate.x = 6e9*SCALE; translate.y = 1e9*SCALE; translate.z = 0;
        vertd_to_vert128_scaled(&body[6].geo.vert_cnt, &body[6].geo.vertd, &body[6].geo.vert128, SCALE*scale*2);
        vert128_translate(&body[6].geo.vert_cnt, &body[6].geo.vert128, translate);
        scale = 25362000;
        translate.x = 7e9*SCALE; translate.y = 1e9*SCALE; translate.z = 0;
        vertd_to_vert128_scaled(&body[7].geo.vert_cnt, &body[7].geo.vertd, &body[7].geo.vert128, SCALE*scale*2);
        vert128_translate(&body[7].geo.vert_cnt, &body[7].geo.vert128, translate);
        scale = 24622000;
        translate.x = 8e9*SCALE; translate.y = 1e9*SCALE; translate.z = 0;
        vertd_to_vert128_scaled(&body[8].geo.vert_cnt, &body[8].geo.vertd, &body[8].geo.vert128, SCALE*scale*2);
        vert128_translate(&body[8].geo.vert_cnt, &body[8].geo.vert128, translate);

        translate.x = 0; translate.y = 0; translate.z = 0;
        vertd_to_vert128_scaled(&body[9].geo.vert_cnt, &body[9].geo.vertd, &body[9].geo.vert128, SCALE);
        vert128_translate(&body[9].geo.vert_cnt, &body[9].geo.vert128, translate);

        translate.x = 200*SCALE; translate.y = 0; translate.z = 0;
        vertd_to_vert128_scaled(&body[10].geo.vert_cnt, &body[10].geo.vertd, &body[10].geo.vert128, SCALE);
        vert128_translate(&body[10].geo.vert_cnt, &body[10].geo.vert128, translate);

        //remove_shared_faces(body[0].geo.tri_cnt, &body[0].geo.tri);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    vertex_data = SDL_malloc(sizeof(float) * 30 * 65536); // 65536 triangle max (per model); ~ 7.86 MB

    return SDL_APP_CONTINUE;
}

#endif
