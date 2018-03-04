#version 330 core

uniform mat4 mMVP;
uniform mat4 mModel;

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 UV;

out vec3 normal;
out vec2 texcoord;

void main() {
	texcoord = UV;
	normal = mat3(transpose(inverse(mModel))) * Normal;
	gl_Position = mMVP * vec4(Position, 1.0);
}

