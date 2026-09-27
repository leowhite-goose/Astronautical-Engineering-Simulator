#version 330 core
layout (location = 0) in vec3 pos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    //gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
    //mat4 test = mat4(2,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
    //gl_Position = test * vec4(pos, 1.0);
    gl_Position = projection * view * vec4(pos, 1.0); // right to left multiplication
}
