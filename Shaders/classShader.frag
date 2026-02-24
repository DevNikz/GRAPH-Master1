#version 330 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	vec3 specular;
	float shininess;
};
uniform Material defaultMat;

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;

in vec3 fragPos;
in vec3 normCoord;
in vec2 texCoord;

//Light Properties
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightStrength;

//Ambient Light Properties
uniform float ambientStrength;
uniform vec3 ambientColor;

//Specular
uniform float specularPhong;
uniform float specularStrength;

uniform sampler2D diffuseMap;
uniform vec3 cameraPos;

uniform float constant;
uniform float linear;
uniform float quadratic;

void main()
{
	//Ambient
	vec3 ambient = ambientColor * ambientStrength;

	//Diffuse
	vec3 normal = normalize(normCoord);
	vec3 lightDir = normalize(lightPosition - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 light = lightStrength * lightColor;
	vec3 diffuse = light * diff;

	//Specular
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0.1f), specularPhong);
	vec3 specular = spec * specularStrength * lightColor;

	//Point light attenuation
	float dist = length(lightPosition - fragPos);
	float att = 1.0 / (constant + linear * dist + quadratic * (dist * dist));

	vec3 FinalDiffuse = diff * vec3(texture(diffuseMap, texCoord));

	/*
	float dist = length(lightPos - fragPos);
	float att = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
	*/

	/*
	diffuse *= att;
	specular *= att;
	ambient *= att;
	*/

	vec3 result = FinalDiffuse + ambient + specular;

	FragColor = vec4(result, 1.0f) * texture(diffuseMap, texCoord);
}