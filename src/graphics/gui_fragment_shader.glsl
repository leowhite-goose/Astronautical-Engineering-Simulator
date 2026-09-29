#version 300 es
out mediump vec4 frag_color;

in mediump vec2 tex_coord;

uniform sampler2D frag_texture;

void main()
{
    frag_color = texture(frag_texture, tex_coord);
}
