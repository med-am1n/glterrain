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

uniform float gTextureScale = 1;

uniform vec3 viewPos;
uniform vec3 lightDir;

uniform float heightScale;
uniform float heightOffset;

uniform int visualizationMode;
uniform bool fog;

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
        vec3 brown = 0.7 * vec3(0.65, 0.45, 0.30);
        vec3 grass = vec3(0.20, 0.50, 0.12);
        float diffuse = max(dot(normalize(Normal), normalize(ReversedLightDir)), 0.0);
        diffuse = max(0.4f, diffuse);
        
        float slope = 1.0 - Normal.y;
        float grassToBrown = smoothstep(0.15, 0.45, slope);

        vec3 terrainColor = mix(grass, brown, grassToBrown);
        vec3 color = terrainColor * diffuse; 
        
        if(fog){
            float distanceToCamera = length(FragPos - viewPos);
            float fogFactor = exp(-0.00005 * distanceToCamera);
            vec3 greyColor = vec3(0.769, 0.769, 0.769);
            color = mix(greyColor, color, fogFactor);
          }
        
        FragColor = vec4(color, 1.0);
    }
    else
    { 

        vec2 uv = Tex * gTextureScale;
        
        vec3 color3 = texture(texture_diffuse3, uv).rgb;
        vec3 color4 = texture(texture_diffuse4, uv).rgb;

        float heightBlend = smoothstep(500.0, 700.0, Height);

        vec3 flatColor = mix(color3, color4, heightBlend);


        vec3 steepColor = texture(texture_diffuse1, uv).rgb;
        
        float slope = 1.0 - Normal.y;

        float steepness = smoothstep(0.15, 0.45, slope);

        vec3 color = mix(flatColor, steepColor, steepness) * SlopeLight;

        if(fog){
            float distanceToCamera = length(FragPos - viewPos);
            float fogFactor = exp(-0.00005 * distanceToCamera);
            vec3 greyColor = vec3(0.769, 0.769, 0.769);
            color = mix(greyColor, color, fogFactor);
        }

        FragColor = vec4(color, 1.0);

      }
}

