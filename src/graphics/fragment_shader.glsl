#version 330 core
out vec4 frag_color;
in vec4 element_base_color;

void main()
{
    frag_color = element_base_color;
}
