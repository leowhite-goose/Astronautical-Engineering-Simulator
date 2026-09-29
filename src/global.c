#ifndef GLOBAL_C
#define GLOBAL_C

#define MOUSE_GRAB_PADDING 8

static SDL_Window *root_window;
static SDL_Renderer *root_gui_renderer;
static SDL_Texture *root_gui_texture;
GLuint root_gui_gl_texture; // https://stackoverflow.com/questions/11281787/sdl-surface-to-opengl-texture?rq=3
static SDL_Surface *root_gui_surface;
static SDL_GLContext root_gl_context;
int root_window_width = 640; // initial main window width
int root_window_height = 480;
int16 last_tps;

const bool *keyboard_scancode_down_state;
int key_count;
bool keyboard_scancode_toggled_state[SDL_SCANCODE_COUNT]; // https://wiki.libsdl.org/SDL3/SDL_Scancode
SDL_Keymod keymod_state;
struct mouse mouse;
struct touch touch[10];

#define SCALE 1e15 // 1e-1 1e2 1e9 1e35; 1e15 --> 1 = fm, 1e15 = meter
vec3f3i128 root_cam = {45,75,0,-67*SCALE,-60*SCALE,-27*SCALE};
//vec3f3i128 cam_pos = {};

// universal constants
const float G_f = 6.67430e-11; // m^3 * kg^-1 * s^-2 ; approx.
const uint32 c_i32 = 299792458;  // m/s ; exact
const uint64 c_sq_ui64 = c_i32 * c_i32;

// debug
vec4f debug_color = {1.0, 0.7, 0.3, 0.5}; // "bubblegum pink"
float global_fp;
vec3f p1a = {-100,0,0};
vec3f p1b = {-100,1,0};
vec3f p1c = {-100.5,1,1};
vec3f p2a = {-99,0,0};
vec3f p2b = {-99,1,0};
vec3f p2c = {-102,4,1};
bool blue_overlap;
static bool touch_button[32];
static int8 by_finger[32];
static float touch_analog[6];
static int32 touch_digital[8];

// world
SDL_Thread *physics_thread;
struct body body[16];

// system
int64 shortest_delay_ns;

// geometry-rendering
float *normal_data;
float *color_data;
float *vertex_data;
uint16 gui_texture_res;

unsigned int vbo, vao, ebo;
unsigned int shader_program;
unsigned int unlit_shader;
unsigned int tex_shader_program;

#endif
