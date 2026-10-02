#ifndef COMMON_C
#define COMMON_C

#include "../lib/cglm-0.9.6/include/cglm/cglm.h"
#include <SDL3/SDL_time.h>

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
typedef signed _BitInt(96) int96;
typedef signed _BitInt(128) int128; // __int128

typedef unsigned _BitInt(8) uint8;
typedef unsigned _BitInt(16) uint16;
typedef unsigned _BitInt(32) uint32;
typedef unsigned _BitInt(64) uint64;
typedef unsigned _BitInt(96) uint96;
typedef unsigned _BitInt(128) uint128;

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
    int32 w;
    float x;
    float y;
    float z;
} vec32i3d;

typedef struct vec32i3d {
    int32 w;
    double x;
    double y;
    double z;
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

typedef struct vec4i32 {        // index triangles using vert references
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

typedef struct vec32i3i128 {       // (3x) -3.40282366921e+38 <--> 3.40282366921e+38 -1
    int32 w;
    int128 x;
    int128 y;
    int128 z;
} vec32i3i128;

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

typedef struct vec3f3i128 {
    float a;
    float b;
    float c;
    int128 x;
    int128 y;
    int128 z;
} vec3f3i128;

typedef struct pos3i128 {
    vec3i128 p;
    vec3f v;
    vec3f a;
} pos3i128;

struct point_particle { // all derived*
    float m;            // mass
    pos3i128 t;         // translative pos, vel, acc
    float I;            // rotational inertia
    pos3i128 r;         // rotational pos, vel, acc
    float Q;            // charge
    vec3i128 AABB[4];   // bounding box
    char mat[];         // material (*not derived)
};

struct geometry {
    int32 vert_cnt;
    uint32 vert_index_cnt;
    int32 tetra_cnt;
    int32 tri_cnt;
    vec32i3f *vertf;
    vec32i3d *vertd;
    vec32i3i128 *vert128;   // for rendering* (deep copied from below)
    vec32i3i128 *vert128_buffer; // for physics (current working buffer)
    vec5i32 *tetra;
    vec4i32 *tri;       // surface (if filtered)
};

struct body {
    struct geometry geo;
    struct point_particle CM;
    struct point_particle CM_prev;
};

typedef void (APIENTRY * glGenVertexArrays_func)(GLsizei n, GLuint *arrays);
glGenVertexArrays_func glGenVertexArrays = 0;

typedef void (APIENTRY * glBindVertexArray_func)(GLuint array);
glBindVertexArray_func glBindVertexArray = 0;

#ifdef SDL_PLATFORM_WIN32
typedef void (APIENTRY * glGenBuffers_func)(GLsizei n, GLuint * buffers); // https://wiki.libsdl.org/SDL3/SDL_GL_GetProcAddress
glGenBuffers_func glGenBuffers = 0;

typedef void (APIENTRY * glBindBuffer_func)(GLenum target, GLuint buffer);
glBindBuffer_func glBindBuffer = 0;

typedef void (APIENTRY * glBufferData_func)(GLenum target, GLsizeiptr size, const void * data, GLenum usage);
glBufferData_func glBufferData = 0;

typedef GLuint (APIENTRY * glCreateShader_func)(GLenum shaderType);
glCreateShader_func glCreateShader = 0;

typedef void (APIENTRY * glShaderSource_func)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
glShaderSource_func glShaderSource = 0;

typedef void (APIENTRY * glCompileShader_func)(GLuint shader);
glCompileShader_func glCompileShader = 0;

typedef GLuint (APIENTRY * glCreateProgram_func)(void);
glCreateProgram_func glCreateProgram = 0;

typedef void (APIENTRY * glAttachShader_func)(GLuint program, GLuint shader);
glAttachShader_func glAttachShader = 0;

typedef void (APIENTRY * glLinkProgram_func)(GLuint program);
glLinkProgram_func glLinkProgram = 0;

typedef void (APIENTRY * glDeleteShader_func)(GLuint shader);
glDeleteShader_func glDeleteShader = 0;

typedef void (APIENTRY * glVertexAttribPointer_func)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer);
glVertexAttribPointer_func glVertexAttribPointer = 0;

typedef void (APIENTRY * glEnableVertexAttribArray_func)(GLuint index);
glEnableVertexAttribArray_func glEnableVertexAttribArray = 0;

typedef void (APIENTRY * glUseProgram_func)(GLuint program);
glUseProgram_func glUseProgram = 0;

typedef void (APIENTRY * glGetProgramiv_func)(GLuint program, GLenum pname, GLint *params);
glGetProgramiv_func glGetProgramiv = 0;

typedef void (APIENTRY * glGetProgramInfoLog_func)(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
glGetProgramInfoLog_func glGetProgramInfoLog = 0;

typedef void (APIENTRY * glGetShaderiv_func)(GLuint shader, GLenum pname, GLint *params);
glGetShaderiv_func glGetShaderiv = 0;

typedef void (APIENTRY * glGetShaderInfoLog_func)(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
glGetShaderInfoLog_func glGetShaderInfoLog = 0;

typedef void (APIENTRY * glUniformMatrix4fv_func)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value); // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUniform.xhtml
glUniformMatrix4fv_func glUniformMatrix4fv = 0;

typedef GLint (APIENTRY * glGetUniformLocation_func)(GLuint program, const GLchar *name);
glGetUniformLocation_func glGetUniformLocation = 0;

typedef void (APIENTRY * glGetnUniformfv_func)(GLuint program, GLint location, GLsizei bufSize, GLfloat *params);
glGetnUniformfv_func glGetnUniformfv = 0;

typedef void (APIENTRY * glProgramUniformMatrix4fv_func)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
glProgramUniformMatrix4fv_func glProgramUniformMatrix4fv = 0;

typedef void (APIENTRY * glUniform4f_func)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
glUniform4f_func glUniform4f = 0;

typedef void (APIENTRY * glBufferSubData_func)(GLenum target, GLintptr offset, GLsizeiptr size, const void * data);
glBufferSubData_func glBufferSubData = 0;

typedef void (APIENTRY * glUniform3f_func)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
glUniform3f_func glUniform3f = 0;

typedef void (APIENTRY * glActiveTexture_func)(GLenum texture);
glActiveTexture_func glActiveTexture_ = 0;
#endif

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

struct finger {
    float x;
    float y;
    float p; // pressure
    bool down;
    float dx;
    float dy;
};

struct touch {
    struct finger finger;
};

#endif
