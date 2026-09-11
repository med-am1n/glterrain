#version 410 core

in float Height;
in vec2 Tex;

out vec4 FragColor;

uniform sampler2D grass1Texture;
uniform sampler2D grass2Texture;
uniform sampler2D snowTexture;
void main()
{
    vec2 uv = Tex * 20.0;

    if (Height < 1.0)
    {
        FragColor = texture(grass1Texture, uv);
    }
    else if (Height < 140.0)
    {
        FragColor = texture(grass2Texture, uv);
    }
    else
    {
        FragColor = texture(snowTexture, uv);
    }
}

