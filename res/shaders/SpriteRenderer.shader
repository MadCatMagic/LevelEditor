#zwrite on
#cull off
#blend on
#blendsrc SrcAlpha
#blenddest OneMinusSrcAlpha
#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoords;
out vec2 UV;

void main() {
	gl_Position = vec4(pos, 1);
	UV = texCoords;
}

#shader fragment
#version 330 core

in vec2 UV;
out vec4 colour;

uniform vec4 tint;

uniform sampler2D blitTexture;
uniform sampler2D depthTexture;

void main() {
	colour = texture(blitTexture, UV) * tint;
}