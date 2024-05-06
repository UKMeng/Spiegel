#type vertex
#version 450 core

layout(location = 0) in vec4 a_Position;

uniform mat4 u_ViewProjection;

void main() {
	gl_Position = u_ViewProjection * a_Position;
}

#type fragment
#version 450 core

void main() {
	//
}