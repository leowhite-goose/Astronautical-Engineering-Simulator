#ifndef GRAVITY_C
#define GRAVITY_C

/*
struct vector3f gravitational_force_vector3f(struct object obj1, struct object obj2) { // force enacted upon obj2 by obj1 (see link below)
    struct vector3f vect0;
    struct vector3f displacement_vect = displacement_vector3f(obj1.d.pos.vec3, obj2.d.pos.vec3);
    float distance = vector_magnitude3f(displacement_vect);
    vect0.x = -G_fp32 * obj1.p.m * obj2.p.m * displacement_vect.x / SDL_pow(distance, 3); // https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form
    vect0.y = -G_fp32 * obj1.p.m * obj2.p.m * displacement_vect.y / SDL_pow(distance, 3);
    vect0.z = -G_fp32 * obj1.p.m * obj2.p.m * displacement_vect.z / SDL_pow(distance, 3);
    return vect0;
}

struct object apply_gravity3f(struct object objN) {
    for (int i = 1; i <= obj_count; i++) { // sizeof(obj) / sizeof(obj[0] = obj_count
        bool is_same_object = (obj[i].d.pos.vec3.x == objN.d.pos.vec3.x) && (obj[i].d.pos.vec3.y == objN.d.pos.vec3.y) && (obj[i].d.pos.vec3.z == objN.d.pos.vec3.z);
        bool massless = (obj[i].p.m <= 1e3); // neglible mass
        if (!is_same_object && !massless) {
            struct vector3f grav_force_vect = gravitational_force_vector3f(obj[i], objN);
            objN = apply_force3f(grav_force_vect, objN);
        }
    }
    return objN;
}*/

#endif
