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

vec4 calculateDirectLight(vec3 baseColor){
	// ambient
    vec3 ambient = dirLight.ambient * baseColor;
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = dirLight.diffuse * diff * baseColor;

  return vec4(ambient + diffuse, 1.0);
}

void main()
{
      vec2 uv = Tex * gTextureScale;

      vec3 N = normalize(Normal);

      float slope = 1.0 - N.y;

      float steepness = smoothstep(0.15, 0.45, slope);

      vec3 flatColor = texture(texture_diffuse2, uv).rgb;
      vec3 steepColor = texture(texture_diffuse1, uv).rgb;

      vec3 color = mix(flatColor, steepColor, steepness);

      FragColor = calculateDirectLight(color.rgb);

}

