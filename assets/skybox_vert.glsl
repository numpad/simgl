#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 UV;

uniform mat4 mProjection;
uniform mat4 mView;
uniform mat4 mModel;

out vec3 TexCoords;

void main() {
	TexCoords = Position;
	gl_Position = mProjection * mView * mModel * vec4(Position, 1.0);
}

