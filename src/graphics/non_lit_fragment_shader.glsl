#version 330 core
out vec4 frag_color;

in float flogz;
in vec4 element_base_color;
in vec3 frag_normal;
in vec3 frag_pos;

uniform vec3 light_pos;

void main()
{
    // depth buffer
    float farplane = 3.40282e38/6;
    float Fcoef = 2.0 / log2(farplane + 1.0);
    gl_FragDepth = log2(flogz) * 0.5 * Fcoef;

    // lighting
    vec3 light_color = vec3(1.0,1.0,1.0); //vec3(1.0,0.7,0.7);

    vec4 result = vec4(light_color, 1.0) * element_base_color;
    frag_color = result;
}
