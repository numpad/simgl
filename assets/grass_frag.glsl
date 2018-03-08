#version 330 core

uniform sampler2D TexImage;

in vec2 TexCoords;

out vec4 FragColor;

void main() {
	vec4 pixel = texture(TexImage, TexCoords);
	FragColor = pixel;
}

