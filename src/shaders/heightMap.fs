#version 410 core

in float Height;
in vec2 Tex;

out vec4 FragColor;

uniform sampler2D gTextureHeight0;
uniform sampler2D gTextureHeight1;
uniform sampler2D gTextureHeight2;
uniform sampler2D gTextureHeight3;
uniform sampler2D gTextureHeight4;

uniform float gHeight0 = 1.0;
uniform float gHeight1 = 50.0;
uniform float gHeight2 = 150.0;
uniform float gHeight3 = 170.0;
uniform float gHeight4 = 250.0;

uniform float gTextureScale = 20.0;

void main()
{
    vec2 uv = Tex * gTextureScale;
    vec4 color;
    if (Height < gHeight0)
    {
        color = texture(gTextureHeight0, uv);
    }
    else if (Height < gHeight1)
    {
        float factor = (Height - gHeight0) / (gHeight1 - gHeight0);
        vec4 color0 = texture(gTextureHeight0, uv);
        vec4 color1 = texture(gTextureHeight1, uv);
        color = mix(color0, color1, clamp(factor, 0.0, 1.0));
    }
    else if (Height < gHeight2)
    {
        float factor = (Height - gHeight1) / (gHeight2 - gHeight1);
        vec4 color0 = texture(gTextureHeight1, uv);
        vec4 color1 = texture(gTextureHeight2, uv);
        color = mix(color0, color1, clamp(factor, 0.0, 1.0));
    }
    else if (Height < gHeight3)
    {
        float factor = (Height - gHeight2) / (gHeight3 - gHeight2);
        vec4 color0 = texture(gTextureHeight2, uv);
        vec4 color1 = texture(gTextureHeight3, uv);
        color = mix(color0, color1, clamp(factor, 0.0, 1.0));
    }
    else if (Height < gHeight4)
    {
        float factor = (Height - gHeight3) / (gHeight4 - gHeight3);
        vec4 color0 = texture(gTextureHeight3, uv);
        vec4 color1 = texture(gTextureHeight4, uv);
        color = mix(color0, color1, clamp(factor, 0.0, 1.0));
    }
    else
    {
        color = texture(gTextureHeight4, uv);
    }

    FragColor = color;
}

