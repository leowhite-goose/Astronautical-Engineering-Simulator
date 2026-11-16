#ifndef INIT_C
#define INIT_C

SDL_AppResult AES_init() {
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
    SDL_SetAppMetadata("Astronautical Engineering Simulator", "0.0.22", "SDL3-Project");

    // init SDL
    SDL_Init(SDL_INIT_VIDEO); // https://wiki.libsdl.org/SDL3/SDL_Init
    if (!SDL_INIT_VIDEO) {
        SDL_Log("Couldn't initialize SDL video: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // https://wiki.libsdl.org/SDL3/SDL_GLAttr
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    #ifdef GLES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0); // I'd like to use 3.1/3.2, but WebGL (emscripten port) only goes to 3.0 :/
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); // https://wiki.libsdl.org/SDL3/SDL_GLProfile
    glsl_version = (char*) SDL_malloc(sizeof(char) * 26);
    char version[] = "#version 300 es           ";
    memcpy(&glsl_version[0], &version[0], 26);
    #endif
    #ifdef GL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // https://wiki.libsdl.org/SDL3/SDL_GLProfile
    glsl_version = (char*) SDL_malloc(sizeof(char) * 26);
    char version[] = "#version 330 core         ";
    memcpy(&glsl_version[0], &version[0], 26);
    #endif
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

    // setting root window icon
    char *icon_path = NULL;
    SDL_asprintf(&icon_path, "%sdata/icon.png", SDL_GetBasePath());
    SDL_Surface *window_icon_surface = SDL_LoadPNG(icon_path);

    gui_texture_res = power_of_two(SDL_max(root_window_width, root_window_height));
    root_gui_surface = SDL_CreateSurface(gui_texture_res, gui_texture_res, SDL_PIXELFORMAT_ABGR8888);
    glGenTextures(1, &root_gui_gl_texture_0); // needed for emscripten
    glGenTextures(1, &root_gui_gl_texture_1);
    glBindTexture(GL_TEXTURE_2D, root_gui_gl_texture_0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Use blurry texture mapping (replace GL_LINEAR with GL_NEAREST for blocky)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, root_gui_surface->w, root_gui_surface->h, 0,  GL_RGBA, GL_UNSIGNED_BYTE, root_gui_surface->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    root_gui_renderer = SDL_CreateSoftwareRenderer(root_gui_surface);
    //SDL_SetRenderVSync(root_gui_renderer, 0);

    if (!window_icon_surface) {
        SDL_Log("Couldn't set window icon: %s", SDL_GetError());
    }
    SDL_free(icon_path);
    SDL_SetWindowIcon(root_window, window_icon_surface);
    SDL_DestroySurface(window_icon_surface);

    // initializating shader programs
    char tex_vertex_source_rel_path[] = "src/graphics/gui_vertex_shader.glsl";
    char tex_fragment_source_rel_path[] = "src/graphics/gui_fragment_shader.glsl";
    AES_init_shader_program(tex_vertex_source_rel_path, tex_fragment_source_rel_path, &tex_shader_program); // for some reason, lit shader program doesn't work right if this isn't first

    char vertex_source_rel_path[] = "src/graphics/vertex_shader.glsl";
    char unlit_fragment_source_rel_path[] = "src/graphics/non_lit_fragment_shader.glsl";
    AES_init_shader_program(vertex_source_rel_path, unlit_fragment_source_rel_path, &unlit_shader);

    char lit_fragment_source_rel_path[] = "src/graphics/fragment_shader.glsl";
    AES_init_shader_program(vertex_source_rel_path, lit_fragment_source_rel_path, &shader_program);

    // loading model data
    char model_file_ENT_H[] = "meshes/TOS-Enterprise-G14.xml"; //"meshes/MeshTest-FEMMeshNetgen001.xml"//"meshes/20mm-Cube-4.xml"//"meshes/TOS-rip-FEMMeshGmsh002.xml"//"meshes/TOS-
    char model_file_ENT_L[] = "meshes/TOS-rip-FEMMeshGmsh002.xml";
    char sphere_diameter1[] = "meshes/Sphere_Diameter=1_Low.xml"; // use for stars, planets, etc.

    char *body_files[] = {sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, sphere_diameter1, model_file_ENT_H, model_file_ENT_L};

    vec3i128 translate;
    vec3i128 vel;
    for (int i = 0; i < 12; i++) {
        load_fenics_mesh(body_files[i], &body[i].geo.vert_cnt, &body[i].geo.tetra_cnt, &body[i].geo.tri_cnt, &body[i].geo.vertf, &body[i].geo.vertd, &body[i].geo.vert128, &body[i].geo.tetra, &body[i].geo.tri, &body[i].geo.vert_index_cnt);
    }
        double radius = 695700e3*SCALE; // radius; https://ssd.jpl.nasa.gov/horizons/app.html#/
        body[0].CM.m = 1988410e27;
        vertd_i_to_vert128_scaled(&body[0].geo.vert_cnt, &body[0].geo.vertd, &body[0].geo.vert128, radius*2);
        translate.x = -1.068108951496322e9*SCALE;
        translate.y = -4.177210908491462e8*SCALE;
        translate.z = 3.086887010002915e7*SCALE;
        body[0].CM.t.p.x = translate.x; body[0].CM.t.p.y = translate.y; body[0].CM.t.p.z = translate.z;
        body[0].CM.t.v.x = 9.305302656256911e0;
        body[0].CM.t.v.y = -1.283177282717393e1;
        body[0].CM.t.v.z = -1.631700118015769e-1;
        vert128_translate(&body[0].geo.vert_cnt, &body[0].geo.vert128, translate);

        radius = 2439.4e3*SCALE;
        body[1].CM.m = 3.302e26;
        vertd_i_to_vert128_scaled(&body[1].geo.vert_cnt, &body[1].geo.vertd, &body[1].geo.vert128, radius*2);
        translate.x = -2.212073002393702e10*SCALE;
        translate.y = -6.682435921338345e10*SCALE;
        translate.z = -3.461577076477692e9*SCALE;
        body[1].CM.t.p.x = translate.x; body[1].CM.t.p.y = translate.y; body[1].CM.t.p.z = translate.z;
        body[1].CM.t.v.x = 3.666229234452722e4;
        body[1].CM.t.v.y = -1.230266984222893e4;
        body[1].CM.t.v.z = -4.368336206255391e3;
        vert128_translate(&body[1].geo.vert_cnt, &body[1].geo.vert128, translate);

        radius = 6051.84e3*SCALE;
        body[2].CM.m = 48.685e26;
        vertd_i_to_vert128_scaled(&body[2].geo.vert_cnt, &body[2].geo.vertd, &body[2].geo.vert128, radius*2);
        translate.x = -1.085736592234813e11*SCALE;
        translate.y = -3.784241757371509e9*SCALE;
        translate.z = 6.190088659339075e9*SCALE;
        body[2].CM.t.p.x = translate.x; body[2].CM.t.p.y = translate.y; body[2].CM.t.p.z = translate.z;
        body[2].CM.t.v.x = 8.984650886248794e2;
        body[2].CM.t.v.y = -3.517203951420625e4;
        body[2].CM.t.v.z = -5.320225928762774e2;
        vert128_translate(&body[2].geo.vert_cnt, &body[2].geo.vert128, translate);

        radius = 6371.01e3*SCALE;
        body[3].CM.m = 5.97219e27;
        vertd_i_to_vert128_scaled(&body[3].geo.vert_cnt, &body[3].geo.vertd, &body[3].geo.vert128, radius*2);
        translate.x = -2.627903751048988e10*SCALE;
        translate.y = 1.445101984929515e11*SCALE;
        translate.z = 3.025245352813601e7*SCALE;
        body[3].CM.t.p.x = translate.x; body[3].CM.t.p.y = translate.y; body[3].CM.t.p.z = translate.z;
        body[3].CM.t.v.x = -2.983052803412253e4;
        body[3].CM.t.v.y = -5.220465675237847e3;
        body[3].CM.t.v.z = -1.014855999592612e-1;
        vert128_translate(&body[3].geo.vert_cnt, &body[3].geo.vert128, translate);

        radius = 1737.53e3*SCALE;
        body[4].CM.m = 7.349e25;
        vertd_i_to_vert128_scaled(&body[4].geo.vert_cnt, &body[4].geo.vertd, &body[4].geo.vert128, radius*2);
        translate.x = -2.659668775178492e10*SCALE;
        translate.y = 1.442683153167126e11*SCALE;
        translate.z = 6.680827660505474e7*SCALE;
        body[4].CM.t.p.x = translate.x; body[4].CM.t.p.y = translate.y; body[4].CM.t.p.z = translate.z;
        body[4].CM.t.v.x = -2.926974096801152e4;
        body[4].CM.t.v.y = -6.020397935372383e3;
        body[4].CM.t.v.z = -1.740818643718001e0;
        vert128_translate(&body[4].geo.vert_cnt, &body[4].geo.vert128, translate);

        radius = 3389.92e3*SCALE;
        body[5].CM.m = 6.4171e26;
        vertd_i_to_vert128_scaled(&body[5].geo.vert_cnt, &body[5].geo.vertd, &body[5].geo.vert128, radius*2);
        translate.x = 2.069269460321208e11*SCALE;
        translate.y = -3.560730804791640e9*SCALE;
        translate.z = -5.147912373388751e9*SCALE;
        body[5].CM.t.p.x = translate.x; body[5].CM.t.p.y = translate.y; body[5].CM.t.p.z = translate.z;
        body[5].CM.t.v.x = 1.304308855632342e3;
        body[5].CM.t.v.y = 2.628158889664317e4;
        body[5].CM.t.v.z = 5.188465759107714e2;
        vert128_translate(&body[5].geo.vert_cnt, &body[5].geo.vert128, translate);

        radius = 69911e3*SCALE;
        body[6].CM.m = 18.9819e29;
        vertd_i_to_vert128_scaled(&body[6].geo.vert_cnt, &body[6].geo.vertd, &body[6].geo.vert128, radius*2);
        translate.x = 5.978410555886381e11*SCALE;
        translate.y = 4.387048655696349e11*SCALE;
        translate.z = -1.520164176015472e10*SCALE;
        body[6].CM.t.p.x = translate.x; body[6].CM.t.p.y = translate.y; body[6].CM.t.p.z = translate.z;
        body[6].CM.t.v.x = -7.892632213479861e3;
        body[6].CM.t.v.y = 1.115034525890079e4;
        body[6].CM.t.v.z = 1.30510044859626432e2;
        vert128_translate(&body[6].geo.vert_cnt, &body[6].geo.vert128, translate);

        radius = 58232e3*SCALE;
        body[7].CM.m = 5.6834e29;
        vertd_i_to_vert128_scaled(&body[7].geo.vert_cnt, &body[7].geo.vertd, &body[7].geo.vert128, radius*2);
        translate.x = 9.576382282218235e11*SCALE;
        translate.y = 9.821474893679625e11*SCALE;
        translate.z = -5.518978744215649e10*SCALE;
        body[7].CM.t.p.x = translate.x; body[7].CM.t.p.y = translate.y; body[7].CM.t.p.z = translate.z;
        body[7].CM.t.v.x = -7.419580377753652e3;
        body[7].CM.t.v.y = 6.725982467906618e3;
        body[7].CM.t.v.z = 1.775011906748625e2;
        vert128_translate(&body[7].geo.vert_cnt, &body[7].geo.vert128, translate);

        radius = 25362e3*SCALE;
        body[8].CM.m = 86.813e27;
        vertd_i_to_vert128_scaled(&body[8].geo.vert_cnt, &body[8].geo.vertd, &body[8].geo.vert128, radius*2);
        translate.x = 2.157706372184191e12*SCALE;
        translate.y = -2.055243161071827e12*SCALE;
        translate.z = -3.559278015686727e10*SCALE;
        body[8].CM.t.p.x = translate.x; body[8].CM.t.p.y = translate.y; body[8].CM.t.p.z = translate.z;
        body[8].CM.t.v.x = 4.646952926205451e3;
        body[8].CM.t.v.y = 4.614360059359629e3;
        body[8].CM.t.v.z = -4.301869182469398e1;
        vert128_translate(&body[8].geo.vert_cnt, &body[8].geo.vert128, translate);

        radius = 24624*SCALE;
        body[9].CM.m = 102.409e27;
        vertd_i_to_vert128_scaled(&body[9].geo.vert_cnt, &body[9].geo.vertd, &body[9].geo.vert128, radius*2);
        translate.x = 2.513785504071903e12*SCALE;
        translate.y = -3.739265163952105e12*SCALE;
        translate.z = 1.907027494504690e10*SCALE;
        body[9].CM.t.p.x = translate.x; body[9].CM.t.p.y = translate.y; body[9].CM.t.p.z = translate.z;
        body[9].CM.t.v.x = 4.475108237730198e3;
        body[9].CM.t.v.y = 3.062850303195201e3;
        body[9].CM.t.v.z = -1.667294226440544e2;
        vert128_translate(&body[9].geo.vert_cnt, &body[9].geo.vert128, translate);

        //X =-2.627641420098823E+07 Y = 1.445057839083436E+08 Z = 3.462988835807145E+04
        //VX=-2.601427552331036E+01 VY= 4.918605296500530E-01 VZ= 3.461249988666562E+00
        translate.x = -2.627641420098823e10 * SCALE, translate.y = 1.445057839083436e11 * SCALE, translate.z = 3.462988835807145e7 * SCALE;
        body[10].CM.t.v.x = -2.601427552331036e4; body[10].CM.t.v.y = 4.918605296500530e2; body[10].CM.t.v.z = 3.461249988666562e3;
        root_cam.x = translate.x + 200 * SCALE; root_cam.y = translate.y + 300 * SCALE; root_cam.z = translate.z + 100 * SCALE;
        root_cam.a = 240; root_cam.b = 95; root_cam.c = 0;
        body[10].CM.m = 1000e6;
        vertd_i_to_vert128_scaled(&body[10].geo.vert_cnt, &body[10].geo.vertd, &body[10].geo.vert128, SCALE);
        body[10].CM.t.p.x = translate.x; body[10].CM.t.p.y = translate.y; body[10].CM.t.p.z = translate.z;
        vert128_translate(&body[10].geo.vert_cnt, &body[10].geo.vert128, translate);
        body[10].CM.trajectory = (vec3i128*) SDL_malloc(sizeof(vec3i128) * 128);

        translate.x = (-2.62764143098823e10 - 100) * SCALE, translate.y = 1.445057839083436e11 * SCALE, translate.z = 3.462988835807145e7 * SCALE;
        body[11].CM.t.v.x = -2.601427552331036e4; body[11].CM.t.v.y = 4.918605296500530e2; body[11].CM.t.v.z = 3.461249988666562e3;
        body[11].CM.m = 1000e6;
        vertd_i_to_vert128_scaled(&body[11].geo.vert_cnt, &body[11].geo.vertd, &body[11].geo.vert128, SCALE);
        body[11].CM.t.p.x = translate.x; body[11].CM.t.p.y = translate.y; body[11].CM.t.p.z = translate.z;
        vert128_translate(&body[11].geo.vert_cnt, &body[11].geo.vert128, translate);

        for (int j = 0; j < 12; j++) {
            remove_shared_faces(body[j].geo.tri_cnt, &body[j].geo.tri);
        }

    for (int i = 0; i < 12; i++) { // init buffers (for physics manipulation)
        copy_128mesh(body[i].geo.vert_cnt, &body[i].geo.vert128, &body[i].geo.vert128_buffer);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 30 * 131072, NULL, GL_STREAM_DRAW); // up to 131072 triangles per drawcall = 3,932,160 bytes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 30 * 131072, NULL, GL_STREAM_DRAW);

    // setting up physics thread/loop
    physics_thread = SDL_CreateThread(physics_loop, "PhysicsThread", (void *)NULL);
    if (!physics_thread) {
        SDL_Log("Physics error: %s", SDL_GetError());
    }

    return SDL_APP_CONTINUE;
}

#endif
