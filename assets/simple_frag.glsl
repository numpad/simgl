#version 330 core

uniform sampler2D TexImage;

in vec3 normal;
in vec2 texcoord;

out vec4 Color;

void main() {
	Color = vec4(texture(TexImage, texcoord).rrr, 1.0);
}

