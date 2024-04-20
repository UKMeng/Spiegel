#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in int a_EntityID;

layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};
uniform float u_Time;

vec3 col = 0.5 + 0.5*cos(u_Time+a_Position.xyx+vec3(0,2,4));

out flat int v_EntityID;
out vec3 v_Color;

void main() {
	v_EntityID = a_EntityID;
	v_Color = col;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in flat int v_EntityID;
in vec3 v_Color;

void main() {
	o_Color =  vec4(v_Color, 1.0);
	o_EntityID = v_EntityID;
}