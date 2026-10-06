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

    bool move_to_body_1 = key_down(SDL_SCANCODE_1);
    if (move_to_body_1) {
        root_cam.a = 210;
        root_cam.b = 90;
        root_cam.c = 0;
        root_cam.x = body[1].geo.vert128[0].x + SCALE*3000e3;
        root_cam.y = body[1].geo.vert128[0].y + SCALE*7000e3; // 7000km
        root_cam.z = body[1].geo.vert128[0].z;
    }
    bool move_to_body_2 = key_down(SDL_SCANCODE_2);
    if (move_to_body_2) {
        root_cam.a = 210;
        root_cam.b = 90;
        root_cam.c = 0;
        root_cam.x = body[2].geo.vert128[0].x + SCALE*5000e3;
        root_cam.y = body[2].geo.vert128[0].y + SCALE*12000e3;
        root_cam.z = body[2].geo.vert128[0].z;
    }
    bool move_to_body_3 = key_down(SDL_SCANCODE_3);
    if (move_to_body_3) {
        root_cam.a = 250;
        root_cam.b = 90;
        root_cam.c = 0;
        root_cam.x = body[3].geo.vert128[0].x + SCALE*5000e3;
        root_cam.y = body[3].geo.vert128[0].y + SCALE*12000e3;
        root_cam.z = body[3].geo.vert128[0].z;
        /*root_cam.x += body[3].CM.t.v.x * SCALE;
        root_cam.y += body[3].CM.t.v.y * SCALE;
        root_cam.z += body[3].CM.t.v.z * SCALE;*/
    }
    bool move_to_body_4 = key_down(SDL_SCANCODE_4);
    if (move_to_body_4) {
        root_cam.a = 210;
        root_cam.b = 90;
        root_cam.c = 0;
        root_cam.x = body[4].geo.vert128[0].x + SCALE*4000e3;
        root_cam.y = body[4].geo.vert128[0].y + SCALE*9000e3;
        root_cam.z = body[4].geo.vert128[0].z;
    }
    bool move_to_body_5 = key_down(SDL_SCANCODE_5);
    if (move_to_body_5) {
        root_cam.a = 210;
        root_cam.b = 90;
        root_cam.c = 0;
        root_cam.x = body[5].geo.vert128[0].x + SCALE*50000e3;
        root_cam.y = body[5].geo.vert128[0].y + SCALE*90000e3;
        root_cam.z = body[5].geo.vert128[0].z;
    }
    bool move_to_body_6 = key_down(SDL_SCANCODE_6);
    if (move_to_body_6) {
        root_cam.a = 210;
        root_cam.b = 90;
        root_cam.c = 0;
        root_cam.x = body[6].geo.vert128[0].x + SCALE*50000e3;
        root_cam.y = body[6].geo.vert128[0].y + SCALE*90000e3;
        root_cam.z = body[6].geo.vert128[0].z;
    }
    bool move_to_body_7 = key_down(SDL_SCANCODE_7);
    if (move_to_body_7) {
        root_cam.a = 210;
        root_cam.b = 90;
        root_cam.c = 0;
        root_cam.x = body[7].geo.vert128[0].x + SCALE*40000e3;
        root_cam.y = body[7].geo.vert128[0].y + SCALE*70000e3;
        root_cam.z = body[7].geo.vert128[0].z;
    }
    bool move_to_body_8 = key_down(SDL_SCANCODE_8);
    if (move_to_body_8) {
        root_cam.a = 210;
        root_cam.b = 90;
        root_cam.c = 0;
        root_cam.x = body[8].geo.vert128[0].x + SCALE*40000e3;
        root_cam.y = body[8].geo.vert128[0].y + SCALE*70000e3;
        root_cam.z = body[8].geo.vert128[0].z;
    }


    bool move_to_body_9 = key_down(SDL_SCANCODE_9);
    if (move_to_body_9) {
        root_cam.a = 45;
        root_cam.b = 75;
        root_cam.c = 0;
        root_cam.x = -67*SCALE;
        root_cam.y = -60*SCALE;
        root_cam.z = -27*SCALE;
    }

    char last_key[16];
    last_key_down(last_key, sizeof(last_key));
    if (SDL_strcmp(last_key, "No keys down")) {
        SDL_Log("%s", last_key);
    }
    //bool is_fullscreen = key_toggle(SDL_SCANCODE_F11);
    bool pan_camera = (mouse.right.toggle || touch_button[7]) && (root_window == SDL_GetMouseFocus());
    static int cam_speed = 58; // 7, 58, 80, 125
    static int pan_sensitivity = -2;
    if (mouse.scrolling && key_down(SDL_SCANCODE_LSHIFT)) {cam_speed += mouse.wheel.y;}
    if (mouse.scrolling && key_down(SDL_SCANCODE_LCTRL)) {pan_sensitivity += mouse.wheel.y;}
    float cam_vel;
    if (last_fps >= 1) {
         cam_vel = SDL_pow(2,cam_speed)/last_fps;
    } else {
        cam_vel = 0;
    }
    float pan_x, pan_y;
    if (!touch_button[7]) {
        pan_x = mouse.x_rel * SDL_pow(2,pan_sensitivity/4);
        pan_y = mouse.y_rel * SDL_pow(2,pan_sensitivity/4);
    } else {
        pan_x = SDL_powf(touch_analog[0],3) * SDL_pow(pan_sensitivity,2);
        pan_y = SDL_powf(touch_analog[1],3) * SDL_pow(pan_sensitivity,2);
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
        int128 cam_move_y = (int128) -cam_move_forward * cam_vel * SDL_sin(-root_cam.b*SDL_PI_D/180) * SDL_sin(root_cam.a*SDL_PI_D/180);
        int128 cam_move_z = (int128) cam_move_forward * cam_vel * SDL_cos(-root_cam.b*SDL_PI_D/180);
        int128 cam_move_x = (int128) -cam_move_forward * cam_vel * SDL_sin(-root_cam.b*SDL_PI_D/180) * SDL_cos(root_cam.a*SDL_PI_D/180);

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
        int128 cam_move_y = (int128) cam_move_right * cam_vel * SDL_cosf(root_cam.a*SDL_PI_F/180);
        int128 cam_move_z = (int128) cam_move_right * cam_vel * 0;
        int128 cam_move_x = (int128) -cam_move_right * cam_vel * SDL_sinf(root_cam.a*SDL_PI_F/180);

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
        int128 cam_move_y = (int128) -cam_move_up * cam_vel * SDL_cosf(-root_cam.b*SDL_PI_F/180) * SDL_sinf(root_cam.a*SDL_PI_F/180);
        int128 cam_move_z = (int128) -cam_move_up * cam_vel * SDL_sinf(-root_cam.b*SDL_PI_F/180);
        int128 cam_move_x = (int128) -cam_move_up * cam_vel * SDL_cosf(-root_cam.b*SDL_PI_F/180) * SDL_cosf(root_cam.a*SDL_PI_F/180);

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
    SDL_GL_SetSwapInterval(vsync);

    // 3D rendering
    queue_render_data = true;
    while (queue_render_data == true && physics_running == true) {
        SDL_Delay(1);
    }
    queue_render_data = false; // should physics not be running/processing
    bool move_to_body_10 = key_down(SDL_SCANCODE_0);
    if (move_to_body_10) {
        root_cam.x = body[10].CM_prev.t.p.x + 0 * SCALE;
        root_cam.y = body[10].CM_prev.t.p.y + 0 * SCALE;
        root_cam.z = body[10].CM_prev.t.p.z + 100 * SCALE;
        /*root_cam.x = body[10].CM.t.p.x + 0 * SCALE;
         *   root_cam.y = body[10].CM.t.p.y + 0 * SCALE;
         *   root_cam.z = body[10].CM.t.p.z + 100 * SCALE;*/
        /*root_cam.x += body[3].CM.t.v.x * SCALE;
         *   root_cam.y += body[3].CM.t.v.y * SCALE;
         *   root_cam.z += body[3].CM.t.v.z * SCALE;*/
    }
    glViewport(0, 0, root_window_width, root_window_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render3D(root_window_width, root_window_height, root_cam);

    // 2D rendering (GUI overlay)
    if (!key_toggle(SDL_SCANCODE_G)) {
        onscreen_overlay(cam_speed, pan_sensitivity, last_fps, last_tps, root_window_width, root_window_height);
    }

    glFlush(); // note
    SDL_GL_SwapWindow(root_window);

    // resetting active control state
    mouse.moving = false;
    mouse.scrolling = false;

    int jk = 1;
    SDL_Log("Elapsed Sim Time : %.3lf", elapsed_simulated_time);
    SDL_Log("Mercury Position : X = %.0lf Y = %.0lf Z = %.0lf", (double) body[jk].CM.t.p.x / (SCALE*1e3), (double) body[jk].CM.t.p.y / (SCALE*1e3), (double) body[jk].CM.t.p.z / (SCALE*1e3));
    SDL_Log("Mercury Velocity : X = %.3lf Y = %.3lf Z = %.3lf", (double) body[jk].CM.t.v.x / (1e3), (double) body[jk].CM.t.v.y / (1e3), (double) body[jk].CM.t.v.z / (1e3));

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
