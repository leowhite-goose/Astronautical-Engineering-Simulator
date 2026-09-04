#ifndef MECHANICS_C
#define MECHANICS_C

/*
 * struct object apply_force3f(struct vector3f force_vect, struct object obj) {
 *    obj.d.acc.vec3.x += force_vect.x / obj.p.m;
 *    obj.d.acc.vec3.y += force_vect.y / obj.p.m;
 *    obj.d.acc.vec3.z += force_vect.z / obj.p.m;
 *    return obj;
 * }
 *
 * struct object update_object(struct object obj, float dt) { // do after all "apply_force" operations for a given object
 *    obj.d.vel.vec3.x += obj.d.acc.vec3.x * dt; // see youtube video : v=nCg3aXn5F3M for choice to preserve phase-space volume over pos->vel order
 *    obj.d.pos.vec3.x += obj.d.vel.vec3.x * dt;
 *
 *    obj.d.vel.vec3.y += obj.d.acc.vec3.y * dt;
 *    obj.d.pos.vec3.y += obj.d.vel.vec3.y * dt;
 *
 *    obj.d.vel.vec3.z += obj.d.acc.vec3.z * dt;
 *    obj.d.pos.vec3.z += obj.d.vel.vec3.z * dt;
 *
 *    obj.d.acc.vec3.x = 0;
 *    obj.d.acc.vec3.y = 0;
 *    obj.d.acc.vec3.z = 0;
 *
 *    return obj;
 * }*/

#endif
