#zwrite off
#cull off
#blend on
#blendsrc SrcAlpha
#blenddest OneMinusSrcAlpha
#shader vertex
#version 430 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec2 realPos;
layout(location = 3) in vec3 realScale_Rot;
layout(location = 4) in vec4 tintIn;

out vec2 UV;
flat out vec4 tint;

void main() {
	float sangle = sin(realScale_Rot.z);
	float cangle = cos(realScale_Rot.z);

	vec2 quadData = vec2(
		cangle * pos.x - sangle * pos.y,
		sangle * pos.x + cangle * pos.y
	);

	gl_Position = vec4(quadData * realScale_Rot.xy + realPos.xy, 0, 1);
	UV = texCoords;
	tint = tintIn;
}

#shader fragment
#version 430 core

in vec2 UV;
flat in vec4 tint;
out vec4 colour;

uniform sampler2D blitTexture;

void main() {
	colour = texture(blitTexture, UV) * tint;
}