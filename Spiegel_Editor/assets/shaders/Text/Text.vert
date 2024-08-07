﻿#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityID;

layout(std140, binding = 0) uniform Camera {
    mat4 u_ViewProjection;
};

out vec2 v_TexCoord;
out vec4 v_Color;
out flat float v_TexIndex;
out flat int v_EntityID;

void main() {
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    v_EntityID = a_EntityID;
    gl_Position = u_ViewProjection * vec4(a_Position.xy, 0.0, 1.0);
}