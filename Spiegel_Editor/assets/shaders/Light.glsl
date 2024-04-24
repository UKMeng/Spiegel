#type vertex
#version 450 core

layout(location = 0) in vec4 a_Position;
layout(location = 3) in int a_EntityID;

layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};

out flat int v_EntityID;

void main() {
	v_EntityID = a_EntityID;;
	gl_Position = u_ViewProjection * a_Position;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

flat in int v_EntityID;

void main() {
	o_Color = vec4(1.0);
	o_EntityID = v_EntityID;
}