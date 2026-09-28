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
    static int cam_speed = 15;
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
        float cam_move_y = -cam_move_forward * cam_vel * SDL_sinf(-root_cam.b*SDL_PI_F/180) * SDL_sinf(root_cam.a*SDL_PI_F/180);
        float cam_move_z = cam_move_forward * cam_vel * SDL_cosf(-root_cam.b*SDL_PI_F/180);
        float cam_move_x = -cam_move_forward * cam_vel * SDL_sinf(-root_cam.b*SDL_PI_F/180) * SDL_cosf(root_cam.a*SDL_PI_F/180);

        if (root_cam.y + cam_move_y <= -SDL_powf(2, 126)) {
            root_cam.y = -SDL_powf(2,126);
        } else if (root_cam.y + cam_move_y >= SDL_powf(2, 126)) {
            root_cam.y = SDL_powf(2,126);
        } else {
            root_cam.y += cam_move_y;
        }

        if (root_cam.z + cam_move_z <= -SDL_powf(2, 126)) {
            root_cam.z = -SDL_powf(2,126);
        } if (root_cam.z + cam_move_z >= SDL_powf(2, 126)) {
            root_cam.z = SDL_powf(2,126);
        } else {
            root_cam.z += cam_move_z;
        }

        if (root_cam.x + cam_move_x <= -SDL_powf(2, 126)) {
            root_cam.x = -SDL_powf(2,126);
        } if (root_cam.x + cam_move_x >= SDL_powf(2, 126)) {
            root_cam.x = SDL_powf(2,126);
        } else {
            root_cam.x += cam_move_x;
        }
    }
    if (cam_move_right) {
        float cam_move_y = cam_move_right * cam_vel * SDL_cosf(root_cam.a*SDL_PI_F/180);
        float cam_move_z = cam_move_right * cam_vel * 0;
        float cam_move_x = -cam_move_right * cam_vel * SDL_sinf(root_cam.a*SDL_PI_F/180);

        if (root_cam.y + cam_move_y <= -SDL_powf(2, 126)) {
            root_cam.y = -SDL_powf(2,126);
        } else if (root_cam.y + cam_move_y >= SDL_powf(2, 126)) {
            root_cam.y = SDL_powf(2,126);
        } else {
            root_cam.y += cam_move_y;
        }

        if (root_cam.z + cam_move_z <= -SDL_powf(2, 126)) {
            root_cam.z = -SDL_powf(2,126);
        } if (root_cam.z + cam_move_z >= SDL_powf(2, 126)) {
            root_cam.z = SDL_powf(2,126);
        } else {
            root_cam.z += cam_move_z;
        }

        if (root_cam.x + cam_move_x <= -SDL_powf(2, 126)) {
            root_cam.x = -SDL_powf(2,126);
        } if (root_cam.x + cam_move_x >= SDL_powf(2, 126)) {
            root_cam.x = SDL_powf(2,126);
        } else {
            root_cam.x += cam_move_x;
        }
    }
    if (cam_move_up) {
        float cam_move_y = -cam_move_up * cam_vel * SDL_cosf(-root_cam.b*SDL_PI_F/180) * SDL_sinf(root_cam.a*SDL_PI_F/180);
        float cam_move_z = -cam_move_up * cam_vel * SDL_sinf(-root_cam.b*SDL_PI_F/180);
        float cam_move_x = -cam_move_up * cam_vel * SDL_cosf(-root_cam.b*SDL_PI_F/180) * SDL_cosf(root_cam.a*SDL_PI_F/180);

        if (root_cam.y + cam_move_y <= -SDL_powf(2, 126)) {
            root_cam.y = -SDL_powf(2,126);
        } else if (root_cam.y + cam_move_y >= SDL_powf(2, 126)) {
            root_cam.y = SDL_powf(2,126);
        } else {
            root_cam.y += cam_move_y;
        }

        if (root_cam.z + cam_move_z <= -SDL_powf(2, 126)) {
            root_cam.z = -SDL_powf(2,126);
        } if (root_cam.z + cam_move_z >= SDL_powf(2, 126)) {
            root_cam.z = SDL_powf(2,126);
        } else {
            root_cam.z += cam_move_z;
        }

        if (root_cam.x + cam_move_x <= -SDL_powf(2, 126)) {
            root_cam.x = -SDL_powf(2,126);
        } if (root_cam.x + cam_move_x >= SDL_powf(2, 126)) {
            root_cam.x = SDL_powf(2,126);
        } else {
            root_cam.x += cam_move_x;
        }
    }

    // 3D rendering
    SDL_GL_SetSwapInterval(vsync);
    /*vec3i128 translate;
     translate.x = cam.x;                                                        *
     translate.y = cam.y;
     translate.z = cam.z;
     //vert128_translate(&body[0].geo.vert_cnt, &body[0].geo.vert128, translate);*/

    // draw a rectangle
    glViewport(0, 0, root_window_width, root_window_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    /*float vertices[] = {
        0.5f,  0.5f, -1.8f,   0.2f, 0.2f, 0.5f, 0.5f, // top right
        0.5f, -0.5f, -1.8f,   0.2f, 0.2f, 0.5f, 0.5f,// bottom right
        -0.5f, -0.5f, -1.9f,  1.0f, 0.0f, 0.0f, 0.5f,// bottom left
        -0.5f,  0.5f, -1.9f,  1.0f, 0.0f, 0.0f, 0.5f// top left
    };
    unsigned int indices[] = {
    0, 1, 3,  // first triangle
    1, 2, 3   // second triangle
    };
    */
    float vertices[] = {
        0.5f,  0.5f, -1.8f,   0.2f, 0.2f, 0.5f, 0.5f, // top right
        0.5f, -0.5f, -1.8f,   0.2f, 0.2f, 0.5f, 0.5f, // bottom right
        -0.5f,  0.5f, -1.9f,  1.0f, 0.0f, 0.0f, 0.5f, // top left
        0.5f, -0.5f, -1.8f,   0.2f, 0.2f, 0.5f, 0.5f,// bottom right
        -0.5f, -0.5f, -1.9f,  1.0f, 0.0f, 0.0f, 0.5f,// bottom left
        -0.5f,  0.5f, -1.9f,  1.0f, 0.0f, 0.0f, 0.5f // top left
    };
    //render_tetrahedra(&body[1].geo.vertf, &body[1].geo.tetra, &body[1].geo.vert128, body[1].geo.vert_cnt, body[1].geo.tetra_cnt, 0, root_cam); // rainbow-prise
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 120 * body[1].geo.tetra_cnt, &vertex_data_c[0], GL_DYNAMIC_DRAW); // glBufferSubData
    render_triangles(body[0].geo.vert_cnt, body[0].geo.tetra_cnt * 4, &body[0].geo.vertf, &body[0].geo.vert128, &body[0].geo.tri, body[0].geo.vert_index_cnt, root_cam); // enterprise
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 30 * body[0].geo.tri_cnt, &vertex_data[0], GL_DYNAMIC_DRAW); // glBufferSubData
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7* sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(shader_program);

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
    unsigned int view_loc = glGetUniformLocation(shader_program, "view");
    unsigned int proj_loc = glGetUniformLocation(shader_program, "projection");
    unsigned int light_pos_loc = glGetUniformLocation(shader_program, "light_pos");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view_matrix[0][0]);
    /*float *view_uniform = (float*) SDL_malloc(sizeof(float) * 16);
    glGetnUniformfv(shader_program, view_loc, sizeof(float) * 16, view_uniform);
    SDL_Log("Uniform - %.3f", *(view_uniform+0));*/
    glUniformMatrix4fv(proj_loc, 1, GL_TRUE, &projection_matrix[0]);
    //SDL_Log("projloc %" SDL_PRIu32, proj_loc);
    //SDL_Log("viewloc %" SDL_PRIu32, view_loc);
    glUniform3f(light_pos_loc, 0, 0, 0);

    glBindVertexArray(vao);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glDrawArrays(GL_TRIANGLES, 0, body[1].geo.tetra_cnt*12);
    glDrawArrays(GL_TRIANGLES, 0, body[0].geo.tetra_cnt*12);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(0);
    glUseProgram(0);

    // 2D rendering (GUI overlay)
    onscreen_overlay(cam_speed, pan_sensitivity, last_fps, last_tps, root_window_width, root_window_height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    gl_render_root_gui(root_window_width, root_window_height); // note
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

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
