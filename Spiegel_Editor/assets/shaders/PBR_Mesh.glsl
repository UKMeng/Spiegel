#type vertex
#version 450 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in float a_DiffuseTextureID;
layout(location = 4) in float a_SpecularTextureID;
layout(location = 5) in int a_EntityID;


layout(std140, binding = 1) uniform Camera {
	mat4 u_ViewProjection;
	vec3 u_ViewPosition;
};

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_Pos;
out flat float v_DiffuseTextureID;
out flat float v_SpecularTextureID;
out flat int v_EntityID;

void main() {
	v_TexCoord = a_TexCoord;
	v_Normal = a_Normal;
	v_Pos = a_Position.xyz;
	v_DiffuseTextureID = a_DiffuseTextureID;
	v_SpecularTextureID = a_SpecularTextureID;
	v_EntityID = a_EntityID;
	gl_Position = u_ViewProjection * a_Position;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_Pos;
in flat float v_DiffuseTextureID;
in flat float v_SpecularTextureID;
in flat int v_EntityID;

layout(std140, binding = 1) uniform Camera {
	mat4 u_ViewProjection;
	vec3 u_ViewPosition;
};

uniform int u_PointLightCount;

struct PointLight {
	vec3 position;
	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
#define MAX_POINT_LGIHT_COUNT 4
uniform PointLight pointLights[MAX_POINT_LGIHT_COUNT];

struct Material {
	vec3 albedo;
	float metallic;
	float roughness;
	float ao;
	float useAlbedoMap;
	float useNormalMap;
	float useMetallicMap;
	float useRoughnessMap;
	float useAOMap;
};

uniform Material material;

layout(binding = 0) uniform sampler2D u_Textures[32];

const float PI = 3.14159265359;

// Normal Distribution Function
float DistributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0) + 0.00001;
	denom = PI * denom * denom;
	return num / denom;
}

// Geometry Function
float GeometrySchlickGGX(float NdotV, float roughness) {
	float num = NdotV;
	float denom = NdotV * (1.0 - roughness) + roughness + 0.000001;
	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 getNormalFromMap() {
	vec3 tangentNormal = texture(u_Textures[2], v_TexCoord).xyz * 2.0 - 1.0;
	vec3 Q1 = dFdx(v_Pos);
	vec3 Q2 = dFdy(v_Pos);
	vec2 st1 = dFdx(v_TexCoord);
	vec2 st2 = dFdy(v_TexCoord);
	vec3 N = normalize(v_Normal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);
	return normalize(TBN * tangentNormal);
}

vec3  lightColor  = vec3(300.0, 300.0, 300.0);

void main() {
	vec3 albedo = material.useAlbedoMap == 0.0 ? material.albedo : pow(texture(u_Textures[1], v_TexCoord).rgb, vec3(2.2));
	vec3 N = material.useNormalMap == 0.0 ? normalize(v_Normal) : getNormalFromMap();
	float metallic = material.useMetallicMap == 0.0 ? material.metallic : texture(u_Textures[3], v_TexCoord).r;
	float roughness = material.useRoughnessMap == 0.0 ? material.roughness : texture(u_Textures[4], v_TexCoord).r;
	float ao = material.useAOMap == 0.0 ? material.ao : texture(u_Textures[5], v_TexCoord).r;

	vec3 V = normalize(u_ViewPosition - v_Pos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);

	for (int i = 0; i < u_PointLightCount; ++i) {
		// Calculate per-light radiance
		vec3 L = normalize(pointLights[i].position - v_Pos);
		vec3 H = normalize(V + L);
		float distance = length(pointLights[i].position - v_Pos);
		float attenuation = 1.0 / (distance * distance);
		// vec3 radiance = pointLights[i].color * attenuation;
		vec3 radiance = lightColor * attenuation;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		vec3 specular = numerator / denominator;

		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0)); // HDR
	color = pow(color, vec3(1.0 / 2.2)); // gamma correction

	o_Color = vec4(color, 1.0);
	o_EntityID = v_EntityID;
}