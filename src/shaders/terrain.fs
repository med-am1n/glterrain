#version 410 core

in float Height;
in vec2 Tex;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 FragPos;
in float SlopeLight;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;

out vec4 FragColor;

uniform float gTextureScale = 20;

uniform vec3 viewPos;
uniform vec3 lightDir;

uniform float heightScale;
uniform float heightOffset;

uniform int visualizationMode;
uniform bool fog;
uniform vec3 fogDensity;

// Based on code originally written by Inigo Quilez, © 2015.
// More info here: https://iquilezles.org/articles/texturerepetition
float sumVec3(vec3 v)
{
    return v.x + v.y + v.z;
}

float greyNosie(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

vec3 textureNoTile(sampler2D samp, vec2 uv)
{
    vec2 x = uv; 
    
    float k = greyNosie(0.005 * x);
    
    float index = k * 8.0;
    float i = floor(index);
    float f = fract(index);

    vec2 offa = sin(vec2(3.0, 7.0) * (i + 0.0));
    vec2 offb = sin(vec2(3.0, 7.0) * (i + 1.0));

    vec2 dx = dFdx(uv);
    vec2 dy = dFdy(uv);

    vec3 cola = textureGrad(samp, uv + offa, dx, dy).rgb;
    vec3 colb = textureGrad(samp, uv + offb, dx, dy).rgb;

    return mix(cola, colb, smoothstep(0.2, 0.8, f - 0.1 * sumVec3(cola - colb)));
}

void main()
{ 
    vec3 ReversedLightDir = -lightDir;
    ReversedLightDir.y = 0.0;
    
    if (visualizationMode == 0)
    {
        float h = (Height - (heightOffset - heightScale)) / (2.0 * heightScale);

        h = clamp(h, 0.0, 1.0);

        FragColor = vec4(h, h, h, 1.0);
    }
    else if (visualizationMode == 1)
    {
        vec3 N = normalize(Normal);

        FragColor = vec4(N * 0.5 + 0.5, 1.0);
    }
    else if (visualizationMode == 2)
    {
        vec2 uv = Tex * gTextureScale;
        
        vec3 color3 = textureNoTile(texture_diffuse2, uv).rgb;
        vec3 color4 = textureNoTile(texture_diffuse4, uv).rgb;

        float heightBlend = smoothstep(500.0, 700.0, Height);

        vec3 flatColor = mix(color3, color4, heightBlend);

        vec3 steepColor = textureNoTile(texture_diffuse0, uv).rgb;
        

        float slope = 1.0 - Normal.y;

        float steepness = smoothstep(0.15, 0.45, slope);
        
        float diffuse = max(dot(normalize(Normal), normalize(ReversedLightDir)), 0.0);

        vec3 color = mix(flatColor, steepColor, steepness) * (0.25 + 0.75 * diffuse);

        
        if(fog){
            float distanceToCamera = length(FragPos - viewPos);
            vec3 fogFactor = exp(-fogDensity * distanceToCamera);
            vec3 fogColor = vec3(0.769, 0.769, 0.769);
            color = mix(fogColor, color, fogFactor);
          }
        
        FragColor = vec4(color, 1.0);
    }
    else
    { 

        vec2 uv = Tex * gTextureScale;
        
        vec3 color3 = textureNoTile(texture_diffuse2, uv).rgb;
        vec3 color4 = textureNoTile(texture_diffuse4, uv).rgb;

        float heightBlend = smoothstep(500.0, 700.0, Height);

        vec3 flatColor = mix(color3, color4, heightBlend);


        vec3 steepColor = textureNoTile(texture_diffuse0, uv).rgb;
        
        float slope = 1.0 - Normal.y;

        float steepness = smoothstep(0.15, 0.45, slope);

        vec3 color = mix(flatColor, steepColor, steepness) * SlopeLight;

        if (fog) {
            float distanceToCamera = length(FragPos - viewPos);

            vec3 fogFactor = exp(-fogDensity * distanceToCamera);

            vec3 fogColor = vec3(0.769, 0.769, 0.769);

            color = mix(fogColor, color, fogFactor);
        }


        FragColor = vec4(color, 1.0);

      }
}

