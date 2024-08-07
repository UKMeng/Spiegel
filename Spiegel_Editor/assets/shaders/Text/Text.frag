#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIndex;
in flat int v_EntityID;

layout(binding = 0) uniform sampler2D u_Text[32];

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Text[int(v_TexIndex)], v_TexCoord).r);
    color =  v_Color * sampled;
    color2 = v_EntityID;
}