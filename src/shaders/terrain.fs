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


void main()
{
    FragColor = vec4(normalize(Normal) * 0.5 + 0.5, 1.0);
}

