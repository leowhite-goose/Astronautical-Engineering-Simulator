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
struct touch touch;

vec6f root_cam = {50,90,0,-70,-110,-10};

// universal constants
const float G_fp32 = 6.67430e-11; // m^3 * kg^-1 * s^-2 ; approx.
const _BitInt(32) c_i32 = 299792458;  // m/s ; exact

// debug
vec4f debug_color = {1.0, 0.7, 0.3, 0.5}; // "bubblegum pink"
float global_fp;
vec3f p1a = {0,0,0};
vec3f p1b = {0,1,0};
vec3f p1c = {0.5,1,1};
vec3f p2a = {1,0,0};
vec3f p2b = {1,1,0};
vec3f p2c = {-2,4,1};
bool blue_overlap;
static bool touch_button[32];
static float touch_analog[6];

// world
SDL_Thread *physics_thread;
mesh128i (*universe)[0];
int32 model_cell_count;
vec32i3f *model_vertices = NULL;
vec5i32 *model_tetrahedra = NULL;

// system
int64 shortest_delay_ns;

// geometry-rendering
float *normal_data;
float *color_data;
float *vertex_data;
uint16 gui_texture_res;

#endif
