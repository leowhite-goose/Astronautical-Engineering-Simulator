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
    glProgramUniformMatrix4fv = (glProgramUniformMatrix4fv_func) SDL_GL_GetProcAddress("glProgramUniformMatrix4fv_func");
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

void render_triangles(uint32 vertex_count, uint32 triangle_count, vec32i3f **vertf, vec32i3i128 **vert128, vec4i32 **triangles, uint32 vert_index_cnt) {
    //SDL_Log("%" SDL_PRIu32, triangle_count);
    //SDL_Log("HYIj - %.3f", (float) (*vert128)[6].x);
    vert128_to_verf(&vertex_count, vert128, vertf);
    //SDL_Log("HYIf - %.3f", (float) (*vertf)[6].x);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    for (int i = 0; i < triangle_count; i++) {
        if ((*triangles)[i].w >= 0) {
            vec3f normal = generate_normal((*vertf)[(*triangles)[i].x], (*vertf)[(*triangles)[i].y], (*vertf)[(*triangles)[i].z]);
            float temp_normal_data[9] = {
                normal.x, normal.y, normal.z, // v0 v1 v2
                normal.x, normal.y, normal.z,
                normal.x, normal.y, normal.z,
            };
            float temp_color_data[12] = {
                0.8,0.8,0.8,1.0,
                0.8,0.8,0.8,1.0,
                0.8,0.8,0.8,1.0
            };
            float temp_vertex_data[9] = {
                (*vertf)[(*triangles)[i].x].x, (*vertf)[(*triangles)[i].x].y, (*vertf)[(*triangles)[i].x].z, // v0
                (*vertf)[(*triangles)[i].y].x, (*vertf)[(*triangles)[i].y].y, (*vertf)[(*triangles)[i].y].z, // v1
                (*vertf)[(*triangles)[i].z].x, (*vertf)[(*triangles)[i].z].y, (*vertf)[(*triangles)[i].z].z, // v2
            };
            //SDL_Log("Tri_x - %.3f", temp_vertex_data[0]);
            for (int j = 0; j < 9; j++) {
                normal_data[j + 9 * i] = temp_normal_data[j];
            }
            for (int j = 0; j < 12; j++) {
                color_data[j + 12 * i] = temp_color_data[j];
            }
            for (int j = 0; j < 9; j++) {
                vertex_data[j + 9 * i] = temp_vertex_data[j];
            }
        } else {
            float temp_normal_data[9] = {
                0,0,0,
                0,0,0,
                0,0,0
            };
            float temp_color_data[12] = {
                0,0,0,0,
                0,0,0,0,
                0,0,0,0
            };
            float temp_vertex_data[9] = {
                0,0,0,
                0,0,0,
                0,0,0
            };
            for (int j = 0; j < 9; j++) {
                normal_data[j + 9 * i] = temp_normal_data[j];
            }
            for (int j = 0; j < 12; j++) {
                color_data[j + 12 * i] = temp_color_data[j];
            }
            for (int j = 0; j < 9; j++) {
                vertex_data[j + 9 * i] = temp_vertex_data[j];
            }
        }
    }
    glVertexPointer(3, GL_FLOAT, 0, vertex_data);
    glNormalPointer(GL_FLOAT, 0, normal_data);
    glColorPointer(4, GL_FLOAT, 0, color_data);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glDrawArrays(GL_TRIANGLES, 0, 3 * triangle_count);
   /* int32 triangle_indicies[vert_index_cnt];
    for (int j = 0; j < vert_index_cnt; j++) {
        triangle_indicies[j] = (*vertf)[j].w;
        SDL_Log("%" SDL_PRIs32, triangle_indicies[j]);
    }
    glDrawElements(GL_TRIANGLES, triangle_count * 3, GL_UNSIGNED_INT, triangle_indicies);*/
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_CULL_FACE);
}

void render_tetrahedra(vec32i3f **nodes, vec5i32 **cells, vec32i3i128 **vert128, uint32 vertex_count, int32 cell_count, bool debug) {
    vert128_to_verf(&vertex_count, vert128, nodes);
    if (!debug) {glEnable(GL_CULL_FACE); glCullFace(GL_BACK);}
    for (int i = 0; i < cell_count; i++) {
        vec3f normal0 = generate_normal((*nodes)[(*cells)[i].b], (*nodes)[(*cells)[i].x], (*nodes)[(*cells)[i].y]);
        vec3f normal1 = generate_normal((*nodes)[(*cells)[i].z], (*nodes)[(*cells)[i].y], (*nodes)[(*cells)[i].x]); // flipped
        vec3f normal2 = generate_normal((*nodes)[(*cells)[i].b], (*nodes)[(*cells)[i].y], (*nodes)[(*cells)[i].z]);
        vec3f normal3 = generate_normal((*nodes)[(*cells)[i].z], (*nodes)[(*cells)[i].x], (*nodes)[(*cells)[i].b]); // flipped
        float temp_normal_data[36] = {
            normal0.x, normal0.y, normal0.z, // v0 v1 v2
            normal0.x, normal0.y, normal0.z,
            normal0.x, normal0.y, normal0.z,

            normal1.x, normal1.y, normal1.z, // v3 v2 v1
            normal1.x, normal1.y, normal1.z,
            normal1.x, normal1.y, normal1.z,

            normal2.x, normal2.y, normal2.z, // v0 v2 v3
            normal2.x, normal2.y, normal2.z,
            normal2.x, normal2.y, normal2.z,

            normal3.x, normal3.y, normal3.z, // v3
            normal3.x, normal3.y, normal3.z, // v1
            normal3.x, normal3.y, normal3.z  // v0
        };
        float transparency_;
        if (debug) {
            transparency_ = 0.5;
        } else {
            transparency_ = 1.0;
        }
        float temp_color_data[48] = {
            1.0,0.0,0.0,transparency_,
            1.0,0.0,0.0,transparency_,
            1.0,0.0,0.0,transparency_,

            0.0,1.0,0.0,transparency_,
            0.0,1.0,0.0,transparency_,
            0.0,1.0,0.0,transparency_,

            0.0,0.0,1.0,transparency_,
            0.0,0.0,1.0,transparency_,
            0.0,0.0,1.0,transparency_,

            1.0,0.0,1.0,transparency_,
            1.0,0.0,1.0,transparency_,
            1.0,0.0,1.0,transparency_
        };
        float temp_vertex_data[36] = {
            (*nodes)[(*cells)[i].b].x, (*nodes)[(*cells)[i].b].y, (*nodes)[(*cells)[i].b].z, // v0
            (*nodes)[(*cells)[i].x].x, (*nodes)[(*cells)[i].x].y, (*nodes)[(*cells)[i].x].z, // v1
            (*nodes)[(*cells)[i].y].x, (*nodes)[(*cells)[i].y].y, (*nodes)[(*cells)[i].y].z, // v2

            (*nodes)[(*cells)[i].z].x, (*nodes)[(*cells)[i].z].y, (*nodes)[(*cells)[i].z].z, // v3
            (*nodes)[(*cells)[i].y].x, (*nodes)[(*cells)[i].y].y, (*nodes)[(*cells)[i].y].z, // v2
            (*nodes)[(*cells)[i].x].x, (*nodes)[(*cells)[i].x].y, (*nodes)[(*cells)[i].x].z, // v1

            (*nodes)[(*cells)[i].b].x, (*nodes)[(*cells)[i].b].y, (*nodes)[(*cells)[i].b].z, // v0
            (*nodes)[(*cells)[i].y].x, (*nodes)[(*cells)[i].y].y, (*nodes)[(*cells)[i].y].z, // v2
            (*nodes)[(*cells)[i].z].x, (*nodes)[(*cells)[i].z].y, (*nodes)[(*cells)[i].z].z, // v3

            (*nodes)[(*cells)[i].z].x, (*nodes)[(*cells)[i].z].y, (*nodes)[(*cells)[i].z].z, // v3
            (*nodes)[(*cells)[i].x].x, (*nodes)[(*cells)[i].x].y, (*nodes)[(*cells)[i].x].z, // v1
            (*nodes)[(*cells)[i].b].x, (*nodes)[(*cells)[i].b].y, (*nodes)[(*cells)[i].b].z  // v0
        };
        for (int j = 0; j < 36; j++) {
            normal_data[j + 36 * i] = temp_normal_data[j];
        }
        for (int j = 0; j < 48; j++) {
            color_data[j + 48 * i] = temp_color_data[j];
        }
        for (int j = 0; j < 36; j++) {
            vertex_data[j + 36 * i] = temp_vertex_data[j];
        }
    }
    /*GLuint vertex_vbo; // https://stackoverflow.com/questions/22298193/setting-color-attribute-for-a-vbo-in-opengl-using-the-fixed-function-pipeline
     *   glGenBuffers(1, &vertex_vbo); // https://stackoverflow.com/questions/14234361/opengl-using-vbo-with-stdvector
     *   glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo); // https://stackoverflow.com/questions/6696688/how-do-i-fix-the-following-gcc-warnings
     *   glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), &vertex_data, GL_DYNAMIC_DRAW);
     *   glBindBuffer(GL_ARRAY_BUFFER, 0);
     *
     *   glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
     *   glVertexPointer(3, GL_FLOAT, 0, 0);*/

    glVertexPointer(3, GL_FLOAT, 0, vertex_data);

    glNormalPointer(GL_FLOAT, 0, normal_data);

    glColorPointer(4, GL_FLOAT, 0, color_data);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glDrawArrays(GL_TRIANGLES, 0, 12 * cell_count);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    //glBindBuffer(GL_ARRAY_BUFFER,0);

    if (!debug) {glDisable(GL_CULL_FACE);}
}

void draw_world_geometry(vec3f3i128 cam) {
    glTranslatef(cam.y, -cam.z, cam.x);
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

    render_triangles(body[0].geo.vert_cnt, body[0].geo.tetra_cnt * 4, &body[0].geo.vertf, &body[0].geo.vert128, &body[0].geo.tri, body[0].geo.vert_index_cnt); // enterprise

    mat_emmision[0] = 0.5;
    mat_emmision[1] = 0.5;
    mat_emmision[2] = 0.5;
    mat_emmision[3] = 1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emmision);
    render_triangles(body[2].geo.vert_cnt, body[2].geo.tetra_cnt * 4, &body[2].geo.vertf, &body[2].geo.vert128, &body[2].geo.tri, body[2].geo.vert_index_cnt); // planet
    mat_emmision[0] = 0;
    mat_emmision[1] = 0;
    mat_emmision[2] = 0;
    mat_emmision[3] = 1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emmision);

    glDisable(GL_LIGHTING);

    render_tetrahedra(&body[1].geo.vertf, &body[1].geo.tetra, &body[1].geo.vert128, body[1].geo.vert_cnt, body[1].geo.tetra_cnt, 1); // rainbow-prise

    glRotatef(90, 1.0, 0.0, 0.0);
    glRotatef(-90, 0.0, 1.0, 0.0);
    glTranslatef(-cam.y, cam.z, -cam.x);
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
    perspectivef(60.0f, (float) window_width / window_height, (float) SDL_pow(2,31), (float) SDL_pow(2,40), projection_matrix_0);
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
