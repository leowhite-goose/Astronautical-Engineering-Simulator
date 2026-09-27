#version 330 core
out vec4 frag_color;
in float flogz;
in vec4 element_base_color;

void main()
{
    float farplane = 1.1e12;
    float Fcoef = 2.0 / log2(farplane + 1.0);
    gl_FragDepth = log2(flogz) * 0.5 * Fcoef;
    frag_color = element_base_color;
}
