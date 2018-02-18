#version 330 core

uniform mat4 MVP;

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 UV;

out vec2 texcoord;
out vec3 normal;

void main() {
	texcoord = UV;
	normal = normalize(MVP * vec4(Normal, 0.0)).xyz;
	gl_Position = MVP * vec4(Position, 1.0);
}

