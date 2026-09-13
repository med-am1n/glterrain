#version 410 core

in float Height;
in vec2 Tex;
in vec3 Normal;
in vec3 Tangent;
in vec3  Bitangent;
in vec3 FragPos;
out vec4 FragColor;

uniform float gTextureScale = 20.0;

// camera
uniform vec3 viewPos;

uniform vec3 lightDir;

void main()
{
    vec4 brown = vec4(0.914, 0.694, 0.639, 1.0); 

    float diffuse = max(dot(normalize(Normal), normalize(lightDir)), 0.0);

    FragColor = brown * diffuse;
}

