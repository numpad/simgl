#version 330 core

uniform vec3 color;
in vec3 vertex_color;
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
	Color = vec4(gamma_correct(vertex_color), 1.0);
}

