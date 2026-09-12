#version 410 core

in float Height;
in vec2 Tex;
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;

// Height spec textures
uniform sampler2D texture_specular0;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;
uniform sampler2D texture_specular4;

uniform float gHeight0 = 1.0;
uniform float gHeight1 = 50.0;
uniform float gHeight2 = 150.0;
uniform float gHeight3 = 170.0;
uniform float gHeight4 = 250.0;

uniform float gTextureScale = 20.0;

// camera
uniform vec3 viewPos;

uniform float shininess;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;

vec4 calculateDirectLight(vec3 baseColor, vec3 specularColor){
	// ambient
    vec3 ambient = dirLight.ambient * baseColor;
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = dirLight.diffuse * diff * baseColor;

	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = dirLight.specular * spec * specularColor.r;
	
  return vec4(ambient + diffuse + specular, 1.0);
}

void main()
{
    vec2 uv = Tex * gTextureScale;
    vec4 color;
    vec3 specularColor;
    if (Height < gHeight0)
    {
        color = texture(texture_diffuse0, uv);
        specularColor = texture(texture_specular0, uv).rgb;
    }
    else if (Height < gHeight1)
    {
        float factor = (Height - gHeight0) / (gHeight1 - gHeight0);
        vec4 color0 = texture(texture_diffuse0, uv);
        vec4 color1 = texture(texture_diffuse1, uv);
        color = mix(color0, color1, clamp(factor, 0.0, 1.0));
        // Specular
        vec3 spec0 = texture(texture_specular0, uv).rgb;
        vec3 spec1 = texture(texture_specular1, uv).rgb;
        specularColor = mix(spec0, spec1, factor);
    }
    else if (Height < gHeight2)
    {
        float factor = (Height - gHeight1) / (gHeight2 - gHeight1);
        vec4 color0 = texture(texture_diffuse1, uv);
        vec4 color1 = texture(texture_diffuse2, uv);
        color = mix(color0, color1, clamp(factor, 0.0, 1.0));
        // Specular
        vec3 spec0 = texture(texture_specular1, uv).rgb;
        vec3 spec1 = texture(texture_specular2, uv).rgb;
        specularColor = mix(spec0, spec1, factor);
    }
    else if (Height < gHeight3)
    {
        float factor = (Height - gHeight2) / (gHeight3 - gHeight2);
        vec4 color0 = texture(texture_diffuse2, uv);
        vec4 color1 = texture(texture_diffuse3, uv);
        color = mix(color0, color1, clamp(factor, 0.0, 1.0));
        // Specular
        vec3 spec0 = texture(texture_specular2, uv).rgb;
        vec3 spec1 = texture(texture_specular3, uv).rgb;
        specularColor = mix(spec0, spec1, factor);
    }
    else if (Height < gHeight4)
    {
        float factor = (Height - gHeight3) / (gHeight4 - gHeight3);
        vec4 color0 = texture(texture_diffuse3, uv);
        vec4 color1 = texture(texture_diffuse4, uv);
        color = mix(color0, color1, clamp(factor, 0.0, 1.0));
        // Specular
        vec3 spec0 = texture(texture_specular3, uv).rgb;
        vec3 spec1 = texture(texture_specular4, uv).rgb;
        specularColor = mix(spec0, spec1, factor);
    }
    else
    {
        color = texture(texture_diffuse4, uv);
        specularColor = texture(texture_specular4, uv).rgb;
    }

      FragColor = calculateDirectLight(color.rgb, specularColor);
}

