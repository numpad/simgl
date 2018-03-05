#version 330 core

uniform sampler2D TexImage;

in vec3 normal;
in vec2 texcoord;
in vec3 instance_color;

out vec4 Color;

void main() {
	vec3 pixel = texture(TexImage, texcoord).rrr;
	float illum = dot(normalize(normal), vec3(0.0, 1.0, 0.0));
	illum = max(illum, 0.065);

	Color = vec4(illum * mix(pixel, instance_color, 0.5), 1.0);
}

