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

void gl_render_root_gui(float window_width, float window_height) { // https://stackoverflow.com/questions/28880562/rendering-text-with-sdl2-and-opengl
    glBindTexture(GL_TEXTURE_2D, root_gui_gl_texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Use blurry texture mapping (replace GL_LINEAR with GL_NEAREST for blocky)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, root_gui_surface->w, root_gui_surface->h, 0,  GL_RGBA, GL_UNSIGNED_BYTE, root_gui_surface->pixels); // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml

    glColor4f(1.0, 1.0, 1.0, 1.0); //Don't use special coloring

    if (window_width >= window_height) {

    } else {

    }
    float scale = 0.25;
    float x_offset = -0.75;
    float y_offset = 0.;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-scale + x_offset, -scale + y_offset);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(scale + x_offset, -scale + y_offset);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(scale + x_offset, scale + y_offset);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-scale + x_offset, scale + y_offset);

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

void onscreen_overlay(int cam_speed, int pan_sensitivity, int last_fps, int last_tps) {
    //SDL_SetRenderTarget(root_gui_renderer, root_gui_texture);

    SDL_SetRenderDrawColor(root_gui_renderer, 255, 255, 255, 31);
    SDL_RenderClear(root_gui_renderer);
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
    int128 big_number = 1.7e38;
    //SDL_RenderDebugTextFormat(root_gui_renderer, 10, 138, "%.15lf",  (double) big_number / 1e15);

    //SDL_SetRenderTarget(root_gui_renderer, NULL);
    //gl_render_root_gui_texture(root_gui_texture);
    //SDL_RenderTexture(root_renderer, root_gui_texture, NULL, NULL);
    SDL_RenderPresent(root_gui_renderer); // put it all on the screen!
}

#endif
