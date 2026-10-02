#ifndef PHYSICS_THREAD
#define PHYSICS_THREAD

void AES_precise_delay_ns(int64 delay) {
    bool waiting = true;
    int64 start_time = SDL_GetTicksNS();
    int64 end_time = start_time + delay;
    while (waiting) { // NEVER releases thread back to OS while running (unlike SDL's delay functions)
        SDL_DelayNS(0);
        //SDL_DelayPrecise(0);
        int64 current_time = SDL_GetTicksNS();
        if (current_time + shortest_delay_ns * 1/3 >= end_time) { // fix
            waiting = false;
        }
    }
}

static int SDLCALL physics_loop(void *data) {
    uint64 target_tick_time_ns = SDL_NS_PER_MS * 1000 * physics_dt; // 1e6 ns = 1ms
    uint64 accu = 0;
    uint64 last_tick_update_ns = 0;
    uint64 last_tick_end_ns = 0;
    uint64 tick_end_ns = 0;
    uint64 elapsed_ns = 0;

    bool current_buffer;
    while (true) { // while physics/gameticks = needed
        if (!key_toggle(SDL_SCANCODE_P)) {
        if (SDL_GetTicksNS() >= SDL_NS_PER_SECOND * 3) { // wait 12 seconds before physics-ing
            physics_running = true;
            if (key_toggle(SDL_SCANCODE_Z)) {
                timewarp = 1000;
                dt = 1;
            } else {
                timewarp = 1;
                dt = 0.001;
            }
            //blue_overlap = triangle_triangle_collisionf(p1a, p1b, p1c, p2a, p2b, p2c); collision testing

            for (int i = 0; i < 11; i++) {
                for (int j = 0; j < 10; j++) {
                    if (i != j) {
                        vec3i128 pos_displacement = {body[j].CM.t.p.x - body[i].CM.t.p.x, body[j].CM.t.p.y - body[i].CM.t.p.y, body[j].CM.t.p.z - body[i].CM.t.p.z};
                        vec3f body_i_pos = {0, 0, 0};
                        vec3f body_j_pos = {(float) -pos_displacement.x / SCALE, (float) -pos_displacement.y / SCALE, (float) -pos_displacement.z / SCALE};
                        vec3f body_i_vel = {body[i].CM.t.v.x, body[i].CM.t.v.y, body[i].CM.t.v.z};
                        vec3f body_j_vel = {body[j].CM.t.v.x, body[j].CM.t.v.y, body[j].CM.t.v.z};
                        float body_i_m = body[i].CM.m / 1e3;
                        float body_j_m = body[j].CM.m / 1e3;
                        vec3d grav_joni = gravitational_force3d(body_j_pos, body_j_vel, body_j_m, body_i_pos, body_i_vel, body_i_m); // j acts upon object i;
                        body[i].CM.t.a.x += (float) grav_joni.x / body_i_m;
                        body[i].CM.t.a.y += (float) grav_joni.y / body_i_m;
                        body[i].CM.t.a.z += (float) grav_joni.z / body_i_m;
                    }
                }
            }
            for (int i = 0; i < 12; i++) {
                body[i].CM.t.v.x += body[i].CM.t.a.x * dt;
                body[i].CM.t.v.y += body[i].CM.t.a.y * dt;
                body[i].CM.t.v.z += body[i].CM.t.a.z * dt;
                body[i].CM.t.p.x += body[i].CM.t.v.x * dt * SCALE;
                body[i].CM.t.p.y += body[i].CM.t.v.y * dt * SCALE;
                body[i].CM.t.p.z += body[i].CM.t.v.z * dt * SCALE;
                vec3i128 translate = {body[i].CM.t.v.x * dt * SCALE, body[i].CM.t.v.y * dt * SCALE, body[i].CM.t.v.z * dt * SCALE};
                //hold_rendering = true; mod_body = i;
                vert128_translate(&body[i].geo.vert_cnt, &body[i].geo.vert128_buffer, translate);
                //vert128_translate(&body[i].geo.vert_cnt, &body[i].geo.vert128, translate);
                //hold_rendering = false; mod_body = -1;
                body[i].CM.t.a.x = 0;
                body[i].CM.t.a.y = 0;
                body[i].CM.t.a.z = 0;
            }

            bool track_body_10 = !key_toggle(SDL_SCANCODE_MINUS);
            if (track_body_10) {
                root_cam_physics.x += body[10].CM.t.v.x * SCALE * dt;
                root_cam_physics.y += body[10].CM.t.v.y * SCALE * dt;
                root_cam_physics.z += body[10].CM.t.v.z * SCALE * dt;
            }
            static vec3f bearing = {(270*SDL_PI_F/180),(90*SDL_PI_F/180),0};
            static vec3f bearing_prev = {(270*SDL_PI_F/180),(90*SDL_PI_F/180),0};
            int2 L_R = key_down(SDL_SCANCODE_RIGHT) - key_down(SDL_SCANCODE_LEFT);
            int2 U_D = key_down(SDL_SCANCODE_UP) - key_down(SDL_SCANCODE_DOWN);
            if (L_R != 0) {U_D = 0;}
            bearing.y += (float) (U_D) * 0.05 * (SDL_PI_F/180);
            bearing.x += (float) (L_R) * 0.05 * (SDL_PI_F/180);
            vec3f bearing_dt = {bearing.x - bearing_prev.x, bearing.y - bearing_prev.y, bearing.z - bearing_prev.z};
            bearing_prev = bearing;
            vec3f rotation_axis = {-1*bearing_dt.y*(SDL_powf(SDL_sinf(bearing.x),1)), -1*bearing_dt.y*(SDL_powf(SDL_cosf(bearing.x),1)), 1*bearing_dt.x}; // (270,90)
            float angle = 0.05 * (SDL_PI_F/180);
            int2 move_forward = key_down(SDL_SCANCODE_BACKSPACE) - key_down(SDL_SCANCODE_SPACE);
            /*vec3f unit_vect = {SDL_cosf(bearing.x - SDL_PI_F) * SDL_sinf(bearing.y), SDL_sinf(bearing.x - SDL_PI_F) * SDL_sinf(bearing.y), SDL_cosf(bearing.y)};
            unit_vect = unit_vector3f(unit_vect);*/
            vec3f unit_vect = {SDL_cosf(bearing.x) * SDL_powf(SDL_sinf(bearing.y),1), SDL_sinf(bearing.x - SDL_PI_F) * SDL_powf(SDL_sinf(bearing.y),1), SDL_powf(SDL_cosf(bearing.y),1)};
            unit_vect = unit_vector3f(unit_vect);
            body[10].CM.t.a.x = (float) 9.81 * move_forward * unit_vect.x;
            body[10].CM.t.a.y = (float) 9.81 * move_forward * unit_vect.y;
            body[10].CM.t.a.z = (float) 9.81 * move_forward * unit_vect.z;
            delta_v_spent += 9.81 * dt * SDL_abs(move_forward);
            if ((rotation_axis.x != 0) || (rotation_axis.y != 0) || (rotation_axis.z != 0)) {
                mod_body = 10;
                rotate_128i(body[10].geo.vert_cnt, &body[10].geo.vert128_buffer, body[10].CM.t.p, rotation_axis, angle);
                mod_body = -1;
            }

            if (queue_render_data == true) {
                for (int i = 0; i < 12; i++) {
                    copy_128mesh(body[i].geo.vert_cnt, &body[i].geo.vert128_buffer, &body[i].geo.vert128);
                    body[i].CM_prev.t.v.x = body[i].CM.t.v.x;
                    body[i].CM_prev.t.v.y = body[i].CM.t.v.y;
                    body[i].CM_prev.t.v.z = body[i].CM.t.v.z;
                    body[i].CM_prev.t.p.x = body[i].CM.t.p.x;
                    body[i].CM_prev.t.p.y = body[i].CM.t.p.y;
                    body[i].CM_prev.t.p.z = body[i].CM.t.p.z;
                }
                root_cam.x += root_cam_physics.x;
                root_cam.y += root_cam_physics.y;
                root_cam.z += root_cam_physics.z;
                root_cam_physics.x = 0;
                root_cam_physics.y = 0;
                root_cam_physics.z = 0;
                queue_render_data = false;
            }

            elapsed_simulated_time += dt;
            ns_since_J2000 += dt * 1e9;
        }

        tick_end_ns = SDL_GetTicksNS(); // this is called at end of each tick
        elapsed_ns = tick_end_ns - last_tick_end_ns;
        if (tick_end_ns - last_tick_update_ns >= SDL_NS_PER_SECOND) { // for updating TPS polling counter (each second)
            last_tick_update_ns = tick_end_ns;
            last_tps = accu;
            accu = 0;
        }
        accu += 1;
        if (elapsed_ns < target_tick_time_ns) {
            //AES_precise_delay_ns(target_tick_time_ns - elapsed_ns);
            if (target_tick_time_ns - elapsed_ns > SDL_NS_PER_MS*0.10) { // if time to wait is at least 0.1ms
                SDL_DelayNS(target_tick_time_ns - elapsed_ns - SDL_NS_PER_MS*0.10); // try to regain thread 0.1ms before needed
            } else { // if less than 0.1ms, buzy wait
                while (elapsed_ns < target_tick_time_ns) {
                    tick_end_ns = SDL_GetTicksNS();
                    elapsed_ns = tick_end_ns - last_tick_end_ns;
                }
            }
        }
        last_tick_end_ns = SDL_GetTicksNS();
    } else {physics_running = false;}}
}

#endif
