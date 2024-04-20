#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal; // Normal Vector


layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};

uniform mat4 model;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_Pos;

void main() {
	v_TexCoord = a_TexCoord;
	v_Normal = a_Normal;
	v_Pos = vec3(model * vec4(a_Position, 1.0));
	gl_Position = u_ViewProjection * model * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_Pos;

uniform vec4 objectColor;

uniform vec3 viewPos;

struct Light {
	vec3 position;
	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
}; 

uniform Material material;

layout(binding = 16) uniform sampler2D u_Texture1;
layout(binding = 17) uniform sampler2D u_Texture2;
layout(binding = 18) uniform sampler2D u_DiffuseMap;
layout(binding = 19) uniform sampler2D u_SpecularMap;

void main() {

	// Ambient Lighting
    // vec3 ambient = light.color * light.ambient * material.ambient; 
	vec3 ambient = light.color * light.ambient * vec3(texture(u_DiffuseMap, v_TexCoord)); 

	// Diffusion Lighting
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(light.position - v_Pos);
	float diff = max(dot(norm, lightDir), 0.0);
	// vec3 diffuse = light.color * light.diffuse * (diff * material.diffuse);
	vec3 diffuse = light.color * light.diffuse * diff * vec3(texture(u_DiffuseMap, v_TexCoord));

	// Specular Lighting
	vec3 viewDir = normalize(viewPos - v_Pos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// vec3 specular = light.color * light.specular * (spec * material.specular);
	vec3 specular = light.color * light.specular * spec * vec3(texture(u_SpecularMap, v_TexCoord));

	// vec3 diffuse =
	
	vec3 lightResult = ambient + diffuse + specular;

	o_Color = vec4(lightResult, 1.0) * texture(u_DiffuseMap, v_TexCoord);
	o_EntityID = -1;
}