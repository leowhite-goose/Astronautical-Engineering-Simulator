#ifndef EVENTS_C
#define EVENTS_C

bool key_down(SDL_Scancode key_scancode) {
    return keyboard_scancode_down_state[key_scancode];
}

bool key_toggle(SDL_Scancode key_scancode) {
    return keyboard_scancode_toggled_state[key_scancode];
}

/*bool activate_control (static bool control, bool input) {
    control = input;
}

bool flip_control (static bool control, bool input) {
    if (control == true && input == true) {

    }

}*/

void last_key_down(char* key_name, int8 buffersize) { // https://stackoverflow.com/questions/1496313/returning-a-c-string-from-a-function

    if (!key_name || buffersize < 1) {
        //return // junk inputs
    } else {
        const char* key = SDL_malloc(sizeof(char) * buffersize);
        bool a_key_is_down = false;
        for (SDL_Scancode i = 0; i < SDL_SCANCODE_COUNT; i++) { // higher scancode keys override lower ones (e.g., B(5) overrides A(4) when both inputted)
            if (keyboard_scancode_down_state[i] == true) {
                key = SDL_GetScancodeName(i);
                a_key_is_down = true;
            }
        }
        if (!a_key_is_down) {
            key = "No keys down";
        }
        SDL_strlcpy(key_name, key, buffersize-1);
    }
    key_name[buffersize-1] = '\0';
}

SDL_AppResult AES_events(SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT: // https://wiki.libsdl.org/SDL3/SDL_EventType
            return SDL_APP_SUCCESS;
            break;
        case SDL_EVENT_MOUSE_MOTION:
            mouse.moving = true;
            mouse.x = event->motion.x;
            mouse.y = event->motion.y;
            mouse.x_rel = event->motion.xrel;
            mouse.y_rel = event->motion.yrel;
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            mouse.scrolling = true;
            mouse.wheel.x = event->wheel.x;
            mouse.wheel.y = event->wheel.y;
            //mouse.wheel.x_accu = event->wheel.integer_x;
            //mouse.wheel.y_accu = event->wheel.integer_y;
            //mouse.wheel.down = event->button.down;
            //mouse.wheel.toggle = !mouse.wheel.toggle;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            switch (event->button.button) {
                case SDL_BUTTON_LEFT:
                    mouse.left.x = event->button.x;
                    mouse.left.y = event->button.y;
                    mouse.left.down = event->button.down;
                    mouse.left.toggle = !mouse.right.toggle;
                    break;
                case SDL_BUTTON_RIGHT:
                    mouse.right.x = event->button.x;
                    mouse.right.y = event->button.y;
                    mouse.right.down = event->button.down;
                    mouse.right.toggle = !mouse.right.toggle;
                    break;
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            switch (event->button.button) {
                case SDL_BUTTON_LEFT:
                    mouse.left.x = event->button.x;
                    mouse.left.y = event->button.y;
                    mouse.left.down = event->button.down;
                    break;
                case SDL_BUTTON_RIGHT:
                    mouse.right.x = event->button.x;
                    mouse.right.y = event->button.y;
                    mouse.right.down = event->button.down;
                    break;
            }
            break;
        case SDL_EVENT_KEY_DOWN:
            SDL_Scancode key_down = event->key.scancode;
            keyboard_scancode_toggled_state[key_down] = !keyboard_scancode_toggled_state[key_down];
            keyboard_scancode_down_state = SDL_GetKeyboardState(&key_count);
            keymod_state = SDL_GetModState();
            break;
        case SDL_EVENT_KEY_UP:
            keyboard_scancode_down_state = SDL_GetKeyboardState(&key_count);
            keymod_state = SDL_GetModState();
            break;
        case SDL_EVENT_TEXT_INPUT:
            break;
        case SDL_EVENT_FINGER_DOWN: // https://wiki.libsdl.org/SDL3/README-touch
            touch.finger.down = true;
            touch.finger.x = event->tfinger.x;
            touch.finger.y = event->tfinger.y;
            touch.finger.p = event->tfinger.pressure;
            //SDL_Log("x%.3f y%.3f p%.3f", touch.finger.x, touch.finger.y, touch.finger.p);
            break;
        case SDL_EVENT_FINGER_UP:
            touch.finger.down = false;
            touch.finger.dx = 0;
            touch.finger.dy = 0;
            break;
        case SDL_EVENT_FINGER_MOTION:
            touch.finger.dx += event->tfinger.dx;
            touch.finger.dy += event->tfinger.dy;
            //SDL_Log("x%.3f y%.3f", touch.finger.dx, touch.finger.dy);
            break;
    }
    return SDL_APP_CONTINUE;  // carry on with the program!
}

#endif
