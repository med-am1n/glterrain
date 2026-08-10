#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos; 


uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 lightPos;  

void main()
{	
	vec4 textureColor = texture(texture1, TexCoord);
	// ambient
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor; 
	
	vec3 result = (ambient + diffuse) * textureColor.rgb;
	FragColor =vec4(result, textureColor.a); // preserving the texture alpha channel
}