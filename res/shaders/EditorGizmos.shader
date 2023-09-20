#zwrite off
#cull off
#blend on
#blendsrc SrcAlpha
#blenddest OneMinusSrcAlpha
#shader vertex
#version 330 core

layout(location = 0) in vec2 pos;

void main()
{
    gl_Position = vec4(pos, 0, 1);
}

#shader fragment
#version 330 core

out vec4 colour;

uniform vec4 tint;

void main()
{
    colour = tint;
}