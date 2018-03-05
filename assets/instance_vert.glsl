#version 330 core

uniform mat4 mProjection;
uniform mat4 mView;

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in mat4 InstanceModel;

out vec3 normal;
out vec2 texcoord;

void main() {
	texcoord = TexCoord;
	normal = normalize(mat3(transpose(inverse(InstanceModel))) * Normal);
	gl_Position = mProjection * mView * InstanceModel * vec4(Position, 1.0);
}

