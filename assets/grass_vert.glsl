#version 330 core

uniform mat4 mProjection;
uniform mat4 mView;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 UV;
layout (location = 3) in mat4 iModel;

out vec2 TexCoords;

void main() {
	TexCoords = UV;
	
	mat4 modelView = mView * iModel;
	
	vec3 rsX = modelView[0].xyz;
	vec3 rsZ = modelView[2].xyz;
	
	/* remove rotation but keep scale */
	modelView[0].xyz = vec3(0.0);
	modelView[2].xyz = vec3(0.0);
	modelView[0][0] = abs(length(rsX));
	modelView[2][2] = abs(length(rsZ));

	gl_Position = mProjection * modelView * vec4(Position, 1.0);
}

