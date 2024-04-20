#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;


layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};

out vec4 v_Color;

void main() {
	v_Color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in vec4 v_Color;

void main() {
	o_Color = v_Color;
	o_EntityID = -1;
}