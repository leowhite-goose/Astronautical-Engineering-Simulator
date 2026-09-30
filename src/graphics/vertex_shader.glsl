#version 300 es
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec4 vertex_color;
layout (location = 2) in vec3 vertex_normal;

out highp float flogz;
out highp vec4 element_base_color;
out highp vec3 frag_normal;
out highp vec3 frag_pos;

uniform highp mat4 view;
uniform highp mat4 projection;

void main() {
    gl_Position = projection * view * vec4(vertex_position, 1.0) * 4e-3; // right to left multiplication; 4e-3 --> scale-down factor
    highp float farplane = 1.36e36; // scaled down by 4e-3
    highp float Fcoef = 2.0 / log2(farplane + 1.0);
    highp float nearplane = 0.1;// post scale-down, so actually 25 base units
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * Fcoef - nearplane; // https://outerra.blogspot.com/2013/07/logarithmic-depth-buffer-optimizations.html
    flogz = 1.0 + gl_Position.w;
    element_base_color = vertex_color;
    frag_normal = vertex_normal;
    frag_pos = vertex_position/(1e20); // 1e18 --> light clips far at max scale; 4e18 --> light clips near at smallest scale
}
