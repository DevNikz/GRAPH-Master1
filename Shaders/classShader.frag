#version 330 core

/*
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
in vec3 FragPos;
in vec3 Normal;
uniform float ambientStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 color;
*/

out vec4 FragColor;
uniform sampler2D tex0;
in vec2 texCoord;

void main()
{
	/*
	vec3 Ia = lightColor * ambientStrength;

	vec3 N = normalize(Normal);
	vec3 Lm = normalize(lightPos - FragPos);
	vec3 Id = lightColor * max(dot(N, Lm), 0.0);

	vec3 V = normalize(viewPos - FragPos);
	vec3 Rm = reflect(-Lm, N);
	vec3 Is = lightColor * pow(max(dot(Rm, V), 0.0), 256) * 0.5;

	vec3 finalColor = (Ia + Id + Is) * color;
	FragColor = vec4(finalColor, 1.0);
	*/
	//FragColor = vec4(0.7f, 0, 0, 1f);
	FragColor = texture(tex0, texCoord);
}