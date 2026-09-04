#ifndef COMMON_C
#define COMMON_C

// defines
#define physics_dt 0.001        // in seconds

// typedefs
typedef signed _BitInt(2) int2;
typedef signed _BitInt(4) int4;
typedef signed _BitInt(8) int8;
typedef signed _BitInt(16) int16;
typedef signed _BitInt(24) int24;
typedef signed _BitInt(32) int32;
typedef signed _BitInt(48) int48;
typedef signed _BitInt(64) int64;
typedef signed _BitInt(128) int128; // __int128

typedef unsigned _BitInt(32) uint32;
typedef unsigned _BitInt(64) uint64;

typedef struct vec2f {
    float x;
    float y;
} vec2f;

typedef struct vec3f {          // 6-9 significant decimal digits
    float x;
    float y;
    float z;
} vec3f;

typedef struct vec4f {
    float w;
    float x;
    float y;
    float z;
} vec4f;

typedef struct vec32i3f {       // exists because of indexing vertices
    int32 i;
    float x;
    float y;
    float z;
} vec32i3f;

typedef struct vec6f {          // exists because there're 6 DoF in 3D space
    float a;
    float b;
    float c;
    float x;
    float y;
    float z;
} vec6f;

typedef struct vec3d {          // 15-17 significant decimal digits
    double x;
    double y;
    double z;
} vec3d;

typedef struct vec3i8 {         // (3x)                -128 <--> 127
    int8 x;
    int8 y;
    int8 z;
} vec3i8;

typedef struct vec3i32 {        // (3x)     -2.147483648e9 <--> 2.147483648e9 -1
    int32 x;
    int32 y;
    int32 z;
} vec3i32;

typedef struct vec4i32 {        // ???
    int32 w;
    int32 x;
    int32 y;
    int32 z;
} vec4i32;

typedef struct vec5i32 {        // exists because index tetrahedra of indexed vertices
    int32 a;
    int32 b;
    int32 x;
    int32 y;
    int32 z;
} vec5i32;

typedef struct vec3i64 {        // (3x) -1.84467440737e+19 <--> 1.84467440737e+19 -1
    int64 x;
    int64 y;
    int64 z;
} vec3i64;

typedef struct vec3i128 {       // (3x) -3.40282366921e+38 <--> 3.40282366921e+38 -1
    int128 x;
    int128 y;
    int128 z;
} vec3i128;

typedef struct vec4i128 {
    int128 w;
    int128 x;
    int128 y;
    int128 z;
} vec4i128;

typedef struct vec6i128 {
    int128 a;
    int128 b;
    int128 c;
    int128 x;
    int128 y;
    int128 z;
} vec6i128;

typedef struct meshf {
    // void* parent;
    int element_count;
    int element_size;           // "4" -> quads, "3" -> triangles, "2" -> lines, "1" -> points
    //vec3f offset;
    vec6f bounding_box;
    //void* polygons // array of pointers
    vec3f *vertices;
    vec3f *normals;
    vec4f *colors;
    //mesh* connected_meshes;   // paired with below
    //vec3f *shared_vertices;   // paired with above
} meshf;

typedef struct mesh128i {
    void *parent;
    void *children;             // dynamic array of pointers
    int32 children_count;

    int32 element_count;        // number of quads + triangles + lines + points; e.g., 3,3,3,2,2,3,3,3,1,2,2
    int4 *element_size;         // dynamic array length = element_count; "4" -> quads, "3" -> triangles, "2" -> lines, "1" -> points
    void *elements;
    int4 *element_type;

    vec3i128 *vertices_pos;
    vec3i128 *vertices_vec;
    vec3i128 *vertices_acc;
    vec3f *normals;
    vec4f *colors;              // w = alpha, xyz = rgb
    int32 *materials;           // might merge with above
    float *temperatures;

    vec4i128 bounding_sphere;   // w = radius, xyz = position
    vec6i128 bounding_box;      // vec3 aabbmin + vec3 aabbmax

    void *connected_meshes;     // dynamic array of pointers; paired with below
    vec3i128 *shared_vertices;  // paired with above
    int8 *connection_types;     // e.g., weld, solid/homogeneous, etc.
} mesh128i;

typedef struct fenics_mesh {
    int node_count;
    vec3f *vertices;
    vec3f *normals;
    vec4f *colors;
    vec3i32 *tetrahedra;
} fenics_mesh;

// structs
SDL_FRect rect4f;

SDL_Rect rect4i;

struct mouse_button {           // https://wiki.libsdl.org/SDL3/SDL_MouseButtonEvent
    //SDL_MouseID mouseID;      // note
    //SDL_WindowID windowID;
    float x;                    // last click's x pos
    float y;
    bool down;
    bool toggle;
};

struct mouse_wheel {            // https://wiki.libsdl.org/SDL3/SDL_MouseWheelEvent ; scroll wheel and trackpad
    //SDL_MouseID mouseID;
    //SDL_WindowID windowID;
    float x;                    // + > R
    float y;                    // + > away
    int32 x_accu;
    int32 y_accu;
    bool down;
    bool toggle;
};

struct mouse {
    struct mouse_button left;   // e.g., "mouse.left.down"
    struct mouse_button right;
    struct mouse_wheel wheel;
    //SDL_MouseID mouseID;
    //SDL_WindowID windowID;
    bool moving;
    bool scrolling;
    float x;
    float y;
    float x_rel;
    float y_rel;
};

#endif
