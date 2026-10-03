#ifndef RENDER3D_C
#define RENDER3D_C

void AES_init_opengl() {
    // create root openGL context;
    root_gl_context = SDL_GL_CreateContext(root_window);
    #ifdef __EMSCRIPTEN__
    SDL_SetWindowFillDocument(root_window, true);
    #endif
    SDL_GL_SetSwapInterval(1); // note

    int opengl_major_version, opengl_minor_version, opengl_profile, depth_size, MSAA_level;;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &opengl_major_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &opengl_minor_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &opengl_profile);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth_size);
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &MSAA_level);
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
    SDL_Log("MSAA Level : %dx", MSAA_level);
    /* Get function pointers for opengl_ext functions */
    glGenVertexArrays = (glGenVertexArrays_func) SDL_GL_GetProcAddress("glGenVertexArrays");
    glBindVertexArray = (glBindVertexArray_func) SDL_GL_GetProcAddress("glBindVertexArray");
    #ifdef SDL_PLATFORM_WIN32
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
    glActiveTexture_ = (glActiveTexture_func) SDL_GL_GetProcAddress("glActiveTexture");
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
    //SDL_Log("%s", vertex_shader_src);
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

void AES_generate_unlit_shaders() {
    int success;
    char info_log[512];

    char *vertex_shader_src;
    char *fragment_shader_src;

    char vertex_shader_rel_path[] = "src/graphics/vertex_shader.glsl";
    char *vertex_path = NULL;
    SDL_asprintf(&vertex_path, "%s" "%s", SDL_GetBasePath(), vertex_shader_rel_path);
    vertex_shader_src = SDL_LoadFile(vertex_path, NULL);

    char fragment_shader_rel_path[] = "src/graphics/non_lit_fragment_shader.glsl";
    char *fragment_path = NULL;
    SDL_asprintf(&fragment_path, "%s" "%s", SDL_GetBasePath(), fragment_shader_rel_path);
    fragment_shader_src = SDL_LoadFile(fragment_path, NULL);

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char *vertex_shader_source = vertex_shader_src;
    //SDL_Log("%s", vertex_shader_src);
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

    unlit_shader = glCreateProgram();
    glAttachShader(unlit_shader, vertex_shader);
    glAttachShader(unlit_shader, fragment_shader);
    glLinkProgram(unlit_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glGetProgramiv(unlit_shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(unlit_shader, 512, NULL, info_log);
        SDL_Log("Shader Program Error: %s", info_log);
    }
}

void AES_generate_tex_shaders() {
    int success;
    char info_log[512];

    char *vertex_shader_src;
    char *fragment_shader_src;

    char vertex_shader_rel_path[] = "src/graphics/gui_vertex_shader.glsl";
    char *vertex_path = NULL;
    SDL_asprintf(&vertex_path, "%s" "%s", SDL_GetBasePath(), vertex_shader_rel_path);
    vertex_shader_src = SDL_LoadFile(vertex_path, NULL);

    char fragment_shader_rel_path[] = "src/graphics/gui_fragment_shader.glsl";
    char *fragment_path = NULL;
    SDL_asprintf(&fragment_path, "%s" "%s", SDL_GetBasePath(), fragment_shader_rel_path);
    fragment_shader_src = SDL_LoadFile(fragment_path, NULL);

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char *vertex_shader_source = vertex_shader_src;
    //SDL_Log("%s", vertex_shader_src);
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

    tex_shader_program = glCreateProgram();
    glAttachShader(tex_shader_program, vertex_shader);
    glAttachShader(tex_shader_program, fragment_shader);
    glLinkProgram(tex_shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glGetProgramiv(tex_shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(tex_shader_program, 512, NULL, info_log);
        SDL_Log("Shader Program Error: %s", info_log);
    }
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

void render_triangles(uint32 vertex_count, uint32 triangle_count, vec32i3f **vertf, vec32i3i128 **vert128, vec4i32 **triangles, uint32 vert_index_cnt, vec3f3i128 cam, vec4f color, int32 *skipped_cnt, vec6i128 AABB, bool bounding_box) {
    //SDL_Log("%" SDL_PRIu32, triangle_count);
    //SDL_Log("HYIj - %.3f", (float) (*vert128)[6].x);
    vec32i3i128 *vert128local = (vec32i3i128 *) SDL_malloc(sizeof(vec32i3i128) * vertex_count);
    for (int i = 0; i < vertex_count; i++) {
        if (!hold_rendering) { // so physics' intermediate steps aren't loaded in, here
            vert128local[i] = (*vert128)[i]; // so that this doesn't modify data physics engine is using
        } else {
            SDL_DelayNS(0);
            i -= 1;
        }
    }
    vec3i128 translate;
    translate.x = -cam.x;
    translate.y = -cam.y;
    translate.z = -cam.z;
    vert128_translate(&vertex_count, &vert128local, translate);
    vert128_to_verf_i(&vertex_count, &vert128local, vertf);
    SDL_free(vert128local);
    for (int i = 0; i < triangle_count; i++) {
        //vec3f normal = generate_normal((*vertf)[(*triangles)[i].x], (*vertf)[(*triangles)[i].y], (*vertf)[(*triangles)[i].z]);
        vec3d normald = generate_normald((*vertf)[(*triangles)[i].x], (*vertf)[(*triangles)[i].y], (*vertf)[(*triangles)[i].z]);
        vec3f normal = {(float)normald.x,(float) normald.y,(float) normald.z};
        float transparency_ = 1.0;
        if ((*triangles)[i].w >= 0) {
            float temp_vertex_data[30] = {
                (*vertf)[(*triangles)[i].x].x, (*vertf)[(*triangles)[i].x].y, (*vertf)[(*triangles)[i].x].z, color.w, color.x, color.y, color.z, normal.x, normal.y, normal.z, // v0
                (*vertf)[(*triangles)[i].y].x, (*vertf)[(*triangles)[i].y].y, (*vertf)[(*triangles)[i].y].z, color.w, color.x, color.y, color.z, normal.x, normal.y, normal.z, // v1
                (*vertf)[(*triangles)[i].z].x, (*vertf)[(*triangles)[i].z].y, (*vertf)[(*triangles)[i].z].z, color.w, color.x, color.y, color.z, normal.x, normal.y, normal.z  // v2
            };
            int32 size = sizeof(float) * 30;
            int32 size_offset = sizeof(float) * (30 * (i - (*skipped_cnt)));
            glBufferSubData(GL_ARRAY_BUFFER, size_offset, size, &temp_vertex_data[0]); // glMapBuffer
        } else {
            (*skipped_cnt)++;
        }
    }
    if (bounding_box) {
        vec32i3i128 AA_BB[2] = {{0, AABB.a, AABB.b, AABB.c}, {1, AABB.x, AABB.y, AABB.z}};
        vec32i3i128 *AABB_ptr = (vec32i3i128*) &AA_BB;
        int32 aabb_cnt = 2;
        vert128_translate(&aabb_cnt, &AABB_ptr, translate);
        vec4f color_f = {1,1,1,1};
        float temp_box_data[30 * 12] = {
            (float) AA_BB[0].x, (float) AA_BB[0].y, (float) AA_BB[0].z , color_f.w, color_f.x, color_f.y, color_f.z, 0, 0, 0, // v0 --> lines
            (float) AA_BB[1].x, (float) AA_BB[0].y, (float) AA_BB[0].z , color_f.w, color_f.x, color_f.y, color_f.z, 0, 0, 0, // v1
            (float) AA_BB[0].x, (float) AA_BB[0].y, (float) AA_BB[0].z , color_f.w, color_f.x, color_f.y, color_f.z, 0, 0, 0, // v0
            (float) AA_BB[0].x, (float) AA_BB[1].y, (float) AA_BB[0].z , color_f.w, color_f.x, color_f.y, color_f.z, 0, 0, 0, // v2
            (float) AA_BB[0].x, (float) AA_BB[0].y, (float) AA_BB[0].z , color_f.w, color_f.x, color_f.y, color_f.z, 0, 0, 0, // v0
            (float) AA_BB[0].x, (float) AA_BB[0].y, (float) AA_BB[1].z , color_f.w, color_f.x, color_f.y, color_f.z, 0, 0, 0, // v3

            (float) AA_BB[1].x, (float) AA_BB[1].y, (float) AA_BB[1].z , color_f.w, color_f.x, color_f.y, color_f.z, 0, 0, 0,
            (float) AA_BB[0].x, (float) AA_BB[1].y, (float) AA_BB[1].z , color_f.w, color_f.x, color_f.y, color_f.z, 0, 0, 0,
            (float) AA_BB[1].x, (float) AA_BB[1].y, (float) AA_BB[1].z , color_f.w, color_f.x, color_f.y, color_f.z, 0, 0, 0,
            (float) AA_BB[1].x, (float) AA_BB[0].y, (float) AA_BB[1].z , color_f.w, color_f.x, color_f.y, color_f.z, 0, 0, 0,
            (float) AA_BB[1].x, (float) AA_BB[1].y, (float) AA_BB[1].z , color_f.w, color_f.x, color_f.y, color_f.z, 0, 0, 0,
            (float) AA_BB[1].x, (float) AA_BB[1].y, (float) AA_BB[0].z , color_f.w, color_f.x, color_f.y, color_f.z, 0, 0, 0
        };
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * (30 * (triangle_count - (*skipped_cnt))), sizeof(float) * 30, &temp_box_data[0]);
    }
}

void render_tetrahedra(vec32i3f **nodes, vec5i32 **cells, vec32i3i128 **vert128, uint32 vertex_count, int32 cell_count, bool debug, vec3f3i128 cam) {
    vec3i128 translate;
    translate.x = -cam.x;
    translate.y = -cam.y;
    translate.z = -cam.z;
    vert128_translate(&vertex_count, vert128, translate);
    vert128_to_verf_i(&vertex_count, vert128, nodes);
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
        int32 size = sizeof(float) * 120;
        int32 size_offset = sizeof(float) * (120 * i);
        glBufferSubData(GL_ARRAY_BUFFER, size_offset, size, &temp_vertex_data[0]); // glMapBuffer
    }
}

/*void draw_world_geometry(vec3f3i128 cam) {
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
}*/

void render_body(int32 id, vec4f color, bool is_lit, bool bounding_box, bool occluded) {
    //glBindVertexArray(vao);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    /*float vertices[] = {
        0 .5f,  0.5f,* -1.8f,   0.2f, 0.2f, 0.5f, 0.5f, // top right
        0.5f, -0.5f, -1.8f,   0.2f, 0.2f, 0.5f, 0.5f,// bottom right
        -0.5f, -0.5f, -1.9f,  1.0f, 0.0f, 0.0f, 0.5f,// bottom left
        -0.5f,  0.5f, -1.9f,  1.0f, 0.0f, 0.0f, 0.5f// top left
    };
    unsigned int indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };*/
    int32 skipped_cnt = 0;
    render_triangles(body[id].geo.vert_cnt, body[id].geo.tetra_cnt * 4, &body[id].geo.vertf, &body[id].geo.vert128, &body[id].geo.tri, body[id].geo.vert_index_cnt, root_cam, color, &skipped_cnt, body[id].CM.AABB, bounding_box); // enterprise
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7* sizeof(float)));
    glEnableVertexAttribArray(2);

    const float identity_matrix[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    float projection_matrix[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}; // 4x4 perspective matrix
    perspective(60.0f, (float) root_window_width / root_window_height, (float) SDL_pow(2,0), (float) SDL_pow(2,128), projection_matrix);
    mat4 view_matrix;
    glm_mat4_identity(view_matrix);
    glm_rotate(view_matrix, ((root_cam.b-90)*SDL_PI_F/180), (vec3) {1,0,0});
    glm_rotate(view_matrix, ((-root_cam.a)*SDL_PI_F/180), (vec3) {0,1,0});
    glm_rotate(view_matrix, ((root_cam.c)*SDL_PI_F/180), (vec3) {0,0,1});
    glm_rotate(view_matrix, ((90)*SDL_PI_F/180), (vec3) {0,1,0});
    glm_rotate(view_matrix, ((-90)*SDL_PI_F/180), (vec3) {1,0,0});
    unsigned int view_loc;
    unsigned int proj_loc;
    unsigned int light_pos_loc;
    if (is_lit) {
        glUseProgram(shader_program);
    } else {
        glUseProgram(unlit_shader);
    }
    if (is_lit) {
        view_loc = glGetUniformLocation(shader_program, "view");
        proj_loc = glGetUniformLocation(shader_program, "projection");
        light_pos_loc = glGetUniformLocation(shader_program, "light_pos");
    } else {
        view_loc = glGetUniformLocation(unlit_shader, "view");
        proj_loc = glGetUniformLocation(unlit_shader, "projection");
        light_pos_loc = glGetUniformLocation(unlit_shader, "light_pos");
    }
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(proj_loc, 1, GL_TRUE, &projection_matrix[0]);
    //glUniform3f(light_pos_loc, 0, 0, 0);
    glUniform3f(light_pos_loc, (float) body[0].CM_prev.t.p.x - root_cam.x, (float) body[0].CM_prev.t.p.y - root_cam.y, (float) body[0].CM_prev.t.p.z - root_cam.z);

    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDrawArrays(GL_TRIANGLES, 0, body[id].geo.tetra_cnt*12 - skipped_cnt*3);
    if (bounding_box == true) {
        glUseProgram(unlit_shader);
        glDrawArrays(GL_LINES, body[id].geo.tetra_cnt*12 - skipped_cnt*3, 6);
    }
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(0);
}

void render3D(float window_width, float window_height, vec3f3i128 cam) {
    vec4f color0 = {1.0,1.0,1.0,1.0};
    vec4f color1 = {0.7,0.7,0.7,1.0};
    vec4f color2 = {0.8,0.7,0.4,1.0};
    vec4f color3 = {0.5,0.6,0.8,1.0};
    vec4f color4 = {0.7,0.7,0.7,1.0};
    vec4f color5 = {0.8,0.5,0.5,1.0};
    vec4f color6 = {0.7,0.6,0.4,1.0};
    vec4f color7 = {0.7,0.6,0.3,1.0};
    vec4f color8 = {0.5,0.6,0.8,1.0};
    vec4f color9 = {0.5,0.6,0.9,1.0};
    vec4f color10 = {0.6,0.6,0.6,1.0};
    vec4f color11 = {0.6,0.6,0.6,1.0};
    render_body(0,color0,0,0,0);
    render_body(1,color1,1,0,0);
    render_body(2,color2,1,0,0);
    render_body(3,color3,1,0,0);
    render_body(4,color4,1,0,0);
    render_body(5,color5,1,0,0);
    render_body(6,color6,1,0,0);
    render_body(7,color7,1,0,0);
    render_body(8,color8,1,0,0);
    render_body(9,color9,1,0,0);
    render_body(10,color10,1,0,0);
    render_body(11,color11,1,0,0);
}

#endif
