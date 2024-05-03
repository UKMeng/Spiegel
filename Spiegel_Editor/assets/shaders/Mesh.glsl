#type vertex
#version 450 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal; // Normal Vector
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

uniform vec4 objectColor;
uniform int dirLightCount;
uniform int pointLightCount;

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

struct DirLight {
	vec3 direction;
	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirLight dirLight;

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;
};

uniform SpotLight spotLight;


struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
}; 

uniform Material material;

layout(binding = 0) uniform sampler2D u_Textures[32];

void main() {
	vec3 norm = normalize(v_Normal);
	vec3 viewDir = normalize(u_ViewPosition - v_Pos);

	// TODO: vec4 adding alpha channel is better
	vec3 diffuseColor = vec3(0.0f);
	vec3 specularColor = vec3(0.0f);
	if(v_DiffuseTextureID != 0.0) diffuseColor = vec3(texture(u_Textures[int(v_DiffuseTextureID)], v_TexCoord));
	if(v_SpecularTextureID != 0.0) specularColor = vec3(texture(u_Textures[int(v_SpecularTextureID)], v_TexCoord));
	//
	/////////
	//// Point Light
	// Calculate Attenuation
	vec3 pointLightResult = vec3(0.0);
	for (int i = 0; i < pointLightCount; i++) {
		PointLight point = pointLights[i];
		float pointDistance = length(point.position - v_Pos);
		float attenuation = 1.0 / (point.constant + point.linear * pointDistance + point.quadratic * (pointDistance * pointDistance));
		// Ambient Lighting
		// vec3 ambient = light.color * light.ambient * material.ambient; 
		vec3 pointLightDir = normalize(point.position - v_Pos);
		vec3 pointAmbient = point.color * point.ambient * diffuseColor; 
		// Diffusion Lighting
		float pointDiff = max(dot(norm, pointLightDir), 0.0);
		// vec3 diffuse = light.color * light.diffuse * (diff * material.diffuse);
		vec3 pointDiffuse = point.color * point.diffuse * pointDiff * diffuseColor;
		// Specular Lighting
		vec3 pointReflectDir = reflect(-pointLightDir, norm);
		float pointSpec = pow(max(dot(viewDir, pointReflectDir), 0.0), material.shininess);
		// vec3 specular = light.color * light.specular * (spec * material.specular);
		vec3 pointSpecular = point.color * point.specular * pointSpec * specularColor;
		// Point Light Result
		pointLightResult += (pointAmbient + pointDiffuse + pointSpecular) * attenuation;
	}
	
	/////////
	// Directional Light
	vec3 dirLightResult = vec3(0.0);
	if (dirLightCount != 0) {
		// Ambient Lighting
		vec3 dirAmbient = dirLight.color * dirLight.ambient * diffuseColor;
		// Diffusion Lighting
		vec3 dirLightDir = normalize(-dirLight.direction);
		float dirDiff = max(dot(norm, dirLightDir), 0.0);
		vec3 dirDiffuse = dirLight.color * dirLight.diffuse * dirDiff * diffuseColor;
		// Specular Lighting
		vec3 dirReflectDir = reflect(-dirLightDir, norm);
		float dirSpec = pow(max(dot(viewDir, dirReflectDir), 0.0), material.shininess);
		vec3 dirSpecular = dirLight.color * dirLight.specular * dirSpec * specularColor;
		// Direction Light Result
		dirLightResult = dirAmbient + dirDiffuse + dirSpecular;
	}
	//
	//
	//////////
	//// Spot Light
	//vec3 spotLightDir = normalize(spotLight.position - v_Pos);
	//float spotDistance = length(spotLight.position - v_Pos);
	//float theta = dot(spotLightDir, normalize(-spotLight.direction));
	//float epsilon = spotLight.cutOff - spotLight.outerCutOff;
	//float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
	//float spotAttenuation = 1.0 / (spotLight.constant + spotLight.linear * spotDistance + spotLight.quadratic * (spotDistance * spotDistance));
	//// Ambient Lighting
	//vec3 spotAmbient = spotLight.color * spotLight.ambient * vec3(texture(u_DiffuseMap, v_TexCoord)); 
	//// Diffusion Lighting
	//float spotDiff = max(dot(norm, spotLightDir), 0.0);
	//vec3 spotDiffuse = spotLight.color * spotLight.diffuse * spotDiff * vec3(texture(u_DiffuseMap, v_TexCoord)) * intensity;
	//// Specular Lighting
	//vec3 spotReflectDir = reflect(-spotLightDir, norm);
	//float spotSpec = pow(max(dot(viewDir, spotReflectDir), 0.0), material.shininess);
	//vec3 spotSpecular = spotLight.color * spotLight.specular * spotSpec * vec3(texture(u_SpecularMap, v_TexCoord)) * intensity;
	//vec3 spotLightResult = (spotAmbient + spotDiffuse + spotSpecular) * spotAttenuation;

	// vec3 lightResult = dirLightResult + pointLightResult + spotLightResult;

	// o_Color = vec4(dirLightResult + pointLightResult, 1.0) * texture(u_DiffuseMap, v_TexCoord);

	
	

	o_Color = vec4(pointLightResult + dirLightResult, 1.0f) * vec4(diffuseColor, 1.0f);
	o_EntityID = v_EntityID;
}