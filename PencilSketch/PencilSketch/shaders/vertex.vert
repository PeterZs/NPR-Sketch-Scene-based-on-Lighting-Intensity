#version 330

layout(location=0) in vec3 iPosition;
layout(location=1) in vec2 tex_coord;
layout(location=2) in vec3 iNormal;

out vec2 TexCoord0;
out vec3 normal;
out vec3 pos;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main(){
	gl_Position=proj*view*model*vec4(iPosition,1.0);
	TexCoord0=tex_coord;
	normal = vec4(model * vec4(iNormal, 0.f)).xyz;
	pos = (model * vec4(iPosition,1.0)).xyz;
}