#version 330 core

uniform mat4 model;
uniform mat4 MVP;

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 UV;

out vec2 texcoord;
out vec3 normal;
out vec3 FragPos;

void main() {
	texcoord = UV;
	normal = mat3(transpose(inverse(model))) * Normal;
	FragPos = vec3(model * vec4(Position, 1.0));
	gl_Position = MVP * vec4(Position, 1.0);
}

