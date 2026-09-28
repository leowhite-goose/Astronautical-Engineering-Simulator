#ifndef RENDER3D_C
#define RENDER3D_C

void AES_init_opengl() {
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
    SDL_GL_SetSwapInterval(1); // note

    int opengl_major_version, opengl_minor_version, opengl_profile, depth_size;
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 0x0001); // https://wiki.libsdl.org/SDL3/SDL_GLProfile
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &opengl_major_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &opengl_minor_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &opengl_profile);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth_size);
    SDL_Log("OpenGL version : %" SDL_PRIu32 ".%" SDL_PRIu32, opengl_major_version, opengl_minor_version); // of 3D openGL context
    if (opengl_profile == SDL_GL_CONTEXT_PROFILE_CORE) {
        SDL_Log("OpenGL profile : Core");
    }
    if (opengl_profile == SDL_GL_CONTEXT_PROFILE_COMPATIBILITY) {
        SDL_Log("OpenGL profile : Compatibility");
    }
    if (opengl_profile == SDL_GL_CONTEXT_PROFILE_ES) {
        SDL_Log("OpenGL profile : ES");
    }
    SDL_Log("Depth size (bits): %" SDL_PRIu32, depth_size);
    /* Get function pointers for opengl_ext functions */
    #ifndef __EMSCRIPTEN__
    glGenBuffers = (glGenBuffers_func) SDL_GL_GetProcAddress("glGenBuffers");
    glBindBuffer = (glBindBuffer_func) SDL_GL_GetProcAddress("glBindBuffer");
    glBufferData = (glBufferData_func) SDL_GL_GetProcAddress("glBufferData");
    glCreateShader = (glCreateShader_func) SDL_GL_GetProcAddress("glCreateShader");
    glShaderSource = (glShaderSource_func) SDL_GL_GetProcAddress("glShaderSource");
    glCompileShader = (glCompileShader_func) SDL_GL_GetProcAddress("glCompileShader");
    glCreateProgram = (glCreateProgram_func) SDL_GL_GetProcAddress("glCreateProgram");
    glAttachShader = (glAttachShader_func) SDL_GL_GetProcAddress("glAttachShader");
    glLinkProgram = (glLinkProgram_func) SDL_GL_GetProcAddress("glLinkProgram");
    glDeleteShader = (glDeleteShader_func) SDL_GL_GetProcAddress("glDeleteShader");
    glGenVertexArrays = (glGenVertexArrays_func) SDL_GL_GetProcAddress("glGenVertexArrays");
    glBindVertexArray = (glBindVertexArray_func) SDL_GL_GetProcAddress("glBindVertexArray");
    glVertexAttribPointer = (glVertexAttribPointer_func) SDL_GL_GetProcAddress("glVertexAttribPointer");
    glEnableVertexAttribArray = (glEnableVertexAttribArray_func) SDL_GL_GetProcAddress("glEnableVertexAttribArray");
    glUseProgram = (glUseProgram_func) SDL_GL_GetProcAddress("glUseProgram");
    glGetProgramiv = (glGetProgramiv_func) SDL_GL_GetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (glGetProgramInfoLog_func) SDL_GL_GetProcAddress("glGetProgramInfoLog");
    glGetShaderiv = (glGetShaderiv_func) SDL_GL_GetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (glGetShaderInfoLog_func) SDL_GL_GetProcAddress("glGetShaderInfoLog");
    glUniformMatrix4fv = (glUniformMatrix4fv_func) SDL_GL_GetProcAddress("glUniformMatrix4fv");
    glGetUniformLocation = (glGetUniformLocation_func) SDL_GL_GetProcAddress("glGetUniformLocation");
    glGetnUniformfv = (glGetnUniformfv_func) SDL_GL_GetProcAddress("glGetnUniformfv");
    glProgramUniformMatrix4fv = (glProgramUniformMatrix4fv_func) SDL_GL_GetProcAddress("glProgramUniformMatrix4fv");
    glUniform4f = (glUniform4f_func) SDL_GL_GetProcAddress("glUniform4f");
    glBufferSubData = (glBufferSubData_func) SDL_GL_GetProcAddress("glBufferSubData");
    glUniform3f = (glUniform3f_func) SDL_GL_GetProcAddress("glUniform3f");
    #endif
}

void AES_generate_shaders() {
    int success;
    char info_log[512];

    char *vertex_shader_src;
    char *fragment_shader_src;

    char vertex_shader_rel_path[] = "src/graphics/vertex_shader.glsl";
    char *vertex_path = NULL;
    SDL_asprintf(&vertex_path, "%s" "%s", SDL_GetBasePath(), vertex_shader_rel_path);
    vertex_shader_src = SDL_LoadFile(vertex_path, NULL);

    char fragment_shader_rel_path[] = "src/graphics/fragment_shader.glsl";
    char *fragment_path = NULL;
    SDL_asprintf(&fragment_path, "%s" "%s", SDL_GetBasePath(), fragment_shader_rel_path);
    fragment_shader_src = SDL_LoadFile(fragment_path, NULL);

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char *vertex_shader_source = vertex_shader_src;
    SDL_Log("%s", vertex_shader_src);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        SDL_Log("Vertex Shader Error: %s", info_log);
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragment_shader_source = fragment_shader_src;
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        SDL_Log("Fragment Shader Error: %s", info_log);
    }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        SDL_Log("Shader Program Error: %s", info_log);
    }
}

void perspectivef(float fovY, float aspect, float z_near, float z_far, float* matrix) { // https://nlguillemot.wordpress.com/2016/12/07/reversed-z-in-opengl/
    float f = 1.0 / SDL_tanf(fovY * 0.5 * (SDL_PI_D / 180));

    matrix[0]  = f / aspect;
    matrix[1]  = 0;
    matrix[2]  = 0;
    matrix[3]  = 0;

    matrix[4]  = 0;
    matrix[5]  = f;
    matrix[6]  = 0;
    matrix[7]  = 0;

    matrix[8]  = 0;
    matrix[9]  = 0;
    matrix[10] = (z_far + z_near) / (z_near - z_far);
    matrix[11] = -1;

    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = (2 * z_far * z_near) / (z_near - z_far); // I think this uses reversed Z (I at least tried it out); I've honestly forgetten what I did here
    matrix[15] = 0;
}

void perspective(float fovY, float aspect, float z_near, float z_far, float* matrix) {
    float f = 1.0 / SDL_tanf(fovY * 0.5 * (SDL_PI_D / 180));

    matrix[0]  = f / aspect;
    matrix[1]  = 0;
    matrix[2]  = 0;
    matrix[3]  = 0;

    matrix[4]  = 0;
    matrix[5]  = f;
    matrix[6]  = 0;
    matrix[7]  = 0;

    matrix[8]  = 0;
    matrix[9]  = 0;
    matrix[10] = -(z_far+z_near) / (z_far - z_near);
    matrix[11] = (-2 * z_far * z_near) / (z_far - z_near);

    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = -1;
    matrix[15] = 0;
}

void render_triangles(uint32 vertex_count, uint32 triangle_count, vec32i3f **vertf, vec32i3i128 **vert128, vec4i32 **triangles, uint32 vert_index_cnt, vec3f3i128 cam) {
    //SDL_Log("%" SDL_PRIu32, triangle_count);
    //SDL_Log("HYIj - %.3f", (float) (*vert128)[6].x);
    vec3i128 translate;
    translate.x = -cam.x;
    translate.y = -cam.y;
    translate.z = -cam.z;
    vert128_translate(&vertex_count, vert128, translate);
    vert128_to_verf(&vertex_count, vert128, vertf);
    translate.x = cam.x;
    translate.y = cam.y;
    translate.z = cam.z;
    vert128_translate(&vertex_count, vert128, translate);
    //SDL_Log("HYIf - %.3f", (float) (*vertf)[6].x);
    for (int i = 0; i < triangle_count; i++) {
        vec3f normal = generate_normal((*vertf)[(*triangles)[i].x], (*vertf)[(*triangles)[i].y], (*vertf)[(*triangles)[i].z]);
        float transparency_ = 1.0;
        float temp_vertex_data[30] = {
            (*vertf)[(*triangles)[i].x].x, (*vertf)[(*triangles)[i].x].y, (*vertf)[(*triangles)[i].x].z, 0.8,0.8,0.8,transparency_, normal.x, normal.y, normal.z, // v0
            (*vertf)[(*triangles)[i].y].x, (*vertf)[(*triangles)[i].y].y, (*vertf)[(*triangles)[i].y].z, 0.8,0.8,0.8,transparency_, normal.x, normal.y, normal.z, // v1
            (*vertf)[(*triangles)[i].z].x, (*vertf)[(*triangles)[i].z].y, (*vertf)[(*triangles)[i].z].z, 0.8,0.8,0.8,transparency_, normal.x, normal.y, normal.z  // v2
        };
        for (int j = 0; j < 30; j++) {
            vertex_data[j + 30 * i] = temp_vertex_data[j];
        }
    }
}

void render_tetrahedra(vec32i3f **nodes, vec5i32 **cells, vec32i3i128 **vert128, uint32 vertex_count, int32 cell_count, bool debug, vec3f3i128 cam) {
    vec3i128 translate;
    translate.x = -cam.x;
    translate.y = -cam.y;
    translate.z = -cam.z;
    vert128_translate(&vertex_count, vert128, translate);
    vert128_to_verf_graphics(&vertex_count, vert128, nodes);
    translate.x = cam.x;
    translate.y = cam.y;
    translate.z = cam.z;
    vert128_translate(&vertex_count, vert128, translate);
    if (!debug) {glEnable(GL_CULL_FACE); glCullFace(GL_BACK);}
    for (int i = 0; i < cell_count; i++) {
        vec3f normal0 = generate_normal((*nodes)[(*cells)[i].b], (*nodes)[(*cells)[i].x], (*nodes)[(*cells)[i].y]);
        vec3f normal1 = generate_normal((*nodes)[(*cells)[i].z], (*nodes)[(*cells)[i].y], (*nodes)[(*cells)[i].x]); // flipped
        vec3f normal2 = generate_normal((*nodes)[(*cells)[i].b], (*nodes)[(*cells)[i].y], (*nodes)[(*cells)[i].z]);
        vec3f normal3 = generate_normal((*nodes)[(*cells)[i].z], (*nodes)[(*cells)[i].x], (*nodes)[(*cells)[i].b]); // flipped
        float transparency_;
        if (debug) {
            transparency_ = 0.5;
        } else {
            transparency_ = 1.0;
        }
        float temp_vertex_data[120] = {
            (*nodes)[(*cells)[i].b].x, (*nodes)[(*cells)[i].b].y, (*nodes)[(*cells)[i].b].z, 1.0,0.0,0.0,transparency_, normal0.x, normal0.y, normal0.z, // v0 // v0 v1 v2
            (*nodes)[(*cells)[i].x].x, (*nodes)[(*cells)[i].x].y, (*nodes)[(*cells)[i].x].z, 1.0,0.0,0.0,transparency_, normal0.x, normal0.y, normal0.z, // v1
            (*nodes)[(*cells)[i].y].x, (*nodes)[(*cells)[i].y].y, (*nodes)[(*cells)[i].y].z, 1.0,0.0,0.0,transparency_, normal0.x, normal0.y, normal0.z, // v2

            (*nodes)[(*cells)[i].z].x, (*nodes)[(*cells)[i].z].y, (*nodes)[(*cells)[i].z].z, 0.0,1.0,0.0,transparency_, normal1.x, normal1.y, normal1.z, // v3 // v3 v2 v1
            (*nodes)[(*cells)[i].y].x, (*nodes)[(*cells)[i].y].y, (*nodes)[(*cells)[i].y].z, 0.0,1.0,0.0,transparency_, normal1.x, normal1.y, normal1.z, // v2
            (*nodes)[(*cells)[i].x].x, (*nodes)[(*cells)[i].x].y, (*nodes)[(*cells)[i].x].z, 0.0,1.0,0.0,transparency_, normal1.x, normal1.y, normal1.z, // v1

            (*nodes)[(*cells)[i].b].x, (*nodes)[(*cells)[i].b].y, (*nodes)[(*cells)[i].b].z, 0.0,0.0,1.0,transparency_, normal2.x, normal2.y, normal2.z, // v0 // v0 v2 v3
            (*nodes)[(*cells)[i].y].x, (*nodes)[(*cells)[i].y].y, (*nodes)[(*cells)[i].y].z, 0.0,0.0,1.0,transparency_, normal2.x, normal2.y, normal2.z, // v2
            (*nodes)[(*cells)[i].z].x, (*nodes)[(*cells)[i].z].y, (*nodes)[(*cells)[i].z].z, 0.0,0.0,1.0,transparency_, normal2.x, normal2.y, normal2.z, // v3

            (*nodes)[(*cells)[i].z].x, (*nodes)[(*cells)[i].z].y, (*nodes)[(*cells)[i].z].z, 1.0,0.0,1.0,transparency_, normal3.x, normal3.y, normal3.z, // v3 // v3 v1 v0
            (*nodes)[(*cells)[i].x].x, (*nodes)[(*cells)[i].x].y, (*nodes)[(*cells)[i].x].z, 1.0,0.0,1.0,transparency_, normal3.x, normal3.y, normal3.z, // v1
            (*nodes)[(*cells)[i].b].x, (*nodes)[(*cells)[i].b].y, (*nodes)[(*cells)[i].b].z, 1.0,0.0,1.0,transparency_, normal3.x, normal3.y, normal3.z  // v0
        };
        for (int j = 0; j < 120; j++) {
            vertex_data_c[j + 120 * i] = temp_vertex_data[j];
        }
    }
    if (!debug) {glDisable(GL_CULL_FACE);}
}

void draw_world_geometry(vec3f3i128 cam) {
    //glTranslatef(cam.y, -cam.z, cam.x);
    glRotatef(90, 0.0, 1.0, 0.0);
    glRotatef(-90, 1.0, 0.0, 0.0);

    glEnable(GL_CULL_FACE);
    glEnable(GL_COLOR_MATERIAL);
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1);
    glEnable(GL_LIGHT0);
    float light0_pos[4] = {cam.x, cam.y, cam.z, 1};
    float atten_terms[3] = {0,0,1}; //{0.0000001/SDL_powf(SCALE,2),0,0}; // falloff = (inversely proportional to) luminance
    float specular[4] = {1,1,1,1};
    float global_ambient[4] = {0, 0, 0, 1};
    //float mat_specular[4] = {0.1,0.1,0.1,1};
    float mat_emmision[4] = {0,0,0,1};
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, atten_terms);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightModelf(GL_LIGHT_MODEL_AMBIENT, *global_ambient);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emmision);
    glPolygonMode(GL_FRONT, GL_FILL); // GL_POINT, GL_LINE, GL_FILL
    glPolygonMode(GL_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (touch_button[0]) {
        glBegin(GL_TRIANGLES); // colliding blue triangles test
        glColor4f(0.2, 0.2, 0.5, 0.4);
        glVertex3f(p1a.x, p1a.y, p1a.z);
        glVertex3f(p1b.x, p1b.y, p1b.z);
        glVertex3f(p1c.x, p1c.y, p1c.z);
        glVertex3f(p2a.x, p2a.y, p2a.z);
        glVertex3f(p2b.x, p2b.y, p2b.z);
        glVertex3f(p2c.x, p2c.y, p2c.z);
        glEnd();
    }

    vec3i128 translate;
    translate.x = cam.x;
    translate.y = cam.y;
    translate.z = cam.z;
    //vert128_translate(&body[0].geo.vert_cnt, &body[0].geo.vert128, translate);
    //SDL_Log("JJIK ] %.3f", body[0].geo.vert128[5].x);

    glEnable(GL_LIGHTING);

    //render_triangles(body[0].geo.vert_cnt, body[0].geo.tetra_cnt * 4, &body[0].geo.vertf, &body[0].geo.vert128, &body[0].geo.tri, body[0].geo.vert_index_cnt, cam); // enterprise

    mat_emmision[0] = 0.5;
    mat_emmision[1] = 0.5;
    mat_emmision[2] = 0.5;
    mat_emmision[3] = 1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emmision);
    //render_triangles(body[2].geo.vert_cnt, body[2].geo.tetra_cnt * 4, &body[2].geo.vertf, &body[2].geo.vert128, &body[2].geo.tri, body[2].geo.vert_index_cnt, cam); // planet
    mat_emmision[0] = 0;
    mat_emmision[1] = 0;
    mat_emmision[2] = 0;
    mat_emmision[3] = 1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emmision);

    glDisable(GL_LIGHTING);

    //render_tetrahedra(&body[1].geo.vertf, &body[1].geo.tetra, &body[1].geo.vert128, body[1].geo.vert_cnt, body[1].geo.tetra_cnt, 1); // rainbow-prise

    glRotatef(90, 1.0, 0.0, 0.0);
    glRotatef(-90, 0.0, 1.0, 0.0);
    //glTranslatef(-cam.y, cam.z, -cam.x);
}

void render3D(float window_width, float window_height, vec3f3i128 cam) {
    glViewport(0, 0, window_width, window_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //3D render
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float projection_matrix_0[16]; // 4x4 perspective matrix
    float projection_matrix_1[16];

    //far
    perspectivef(60.0f, (float) window_width / window_height, (float) SDL_pow(2,10), (float) SDL_pow(2,20), projection_matrix_0);
    glLoadMatrixf(projection_matrix_0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(cam.b - 90, 1.0f, 0.0f, 0.0f);
    glRotatef(-cam.a    , 0.0f, 1.0f, 0.0f);
    glRotatef(cam.c     , 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    draw_world_geometry(cam);
    //glDisable(GL_DEPTH_TEST);

    /*glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glClear(GL_DEPTH_BUFFER_BIT);

    //close
    perspectivef(60.0f, (float) window_width / window_height, (float) SDL_pow(2,8), (float) SDL_pow(2,15), projection_matrix_1);
    glLoadMatrixf(projection_matrix_1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(cam.b - 90, 1.0f, 0.0f, 0.0f);
    glRotatef(-cam.a    , 0.0f, 1.0f, 0.0f);
    glRotatef(cam.c     , 0.0f, 0.0f, 1.0f);

    //glEnable(GL_DEPTH_TEST);
    draw_world_geometry(cam);*/
    glDisable(GL_DEPTH_TEST);

    //2D render
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // https://stackoverflow.com/questions/2571402/how-to-use-glortho-in-opengl

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    gl_render_root_gui(window_width, window_height); // note
    glDisable(GL_TEXTURE_2D);
}

#endif
