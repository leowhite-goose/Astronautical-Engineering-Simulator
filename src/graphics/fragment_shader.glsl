#version 330 core
out vec4 frag_color;

in float flogz;
in vec4 element_base_color;
in vec3 frag_normal;

uniform vec3 light_pos;

void main()
{
    // depth buffer
    float farplane = 3.40282e38/6;
    float Fcoef = 2.0 / log2(farplane + 1.0);
    gl_FragDepth = log2(flogz) * 0.5 * Fcoef;

    // lighting
    vec3 light_color = vec3(1.0,1.0,1.0);
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * light_color;

    vec3 light_test = vec3(-1,0,0);
    float diff = max(dot(frag_normal, light_test), 0.0);
    vec3 diffuse = diff * light_color;

    //vec4 result = (vec4(ambient,1.0) + vec4(diffuse,1.0)) * element_base_color;
    vec4 result = vec4(ambient + diffuse, 1.0) * element_base_color;
    frag_color = result;
}
