#version 330 core

uniform sampler2D uDiffuse;

in vec2 fTexCoord;

out vec4 FragColor;

void main() {
	FragColor = vec4(texture(uDiffuse, fTexCoord).rgb, 1.0);
}
