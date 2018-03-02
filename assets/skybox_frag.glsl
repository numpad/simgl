#version 330 core

uniform samplerCube SkyboxTexture;

in vec3 TexCoords;
out vec4 Color;

void main() {
	Color = texture(SkyboxTexture, TexCoords);
}

