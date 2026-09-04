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

    while (true) { // while physics/gameticks = needed
        blue_overlap = triangle_triangle_collisionf(p1a, p1b, p1c, p2a, p2b, p2c);

        tick_end_ns = SDL_GetTicksNS(); // this is called at end of each tick
        elapsed_ns = tick_end_ns - last_tick_end_ns;
        if (tick_end_ns - last_tick_update_ns >= SDL_NS_PER_SECOND) { // for updating TPS polling counter (each second)
            last_tick_update_ns = tick_end_ns;
            last_tps = accu;
            accu = 0;
        }
        accu += 1;
        if (elapsed_ns < target_tick_time_ns) {
            AES_precise_delay_ns(target_tick_time_ns - elapsed_ns);
        }
        last_tick_end_ns = SDL_GetTicksNS();
    }
}

#endif
