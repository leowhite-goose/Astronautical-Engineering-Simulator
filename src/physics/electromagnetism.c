vec3f Lorentz_force3f(vec3f electric_field, vec3f magnetic_field, vec3f velocity, float charge) {
    return vector_scale3f(vector_addition3f(electric_field, cross_product3f(velocity, magnetic_field)), charge);
}
