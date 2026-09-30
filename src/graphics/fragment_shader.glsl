#version 300 es
out highp vec4 frag_color;

in highp float flogz;
in highp vec4 element_base_color;
in highp vec3 frag_normal;
in highp vec3 frag_pos;

uniform highp vec3 light_pos;

void main()
{
    // depth buffer
    highp float farplane = 1.36e36; //5.67e37; //3.40282e38/6;
    highp float Fcoef = 2.0 / log2(farplane + 1.0);
    gl_FragDepth = log2(flogz) * 0.5 * Fcoef;

    // lighting
    highp vec3 light_color = vec3(1.0,1.0,1.0); //vec3(1.0,0.4,0.4);
    highp float ambient_strength = 0.2;
    highp vec3 ambient = ambient_strength * light_color;

    // diffusion
    highp vec3 light_post = light_pos/(1e20);
    highp vec3 light_dir = normalize(light_post - frag_pos);
    highp float diff = max(dot(frag_normal, light_dir), 0.0);// frag_normal.z * (-1); //max(dot(frag_normal, light_dir), 0.0);
    highp vec3 diffuse = diff * light_color;

    // final color
    highp vec4 result = vec4(ambient + diffuse, 1.0) * element_base_color;
    frag_color = result;
}
