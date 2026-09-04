#ifndef RELATIVITY_C
#define RELATIVITY_C

/*
double lorentz_factor_fp64(double velocity_fp64_vector[3]) { // https://en.wikipedia.org/wiki/Lorentz_factor
    double velocity_fp64_magnitude = SDL_sqrt(SDL_pow(velocity_fp64_vector[0], 2) + SDL_pow(velocity_fp64_vector[1], 2) + SDL_pow(velocity_fp64_vector[2], 2));
    if (velocity_fp64_magnitude < (_BitInt(32)) c_int32*0.014) { // sizeof() = 8 bytes (issue)
        return 1; // max <0.01% error
    } else if (velocity_fp64_magnitude < (_BitInt(32)) c_int32*0.22) {
        double beta_squared_fp64 = SDL_pow(velocity_fp64_magnitude/c_int32, 2); // = (v^2 / c^2) = (v/c)^2 fraction
        return 1 + 0.5 * beta_squared_fp64; // max <0.1% error
    } else {
        double beta_squared_fp64 = SDL_pow(velocity_fp64_magnitude/c_int32, 2); // = (v^2 / c^2) = (v/c)^2 fraction
        return 1 / SDL_sqrt(1-(beta_squared_fp64)); // theoretically exact
    }
}

double relativistic_mass_fp64(double rest_mass_fp64, double velocity_fp64_vector[3]) { // https://en.wikipedia.org/wiki/Mass_in_special_relativity
    return rest_mass_fp64 * lorentz_factor_fp64(velocity_fp64_vector);
}*/

#endif
