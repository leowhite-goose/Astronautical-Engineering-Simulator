#version 330 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec4 vertex_color;

out float flogz;
out vec4 element_base_color;

uniform mat4 view;
uniform mat4 projection;

void main() {
    //gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
    //mat4 test = mat4(2,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
    //gl_Position = test * vec4(pos, 1.0);
    gl_Position = projection * view * vec4(vertex_position, 1.0); // right to left multiplication
    float farplane = 1.1e12;
    float Fcoef = 2.0 / log2(farplane + 1.0);
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * Fcoef - 1.0; // https://outerra.blogspot.com/2013/07/logarithmic-depth-buffer-optimizations.html
    flogz = 1.0 + gl_Position.w;
    element_base_color = vertex_color;
}
