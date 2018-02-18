#version 330 core

uniform sampler2D teximage;
uniform sampler2D teximage2;

in vec2 texcoord;
in vec3 normal;
out vec4 FragColor;

float scalar_gamma_correct(float c) {
	return pow(c, 1.0 / 2.2);
}

vec4 gamma_correct(vec4 color) {
	return vec4(
		scalar_gamma_correct(color.r),
		scalar_gamma_correct(color.g),
		scalar_gamma_correct(color.b),
		color.a
	);
}

void main() {
	vec4 pixel = texture(teximage, texcoord);
	vec4 data = texture(teximage2, texcoord);
	pixel.a = 1.0;
	
	FragColor = pixel * (normal.z * -1.0 + 0.4);
}

