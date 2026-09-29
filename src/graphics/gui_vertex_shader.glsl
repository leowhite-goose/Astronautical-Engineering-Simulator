#version 300 es
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 texture_coords;

out mediump vec2 tex_coord;

void main() {
    gl_Position = vec4(vertex_position, 1.0, 1.0);
    tex_coord = texture_coords;
}
