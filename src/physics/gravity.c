#ifndef GRAVITY_C
#define GRAVITY_C

struct vec3f gravitational_force3f(vec3f pos1_3f, vec3f vel1_3f, float mass1_f, vec3f pos2_3f, vec3f vel2_3f, float mass2_f) { // force enacted upon obj2 by obj1; Swartzschild
    vec3f gravitational_force_vector_3f;

    vec3f dist_vec_3f = displacement_vector3f(pos1_3f, pos2_3f);
    float dist_mag_f = vector_magnitude3f(dist_vec_3f);

    // Newtonian contribution (translative)
    gravitational_force_vector_3f.x = -G_f * mass1_f * mass2_f * dist_vec_3f.x / SDL_powf(dist_mag_f, 3); // https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form
    gravitational_force_vector_3f.y = -G_f * mass1_f * mass2_f * dist_vec_3f.y / SDL_powf(dist_mag_f, 3);
    gravitational_force_vector_3f.z = -G_f * mass1_f * mass2_f * dist_vec_3f.z / SDL_powf(dist_mag_f, 3);

    float effective_mass_f = (mass1_f + mass2_f) / (mass1_f * mass2_f); // see Wikipedia
    vec3f angular_momentum_3f = {
    effective_mass_f * dist_vec_3f.x * ( ((mass1_f * vel1_3f.x + mass2_f * vel2_3f.x)/(mass1_f + mass2_f)) / (pos1_3f.x * mass1_f + pos2_3f.x * mass2_f) ),
    effective_mass_f * dist_vec_3f.y * ( ((mass1_f * vel1_3f.y + mass2_f * vel2_3f.y)/(mass1_f + mass2_f)) / (pos1_3f.y * mass1_f + pos2_3f.y * mass2_f) ),
    effective_mass_f * dist_vec_3f.z * ( ((mass1_f * vel1_3f.z + mass2_f * vel2_3f.z)/(mass1_f + mass2_f)) / (pos1_3f.z * mass1_f + pos2_3f.z * mass2_f) )};

    // Newtonian contribution (rotational)
    gravitational_force_vector_3f.x -= (SDL_powf(angular_momentum_3f.x, 2) * dist_vec_3f.x) / (effective_mass_f * SDL_powf(dist_mag_f, 4));
    gravitational_force_vector_3f.y -= (SDL_powf(angular_momentum_3f.y, 2) * dist_vec_3f.y) / (effective_mass_f * SDL_powf(dist_mag_f, 4));
    gravitational_force_vector_3f.z -= (SDL_powf(angular_momentum_3f.z, 2) * dist_vec_3f.z) / (effective_mass_f * SDL_powf(dist_mag_f, 4));

    // Swartzchild contribution
    gravitational_force_vector_3f.x += (3 * G_f * (mass1_f + mass2_f) * SDL_powf(angular_momentum_3f.x, 2)) / (c_sq_ui64 * effective_mass_f * SDL_powf(dist_mag_f, 5));
    gravitational_force_vector_3f.y += (3 * G_f * (mass1_f + mass2_f) * SDL_powf(angular_momentum_3f.y, 2)) / (c_sq_ui64 * effective_mass_f * SDL_powf(dist_mag_f, 5));
    gravitational_force_vector_3f.z += (3 * G_f * (mass1_f + mass2_f) * SDL_powf(angular_momentum_3f.z, 2)) / (c_sq_ui64 * effective_mass_f * SDL_powf(dist_mag_f, 5));

    return gravitational_force_vector_3f;
}

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
