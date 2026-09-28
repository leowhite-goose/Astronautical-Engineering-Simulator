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
    vec3 light_color = vec3(1.0,1.0,1.0); //vec3(1.0,0.4,0.4);
    float ambient_strength = 0.2;
    vec3 ambient = ambient_strength * light_color;

    vec3 light_post = light_pos/(8e16);
    vec3 light_dir = normalize(light_post - frag_pos);
    float diff = max(dot(frag_normal, light_dir), 0.0);// frag_normal.z * (-1); //max(dot(frag_normal, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    vec4 result = vec4(ambient + diffuse, 1.0) * element_base_color;
    frag_color = result;
}
