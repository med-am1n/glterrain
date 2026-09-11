#version 410 core

in float Height;
in vec2 Tex;

out vec4 FragColor;

uniform sampler2D grassTexture;

void main()
{
    FragColor = texture(grassTexture, Tex * 20.0);
}

