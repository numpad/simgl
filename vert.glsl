#version 330 core

uniform mat4 MVP;

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Color;

void main() {
	gl_Position = MVP * vec4(Position, 1.0);
}

