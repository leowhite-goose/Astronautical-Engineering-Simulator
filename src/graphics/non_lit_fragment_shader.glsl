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
    highp float farplane = 5.67e37; //3.40282e38/6;
    highp float Fcoef = 2.0 / log2(farplane + 1.0);
    gl_FragDepth = log2(flogz) * 0.5 * Fcoef;

    // lighting
    highp vec3 light_color = vec3(1.0,1.0,1.0); //vec3(1.0,0.7,0.7);

    highp vec4 result = vec4(light_color, 1.0) * element_base_color;
    frag_color = result;
}
