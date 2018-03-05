#version 330 core

uniform mat4 mProjection;
uniform mat4 mView;

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in mat4 InstanceModel;
layout(location = 7) in vec3 InstanceColor;

out vec3 normal;
out vec2 texcoord;
out vec3 instance_color;

void main() {
	instance_color = InstanceColor;
	texcoord = TexCoord;
	normal = normalize(mat3(transpose(inverse(InstanceModel))) * Normal);
	gl_Position = mProjection * mView * InstanceModel * vec4(Position, 1.0);
}

