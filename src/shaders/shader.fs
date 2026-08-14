#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos; 

struct Material {
	sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position; // no longer necessary when using directional lights.
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	float a;
	float b;
	float c;
};
 

uniform vec3 viewPos;
uniform Material material;
uniform Light light;  

vec4 pointLight(){

	vec3 lightVec=light.position - FragPos;
	float dist = length(lightVec);
	float inten = 1.0f / (light.a * dist * dist + light.b * dist + light.c);
	// ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightVec);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;
	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

	return vec4(ambient + diffuse * inten + specular * inten, 1.0);
}

vec4 directLight(){
	// ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;
	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

	return vec4(ambient + diffuse + specular, 1.0);
}
vec4 spotLight(){

	float outerCone = 0.90f;
	float innerCone = 0.95f;
	// ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;
	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

	float angle = dot(lightDir, normalize(-light.direction));
	float epsilon = innerCone - outerCone;
	float inten = clamp((angle - outerCone)/ epsilon, 0.0f, 1.0f);
	return vec4(ambient + diffuse * inten + specular * inten, 1.0);

}
void main()
{	

	FragColor = pointLight();
}