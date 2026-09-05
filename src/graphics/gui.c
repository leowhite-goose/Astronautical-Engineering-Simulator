#ifndef GUI_C
#define GUI_C

SDL_HitTestResult HitTestCallback(SDL_Window *window, const SDL_Point *area, void *data) // https://gist.github.com/dele256/901dd1e8f920327fc457a538996f2a29
{
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    if(area->y < MOUSE_GRAB_PADDING)
    {
        if(area->x < MOUSE_GRAB_PADDING)
        {
            return SDL_HITTEST_RESIZE_TOPLEFT;
        }
        else if(area->x > width - MOUSE_GRAB_PADDING)
        {
            return SDL_HITTEST_RESIZE_TOPRIGHT;
        }
        else
        {
            return SDL_HITTEST_RESIZE_TOP;
        }
    }
    else if(area->y > height - MOUSE_GRAB_PADDING)
    {
        if(area->x < MOUSE_GRAB_PADDING)
        {
            return SDL_HITTEST_RESIZE_BOTTOMLEFT;
        }
        else if(area->x > width - MOUSE_GRAB_PADDING)
        {
            return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
        }
        else
        {
            return SDL_HITTEST_RESIZE_BOTTOM;
        }
    }
    else if(area->x < MOUSE_GRAB_PADDING)
    {
        return SDL_HITTEST_RESIZE_LEFT;
    }
    else if(area->x > width - MOUSE_GRAB_PADDING)
    {
        return SDL_HITTEST_RESIZE_RIGHT;
    }
    else if(area->y < MOUSE_GRAB_PADDING * 3)
    {
        return SDL_HITTEST_DRAGGABLE;
    }

    return SDL_HITTEST_NORMAL; // SDL_HITTEST_DRAGGABLE
}

Uint32 power_of_two(uint32 input) {
    float exponent = SDL_logf(input) / SDL_logf(2); // https://stackoverflow.com/questions/11054740/logarithm-function-of-an-arbitrary-integer-base-in-c
    Uint32 output = SDL_powf(2, SDL_ceil(exponent));
    return output;
}

void gl_render_root_gui(float window_width, float window_height) { // https://stackoverflow.com/questions/28880562/rendering-text-with-sdl2-and-opengl
    glBindTexture(GL_TEXTURE_2D, root_gui_gl_texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Use blurry texture mapping (replace GL_LINEAR with GL_NEAREST for blocky)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, root_gui_surface->w, root_gui_surface->h, 0,  GL_RGBA, GL_UNSIGNED_BYTE, root_gui_surface->pixels); // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml

    glColor4f(1.0, 1.0, 1.0, 1.0); //Don't use special coloring

    float x_scale = gui_texture_res/window_width;
    float y_scale = gui_texture_res/window_height;
    float x_offset = gui_texture_res/window_width - 1;
    float y_offset = 1 - gui_texture_res/window_height;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-x_scale + x_offset, -y_scale + y_offset);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(x_scale + x_offset, -y_scale + y_offset);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(x_scale + x_offset, y_scale + y_offset);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-x_scale + x_offset, y_scale + y_offset);

    glEnd();

    /*float quad_vertices[8] = {
        -scale + x_offset, -scale + y_offset,
        scale + x_offset, -scale + y_offset,
        scale + x_offset, scale + y_offset,
        -scale + x_offset, scale + y_offset
    };
    float tex_coords[8] = {
        0, 1,
        1, 1,
        1, 0,
        0, 0
    };
    glTexCoordPointer(2, GL_FLOAT, 0, &tex_coords);
    glVertexPointer(2, GL_FLOAT, 0, &quad_vertices);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_QUADS, 0, 1);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);*/
}

bool coords_in_rectf(float x, float y, float rect[4]) { // rect[4] = {x,y,w,h};
    if ((rect[0] <= x) && (x <= rect[0] + rect[2]) && (rect[1] <= y) && (y <= rect[1] + rect[3])) {
        return true;
    } else {
        return false;
    }
}

void onscreen_overlay(int cam_speed, int pan_sensitivity, int last_fps, int last_tps, uint16 window_width, uint16 window_height) {
    if (touch_analog[2] <= 0) {
        touch_analog[2] = 1;
    }
    if (touch_button[8] == true) {
        touch_analog[2] = 1;
    }
    if (touch_button[9] == true) {
        touch_analog[2] = 2;
    }
    if (touch_button[10] == true) {
        touch_analog[2] = 3;
    }
    float gui_render_scale = touch_analog[2];
    SDL_SetRenderScale(root_gui_renderer, gui_render_scale, gui_render_scale);
    uint16 actual_width = window_width;
    uint16 actual_height = window_width;
    window_width = window_width / touch_analog[2];
    window_height = window_height / touch_analog[2];

    SDL_SetRenderDrawColor(root_gui_renderer, 255, 255, 255, 0);
    SDL_RenderClear(root_gui_renderer);
    SDL_SetRenderDrawColor(root_gui_renderer, 255, 255, 255, 31);
    float finger_x = window_width * (touch.finger.x + touch.finger.dx);
    float finger_y = window_height * (touch.finger.y + touch.finger.dy);
    //SDL_Log("x: %.3f y: %.3f", finger_x, finger_y);
    SDL_FRect rects_f[] = {
        {0, 0, 128, 150},
        {window_width - 128 - 32, window_height - 128 - 32, 128, 128}, // touchpad
        {80, window_height - 80 - 32, 32, 32}, // middle
        {32, window_height - 80 - 32, 32, 32}, // left
        {80, window_height - 128 - 32, 32, 32}, // top
        {128, window_height - 80 - 32, 32, 32}, // right
        {80, window_height - 32 - 32, 32, 32}, // bottom
        {176, window_height - 112 - 32, 32, 32}, // up
        {176, window_height - 48 - 32, 32, 32} // down
    };
    float rect0[] = {80, window_height - 80 - 32, 32, 32}; // middle
    touch_button[0] = (touch.finger.down && coords_in_rectf(finger_x, finger_y, rect0));
    float rect1[] = {32, window_height - 80 - 32, 32, 32}; // left
    touch_button[1] = (touch.finger.down && coords_in_rectf(finger_x, finger_y, rect1));
    float rect2[] = {80, window_height - 128 - 32, 32, 32}; // top
    touch_button[2] = (touch.finger.down && coords_in_rectf(finger_x, finger_y, rect2));
    float rect3[] = {128, window_height - 80 - 32, 32, 32}; // right
    touch_button[3] = (touch.finger.down && coords_in_rectf(finger_x, finger_y, rect3));
    float rect4[] = {80, window_height - 32 - 32, 32, 32}; // bottom
    touch_button[4] = (touch.finger.down && coords_in_rectf(finger_x, finger_y, rect4));
    float rect5[] = {176, window_height - 112 - 32, 32, 32}; // up
    touch_button[5] = (touch.finger.down && coords_in_rectf(finger_x, finger_y, rect5));
    float rect6[] = {176, window_height - 48 - 32, 32, 32}; // down
    touch_button[6] = (touch.finger.down && coords_in_rectf(finger_x, finger_y, rect6));
    float rect7[] = {window_width - 128 - 32, window_height - 128 - 32, 128, 128}; // touchpad
    touch_button[7] = (touch.finger.down && coords_in_rectf(finger_x, finger_y, rect7));
    if (touch_button[7]) {
        touch_analog[0] = (finger_x - window_width + 96)/64;
        touch_analog[1] = (finger_y - window_height + 96)/64;
    } else {
        touch_analog[0] = 0;
        touch_analog[1] = 0;
    }
    //SDL_Log("x%.3f y%.3f", touch_analog[0], touch_analog[1]);
    SDL_RenderFillRects(root_gui_renderer, rects_f, sizeof(rects_f)/sizeof(rects_f[0]));
    for (int i = 0; i < 5; i++) {
        SDL_FRect rect9 = {window_width - 32 - 64 * i - 32, 32, 32, 32};
        float rect9t[] = {rect9.x, rect9.y, rect9.w, rect9.h};
        touch_button[8+i] = (touch.finger.down && coords_in_rectf(finger_x, finger_y, rect9t));
        SDL_RenderFillRect(root_gui_renderer, &rect9);
    }
    SDL_SetRenderDrawColor(root_gui_renderer, 255, 255, 255, 63);
    SDL_FRect rect8 = {window_width - 128 - 48 + 64*(touch_analog[0]+1), window_height - 128 - 48 + 64*(touch_analog[1]+1), 32, 32};
    SDL_RenderFillRect(root_gui_renderer, &rect8);
    SDL_SetRenderDrawColor(root_gui_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); // sets draw color to white, full alpha
    SDL_RenderDebugTextFormat(root_gui_renderer, 10, 10, "Cam Vel:%" SDL_PRIs32, cam_speed);
    SDL_RenderDebugTextFormat(root_gui_renderer, 10, 26, "Pan Vel:%" SDL_PRIs32, pan_sensitivity);
    SDL_RenderDebugTextFormat(root_gui_renderer, 10, 42, "%" SDL_PRIs32 "x% " SDL_PRIs32 "y% " SDL_PRIs32 "z", (int) root_cam.x, (int) root_cam.y, (int) root_cam.z);
    SDL_RenderDebugTextFormat(root_gui_renderer, 10, 58, "%" SDL_PRIs32 "x% " SDL_PRIs32 "y% " SDL_PRIs32 "z", (int) root_cam.a, (int) root_cam.b, (int) root_cam.c);
    SDL_RenderDebugTextFormat(root_gui_renderer, 10, 74, "%" SDL_PRIs32, (int) global_fp);

    p2c.x = root_cam.x+1;
    p2c.y = root_cam.y+1;
    p2c.z = root_cam.z+1;
    SDL_RenderDebugTextFormat(root_gui_renderer, 10, 90, "Collision?:%" SDL_PRIs32, (int) blue_overlap);

    SDL_RenderDebugTextFormat(root_gui_renderer, 10, 106, "%" SDL_PRIs32, (int) last_fps);
    SDL_RenderDebugTextFormat(root_gui_renderer, 10, 122, "%" SDL_PRIs32, (int) last_tps);

    SDL_RenderPresent(root_gui_renderer); // put it all on the screen!

    window_width = actual_width;
    window_width = actual_height;
}

#endif
