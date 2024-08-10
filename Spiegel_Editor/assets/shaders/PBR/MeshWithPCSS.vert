#version 450 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in int a_EntityID;

layout(std140, binding = 1) uniform Camera {
    mat4 u_ViewProjection;
    vec3 u_ViewPosition;
};

uniform	mat4 u_LightSpaceMatrix;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_Pos;
out vec4 v_FragPosLightSpace;
out flat int v_EntityID;

void main() {
    v_TexCoord = a_TexCoord;
    v_Normal = a_Normal;
    v_Pos = a_Position.xyz;
    v_FragPosLightSpace = u_LightSpaceMatrix * a_Position;
    v_EntityID = a_EntityID;
    gl_Position = u_ViewProjection * a_Position;
}
