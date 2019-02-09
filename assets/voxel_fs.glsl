#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vColor;

out vec4 FragColor;

void main() {
	vec3 out_color = vColor;
	FragColor = vec4(out_color, 1.0);
}

