#ifndef MAINLOOP_C
#define MAINLOOP_C

SDL_AppResult AES_mainloop() {
    // updating global variables;
    SDL_GetWindowSizeInPixels(root_window, &root_window_width, &root_window_height);
    if ((SDL_max(root_window_width, root_window_height) > gui_texture_res) || (SDL_max(root_window_width, root_window_height) * 2 < gui_texture_res)) {
        gui_texture_res = power_of_two(SDL_max(root_window_width, root_window_height));
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
    static int cam_speed = 5;
    static int pan_sensitivity = -2;
    if (mouse.scrolling && key_down(SDL_SCANCODE_LSHIFT)) {cam_speed += mouse.wheel.y;}
    if (mouse.scrolling && key_down(SDL_SCANCODE_LCTRL)) {pan_sensitivity += mouse.wheel.y;}
    float cam_vel = SDL_pow(2,cam_speed)/60;
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
    static uint64 last_fps;
    onscreen_overlay(cam_speed, pan_sensitivity, last_fps, last_tps, root_window_width, root_window_height);

    // 3D rendering
    //SDL_GL_MakeCurrent(root_window, root_gl_context);   // restores gl_context
    SDL_GL_SetSwapInterval(vsync);
    render3D(root_window_width, root_window_height, root_cam);
    //SDL_FlushRenderer(root_renderer);                   // rids of gl_context

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
