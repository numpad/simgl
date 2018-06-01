#version 330 core

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

out vec2 fTexCoord;
out vec3 fNormal;
out vec3 fPosition;

void main() {
	fTexCoord = vTexCoord;
	fNormal = transpose(inverse(mat3(uModel))) * vNormal;
	fPosition = vec3(uModel * vec4(vPosition, 1.0));
	gl_Position = uProjection * uView * uModel * vec4(vPosition, 1.0);
}

