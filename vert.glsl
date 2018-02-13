#version 330 core

uniform mat4 MVP;

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 UV;

out vec2 texcoord;

void main() {
	texcoord = UV;
	gl_Position = MVP * vec4(Position, 1.0);
}

