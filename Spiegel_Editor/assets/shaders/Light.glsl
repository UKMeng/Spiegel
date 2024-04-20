#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};

uniform mat4 model;

void main() {
	gl_Position = u_ViewProjection * model * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

void main() {
	o_Color = vec4(1.0);
	o_EntityID = -1;
}