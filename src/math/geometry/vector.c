#ifndef VECTOR_C
#define VECTOR_C

float vector_magnitude3f(vec3f vect1) {
    float magnitude = SDL_sqrtf(SDL_powf(vect1.x,2)+SDL_powf(vect1.y,2)+SDL_powf(vect1.z,2));
    return magnitude; // i.e., distance
}

vec3f displacement_vector3f(vec3f vect1, vec3f vect2) {
    vec3f vect0;
    vect0.x = vect2.x - vect1.x;
    vect0.y = vect2.y - vect1.y;
    vect0.z = vect2.z - vect1.z;
    return vect0;
}

vec3f unit_vector3f(vec3f vect1) {
    vec3f vect0;
    float magnitude1 = vector_magnitude3f(vect1);
    vect0.x = vect1.x / magnitude1;
    vect0.y = vect1.y / magnitude1;
    vect0.z = vect1.z / magnitude1;
    return vect0;
}

vec3f cross_product3f(vec3f A, vec3f B) {
    vec3f vect0 = {(A.y)*(B.z) - (A.z)*(B.y), (A.z)*(B.x) - (A.x)*(B.z), (A.x)*(B.y) - (A.y)*(B.x)};
    return vect0;
}

vec3f vector_addition3f(vec3f vect1, vec3f vect2) {
    vec3f vect0;
    vect0.x = vect1.x + vect2.x;
    vect0.y = vect1.y + vect2.y;
    vect0.z = vect1.z + vect2.z;
    return vect0;
}

vec3f vector_subtract3f(vec3f vect1, vec3f vect2) {
    vec3f vect0;
    vect0.x = vect1.x - vect2.x;
    vect0.y = vect1.y - vect2.y;
    vect0.z = vect1.z - vect2.z;
    return vect0;
}

vec3f vector_scale3f(vec3f vect1, float scale) {
    vec3f vect0;
    vect1.x = vect1.x * scale;
    vect1.y = vect1.y * scale;
    vect1.z = vect1.z * scale;
    return vect0;
}

vec3f triangle_normal3f(vec3f A, vec3f B, vec3f C) {
    vec3f vect1 = vector_subtract3f(B, A);
    vec3f vect2 = vector_subtract3f(C, B);
    vec3f normal = cross_product3f(vect1, vect2); // only needs 2 out of 3 vectors/vertices
    return normal;
}

vec3f triangle_center3f(vec3f A, vec3f B, vec3f C) {
    vec3f center_point;
    //center_point = vector_scale3f(vector_addition3f(vector_addition3f(A, point2), point3), 1/3);
    center_point = (vec3f) {(A.x + B.x + C.x) / 3, (A.y + B.y + C.y) / 3, (A.z + B.z + C.z) / 3};
    return center_point;
}

vec4f plane_from_point_normal4f(vec3f point, vec3f normal) { /* Ax + By + Cz + D = 0 */
    vec4f plane;
    plane.x = normal.x;                                                         /* A */
    plane.y = normal.y;                                                         /* B */
    plane.z = normal.z;                                                         /* C */
    plane.w = -normal.x * point.x - normal.y * point.y - normal.z * point.z;    /* D */
    return plane;
}

float herons_formula3f(vec3f A, vec3f B, vec3f C) { // triangle area
    float edge1_length = vector_magnitude3f(vector_subtract3f(A, B));
    float edge2_length = vector_magnitude3f(vector_subtract3f(B, C));
    float edge3_length = vector_magnitude3f(vector_subtract3f(C, A));
    float s = (edge1_length + edge2_length + edge3_length) / 2;
    float triangle_area = SDL_sqrt(s * (s - edge1_length) * (s - edge2_length) * (s - edge3_length));
    return triangle_area;
}

float vector_to_box_distance(vec6f view, vec6f box) { // https://gelamisalami.github.io/blog/posts/ray-box-intersection/
    vec3f view_ray = {SDL_sinf(view.b * SDL_PI_F/180) * SDL_cosf(view.a * SDL_PI_F/180), SDL_sinf(view.b * SDL_PI_F/180) * SDL_sinf(view.a * SDL_PI_F/180), SDL_cosf(view.b * SDL_PI_F/180)};
    box.a = box.a - view.x; // aabb min
    box.b = box.b - view.y;
    box.c = box.c - view.z;
    box.x = box.x - view.x; // aabb max
    box.y = box.y - view.y;
    box.z = box.z - view.z;

    vec3f planes_min = {-box.a/view_ray.x, -box.b/view_ray.y, -box.c/view_ray.z};
    vec3f planes_max = {-box.x/view_ray.x, -box.y/view_ray.y, -box.z/view_ray.z};

    vec3f planes_near = {SDL_min(planes_min.x, planes_max.x), SDL_min(planes_min.y, planes_max.y), SDL_min(planes_min.z, planes_max.z)};
    vec3f planes_far = {SDL_max(planes_min.x, planes_max.x), SDL_max(planes_min.y, planes_max.y), SDL_max(planes_min.z, planes_max.z)};

    float t_near = SDL_max(SDL_max(planes_near.x, planes_near.y), planes_near.z);
    float t_far = SDL_min(SDL_min(planes_far.x, planes_far.y), planes_far.z);

    if (t_near > t_far) {
        return 3.8e38; // no hit
    }
    return -t_near;
}

bool bounding_box_check(vec6f box1, vec6f box2) {
    //bool x_overlap =
    //bool y_overlap =
    //bool z_overlap =
    bool intersecting; // =
    return intersecting;
}

float line_portion_through_planef(vec3f line_start, vec3f line_end, vec4f plane) {
    vec3f p1 = line_start;
    vec3f p2 = line_end;
    float t = -(plane.x * p2.x + plane.y * p2.y + plane.z * p2.z + plane.w) / (plane.x * (p1.x - p2.x) + plane.y * (p1.y - p2.y) + plane.z * (p1.z - p2.z));
    return t;
}

bool is_point_in_trianglef(vec3f P, vec3f A, vec3f B, vec3f C) {
    // linear combination method : youtube: watch?v=HYAgJN3x4GA
    float w1 = ( A.x * (C.y - A.y) + (P.y - A.y) * (C.x - A.x) - P.x * (C.y - A.y) ) / ( (B.y - A.y) * (C.x - A.x) - (B.x - A.x) * (C.y - A.y) );
    float w2 = ( P.y - A.y - w1 * (B.y - A.y) ) / (C.y - A.y);
    bool inside;
    if ( (w1 >= 0) && (w2 >= 0) && (w1 + w2 <= 1) ) {
        inside = true;
    } else {
        inside = false;
    }
    /* heron's formula method : https://www.braynzarsoft.net/viewtutorial/q16390-24-triangle-to-triangle-collision-detection
    float actual_triangle1_area = herons_formula3f(tri_vert1, tri_vert2, tri_vert3);
    float triangle1_area_from_intersect = herons_formula3f(tri_vert1, tri_vert2, point) + herons_formula3f(tri_vert1, point, tri_vert3) + herons_formula3f(point, tri_vert2, tri_vert3);
    if (actual_triangle1_area * 1.001 >= triangle1_area_from_intersect) {
        inside = true;
    } else {
        inside = false;
    }*/
    return inside;
}

vec3f point_along_line3f(vec3f line_start, vec3f line_end, float along) {
    vec3f p1 = line_start;
    vec3f p2 = line_end;
    float t = along;
    vec3f point = (vec3f) {(p1.x * t) + (p2.x * (1 - t)), (p1.y * t) + (p2.y * (1 - t)), (p1.z * t) + (p2.z * (1 - t))};
    return point;
}

bool line_triangle_collisionf(vec3f line_start, vec3f line_end, vec3f tri_vert1, vec3f tri_vert2, vec3f tri_vert3) { // https://www.braynzarsoft.net/viewtutorial/q16390-24-triangle-to-triangle-collision-detection
    bool line_triangle_intersect = false; // assume they don't (needed for some reason, here)
    vec3f tri_norm = triangle_normal3f(tri_vert1, tri_vert2, tri_vert3);
    vec3f tri_cen = triangle_center3f(tri_vert1, tri_vert2, tri_vert3);
    vec4f tri_plane = plane_from_point_normal4f(tri_cen, tri_norm);

    float t; // represents how along the line segment the plane is (e.g., 0.5 = 50% to end of line from start & vice versa)
    vec3f intersect_point;
    t = line_portion_through_planef(line_start, line_end, tri_plane);
    if ((0 <= t) && (t <= 1)) {
        intersect_point = point_along_line3f(line_start, line_end, t); // on the plane
        line_triangle_intersect = is_point_in_trianglef(intersect_point, tri_vert1, tri_vert2, tri_vert3);
    }
    //SDL_Log("Hmm: %" SDL_PRIu32, line_triangle_intersect);
    return line_triangle_intersect;
}

int SDLCALL triangle_triangle_collisiontf(void *raw_data) {
    bool triangles_intersect = false; // assume they don't
    vec3f (*vertices)[6] = raw_data;
    //SDL_Log("Mem: %.3f", (*vertices)[5].x); // worker thread value

    if (!triangles_intersect) { // triangle 2 edge's intersect 1's face?
        triangles_intersect = line_triangle_collisionf((*vertices)[3], (*vertices)[4], (*vertices)[0], (*vertices)[1], (*vertices)[2]);
    }
    if (!triangles_intersect) {
        triangles_intersect = line_triangle_collisionf((*vertices)[3], (*vertices)[5], (*vertices)[0], (*vertices)[1], (*vertices)[2]);
    }
    if (!triangles_intersect) {
        triangles_intersect = line_triangle_collisionf((*vertices)[5], (*vertices)[3], (*vertices)[0], (*vertices)[1], (*vertices)[2]);
    }

    if (!triangles_intersect) { // triangle 1 edge's intersect 2's face?
        triangles_intersect = line_triangle_collisionf((*vertices)[0], (*vertices)[1], (*vertices)[3], (*vertices)[4], (*vertices)[5]);
    }
    if (!triangles_intersect) {
        triangles_intersect = line_triangle_collisionf((*vertices)[1], (*vertices)[2], (*vertices)[3], (*vertices)[4], (*vertices)[5]);
    }
    if (!triangles_intersect) {
        triangles_intersect = line_triangle_collisionf((*vertices)[2], (*vertices)[0], (*vertices)[3], (*vertices)[4], (*vertices)[5]);
    }

    //SDL_Log("Collision: %" SDL_PRIu32, triangles_intersect);
    return triangles_intersect;
}

bool triangle_triangle_collisionf(vec3f point1a, vec3f point1b, vec3f point1c, vec3f point2a, vec3f point2b, vec3f point2c) {
    int triangles_collide = 0; // assume they don't

    vec3f vertex_data[6] = {point1a, point1b, point1c, point2a, point2b, point2c};

    SDL_Thread *thread;
    //SDL_Log("OG: %.3f", vertex_data[5].x); //main thread values
    thread = SDL_CreateThread(triangle_triangle_collisiontf, "tritricolli", (void *) &vertex_data);
    SDL_WaitThread(thread, &triangles_collide);

    return (bool) triangles_collide;
}

#endif
