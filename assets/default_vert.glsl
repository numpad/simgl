#version 330 core

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

out vec2 fTexCoord;

void main() {
	fTexCoord = vTexCoord;
	gl_Position = uProjection * uView * uModel * vec4(vPosition, 1.0);
}
