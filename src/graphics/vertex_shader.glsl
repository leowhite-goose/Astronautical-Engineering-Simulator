#version 330 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec4 vertex_color;
layout (location = 2) in vec3 vertex_normal;

out float flogz;
out vec4 element_base_color;
out vec3 frag_normal;
out vec3 frag_pos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    //gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
    //mat4 test = mat4(2,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
    //gl_Position = test * vec4(pos, 1.0);
    gl_Position = projection * view * vec4(vertex_position, 1.0) * 5e-2; // right to left multiplication; 1e0 --> perfect at smallest; 5e-2 --> kinda ehh at smallest & maximum scales
    float farplane = 3.40282e38/6;
    float Fcoef = 2.0 / log2(farplane + 1.0);
    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * Fcoef - 1.0; // https://outerra.blogspot.com/2013/07/logarithmic-depth-buffer-optimizations.html
    gl_Position.z *= gl_Position.w; // https://papadanku.github.io/blog/learned/logdepth.html
    flogz = 1.0 + gl_Position.w;
    element_base_color = vertex_color;
    frag_normal = vertex_normal;
    frag_pos = vertex_position/(1e18); // 1e18 --> light clips far at max scale; 4e18 --> light clips near at smallest scale
}
