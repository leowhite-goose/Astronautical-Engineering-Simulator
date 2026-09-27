#ifndef MAINLOOP_C
#define MAINLOOP_C

SDL_AppResult AES_mainloop() {
    static uint64 last_fps;
    #ifdef __EMSCRIPTEN__
    bool fill = true;
    SDL_SetWindowFillDocument(root_window, fill);
    #endif
    // updating global variables;
    SDL_GetWindowSizeInPixels(root_window, &root_window_width, &root_window_height);
    if ((SDL_max(root_window_width, root_window_height) > gui_texture_res) || (SDL_max(root_window_width, root_window_height) * 2 < gui_texture_res)) {
        gui_texture_res = power_of_two(SDL_max(root_window_width, root_window_height)); // account for max supported resolution, like with webgl for browsers
        SDL_DestroySurface(root_gui_surface);
        root_gui_surface =  SDL_CreateSurface(gui_texture_res, gui_texture_res, SDL_PIXELFORMAT_ABGR8888);
        SDL_DestroyRenderer(root_gui_renderer);
        root_gui_renderer = SDL_CreateSoftwareRenderer(root_gui_surface);
        //SDL_Log("GUI res:%" SDL_PRIu32, gui_texture_res);
    }
    int64 start_time = SDL_GetTicksNS();
    SDL_DelayNS(0);
    int64 end_time = SDL_GetTicksNS();
    shortest_delay_ns = end_time - start_time;

    // control mapping
    int vsync = !key_toggle(SDL_SCANCODE_V);
    float cursor_pan_x;
    float cursor_pan_y;
    bool move_cam_with_mouse = true;
    int cam_move_forward = key_down(SDL_SCANCODE_W) - key_down(SDL_SCANCODE_S) + touch_button[2] - touch_button[4];
    int cam_move_right = key_down(SDL_SCANCODE_A) - key_down(SDL_SCANCODE_D) + touch_button[1] - touch_button[3];
    int cam_move_up = key_down(SDL_SCANCODE_R) - key_down(SDL_SCANCODE_F) + touch_button[5] - touch_button[6];
    char last_key[16];
    last_key_down(last_key, sizeof(last_key));
    if (SDL_strcmp(last_key, "No keys down")) {
        SDL_Log("%s", last_key);
    }
    //bool is_fullscreen = key_toggle(SDL_SCANCODE_F11);
    bool pan_camera = (mouse.right.toggle || touch_button[7]) && (root_window == SDL_GetMouseFocus());
    static int cam_speed = 35;
    static int pan_sensitivity = -2;
    if (mouse.scrolling && key_down(SDL_SCANCODE_LSHIFT)) {cam_speed += mouse.wheel.y;}
    if (mouse.scrolling && key_down(SDL_SCANCODE_LCTRL)) {pan_sensitivity += mouse.wheel.y;}
    float cam_vel = SDL_pow(2,cam_speed)/last_fps;
    float pan_x, pan_y;
    if (!touch_button[7]) {
        pan_x = mouse.x_rel * SDL_pow(2,pan_sensitivity/4);
        pan_y = mouse.y_rel * SDL_pow(2,pan_sensitivity/4);
    } else {
        pan_x = touch_analog[0] * SDL_pow(2,pan_sensitivity/4);
        pan_y = touch_analog[1] * SDL_pow(2,pan_sensitivity/4);
    }
    //SDL_SetWindowFullscreen(root_window, is_fullscreen);
    //SDL_Log("x%.3f y%.3f", pan_x, pan_y);

    // camera panning
    if (pan_camera) {
        if (move_cam_with_mouse) {SDL_SetWindowRelativeMouseMode(root_window, true);}
        if (mouse.moving || touch_button[7]) {
            if (root_cam.b + pan_y > 180) {
                root_cam.b = 180;
            } else if (root_cam.b + pan_y < 0){
                root_cam.b = 0;
            } else {
                root_cam.b += pan_y;
            }

            root_cam.a -= pan_x;
            while (root_cam.a >= 360) {
                root_cam.a = root_cam.a - 360;
            }
            while (root_cam.a < 0) {
                root_cam.a = root_cam.a + 360;
            }
        }
    } else {
        if (move_cam_with_mouse) {SDL_SetWindowRelativeMouseMode(root_window, false);}
    }
    // camera moving
    if (cam_move_forward) {
        root_cam.y -= cam_move_forward * cam_vel * SDL_sinf(-root_cam.b*SDL_PI_F/180) * SDL_sinf(root_cam.a*SDL_PI_F/180);
        root_cam.z += cam_move_forward * cam_vel * SDL_cosf(-root_cam.b*SDL_PI_F/180);
        root_cam.x -= cam_move_forward * cam_vel * SDL_sinf(-root_cam.b*SDL_PI_F/180) * SDL_cosf(root_cam.a*SDL_PI_F/180);
    }
    if (cam_move_right) {
        root_cam.y += cam_move_right * cam_vel * SDL_cosf(root_cam.a*SDL_PI_F/180);
        root_cam.z += cam_move_right * cam_vel * 0;
        root_cam.x -= cam_move_right * cam_vel * SDL_sinf(root_cam.a*SDL_PI_F/180);
    }
    if (cam_move_up) {
        root_cam.y -= cam_move_up * cam_vel * SDL_cosf(-root_cam.b*SDL_PI_F/180) * SDL_sinf(root_cam.a*SDL_PI_F/180);
        root_cam.z -= cam_move_up * cam_vel * SDL_sinf(-root_cam.b*SDL_PI_F/180);
        root_cam.x -= cam_move_up * cam_vel * SDL_cosf(-root_cam.b*SDL_PI_F/180) * SDL_cosf(root_cam.a*SDL_PI_F/180);
    }

    // 2D rendering (GUI overlay)
    onscreen_overlay(cam_speed, pan_sensitivity, last_fps, last_tps, root_window_width, root_window_height);

    // 3D rendering
    SDL_GL_SetSwapInterval(vsync);
    render3D(root_window_width, root_window_height, root_cam);

    // draw a rectangle
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    float vertices[] = {
        0.5f,  0.5f, -1.8f,  // top right
        0.5f, -0.5f, -1.8f,  // bottom right
        -0.5f, -0.5f, -1.9f,  // bottom left
        -0.5f,  0.5f, -1.9f   // top left
    };
    unsigned int indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(shader_program);

    //float projection_matrix[16]; // 4x4 perspective matrix
    const float identity_matrix[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    float view_matrix[4][4] = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
    float projection_matrix[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    perspective(60.0f, (float) root_window_width / root_window_height, (float) 0.1, (float) 100, projection_matrix);
    unsigned int view_loc = glGetUniformLocation(shader_program, "view");
    unsigned int proj_loc = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view_matrix[0][0]);
    /*float *view_uniform = (float*) SDL_malloc(sizeof(float) * 16);
    glGetnUniformfv(shader_program, view_loc, sizeof(float) * 16, view_uniform);
    SDL_Log("Uniform - %.3f", *(view_uniform+0));*/
    glUniformMatrix4fv(proj_loc, 1, GL_TRUE, &projection_matrix[0]);
    //SDL_Log("projloc %" SDL_PRIu32, proj_loc);
    //SDL_Log("viewloc %" SDL_PRIu32, view_loc);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    glFlush(); // note
    SDL_GL_SwapWindow(root_window);

    // resetting active control state
    mouse.moving = false;
    mouse.scrolling = false;

    // FPS
    static uint64 accu = 0;
    static uint64 last_tick_update_ns = 0;
    static uint64 last_tick_end_ns = 0;
    static uint64 tick_end_ns = 0;
    static uint64 elapsed_ns = 0;
    tick_end_ns = SDL_GetTicksNS(); // this is called at end of each tick
    elapsed_ns = tick_end_ns - last_tick_end_ns;
    if (tick_end_ns - last_tick_update_ns >= SDL_NS_PER_SECOND) { // for updating TPS polling counter (each second)
        last_tick_update_ns = tick_end_ns;
        last_fps = accu;
        accu = 0;
    }
    accu += 1;
    last_tick_end_ns = SDL_GetTicksNS();

    return SDL_APP_CONTINUE;  // carry on with the program!
}

#endif
