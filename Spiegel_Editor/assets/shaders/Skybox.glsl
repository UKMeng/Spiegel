#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

out vec3 v_TexCoords;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
	v_TexCoords = a_Position;
	vec4 pos = u_Projection * u_View * vec4(a_Position, 1.0);
	gl_Position = pos.xyww;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int EntityID;


in vec3 v_TexCoords;

layout(binding = 32) uniform samplerCube u_Skybox;

void main() {
	color = texture(u_Skybox, v_TexCoords);
	EntityID = -1;
}