#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_EntityID;

uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;
out vec4 v_Color;
out flat int v_EntityID;

void main() {
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_EntityID = a_EntityID;
	gl_Position = u_ViewProjection * vec4(a_Position.xy, 0.0, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat int v_EntityID;

uniform sampler2D u_Text;

void main() {
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Text, v_TexCoord).r);
	color =  v_Color * sampled;
	color2 = v_EntityID;
}