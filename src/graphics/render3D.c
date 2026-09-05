#ifndef RENDER3D_C
#define RENDER3D_C

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
    matrix[14] = (2 * z_far * z_near) / (z_near - z_far);
    matrix[15] = 0;
}

void draw_world_geometry(vec6f cam) {
    glTranslatef(cam.y, -cam.z, cam.x);
    glRotatef(90, 0.0, 1.0, 0.0);
    glRotatef(-90, 1.0, 0.0, 0.0);

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1);
    glEnable(GL_LIGHT0);
    float light0_pos[4] = {cam.x, cam.y, cam.z, 1};
    float atten_terms[3] = {0.00001,0,0};
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

    static meshf *earth_mesh;
    if (!earth_mesh) {
        earth_mesh = create_UV_sphere_mesh(1.0, 12, 8); // "earth"
        vec4f color = {1.0, 0.2, 0.4, 0.2};
        set_mesh_color(color, earth_mesh);
    }
    vec3f position = {10,1,1};
    render_mesh(earth_mesh, position);
    vec6f box = {9,0,0,11,2,2};
    global_fp = vector_to_box_distance(cam, box);
    static meshf *atmosphere_mesh;
    if (!atmosphere_mesh) {
        atmosphere_mesh = create_UV_sphere_mesh(1.05, 12, 8); // "atmosphere"
        vec4f color = {0.5, 0.2, 0.2, 0.4};
        set_mesh_color(color, atmosphere_mesh);
    }
    render_mesh(atmosphere_mesh, position);

    glDisable(GL_LIGHTING);
    glEnd();

    if (touch_button[0]) {
        glBegin(GL_TRIANGLES); // colliding blue triangles test
        glVertex3f(p1a.x, p1a.y, p1a.z);
        glVertex3f(p1b.x, p1b.y, p1b.z);
        glVertex3f(p1c.x, p1c.y, p1c.z);
        glVertex3f(p2a.x, p2a.y, p2a.z);
        glVertex3f(p2b.x, p2b.y, p2b.z);
        glVertex3f(p2c.x, p2c.y, p2c.z);
        glEnd();
    }

    glEnable(GL_LIGHTING);
    render_tetrahedra(&model_vertices, &model_tetrahedra, model_cell_count, position, 0); // loading 3D model testing
    glDisable(GL_LIGHTING);

    glTranslatef(200, 0, 0);
    render_tetrahedra(&model_vertices, &model_tetrahedra, model_cell_count, position, 1);
    glTranslatef(-200, 0, 0);

    glRotatef(90, 1.0, 0.0, 0.0);
    glRotatef(-90, 0.0, 1.0, 0.0);
    glTranslatef(-cam.y, cam.z, -cam.x);
}

void render3D(float window_width, float window_height, vec6f cam) {
    glViewport(0, 0, window_width, window_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //3D render
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float projection_matrix_0[16]; // 4x4 perspective matrix

    perspectivef(60.0f, (float) window_width / window_height, (float) SDL_pow(2,0), (float) SDL_pow(2,126), projection_matrix_0);
    glLoadMatrixf(projection_matrix_0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(cam.b - 90, 1.0f, 0.0f, 0.0f);
    glRotatef(-cam.a    , 0.0f, 1.0f, 0.0f);
    glRotatef(cam.c     , 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    draw_world_geometry(cam);
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
