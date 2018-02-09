#version 330 core

uniform mat4 MVP;
layout(location = 0) in vec3 Position;
out vec4 color;

void main() {
	gl_Position = MVP * vec4(Position, 1.0);
	color = vec4(1.0, 0.0, 0.0, 1.0);
}

