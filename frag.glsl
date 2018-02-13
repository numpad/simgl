#version 330 core

uniform vec3 color;
uniform sampler2D teximage;

in vec2 texcoord;
out vec4 Color;

float scalar_gamma_correct(float c) {
	return pow(c, 1.0 / 2.2);
}

vec3 gamma_correct(vec3 color) {
	return vec3(
		scalar_gamma_correct(color.r),
		scalar_gamma_correct(color.g),
		scalar_gamma_correct(color.b)
	);
}

void main() {
	Color = vec4(texture(teximage, vec2(texcoord.x, 1.0 - texcoord.y)).rgb, 1.0);
}

