#version 410 core

in float Height;
in vec2 Tex;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 FragPos;
in float SlopeLight;

out vec4 FragColor;

uniform float gTextureScale;

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
        vec3 brown = vec3(0.914, 0.694, 0.639);
        float diffuse = max(dot(normalize(Normal), normalize(ReversedLightDir)), 0.0);
        diffuse = max(0.4f, diffuse);
        vec3 color = brown * diffuse;
        
        if(fog){
            float distanceToCamera = length(FragPos - viewPos);
            float fogFactor = exp(-0.00050 * distanceToCamera);
            vec3 greyColor = vec3(0.5);
            color = mix(greyColor, color, fogFactor);
          }
        
        FragColor = vec4(color, 1.0);
    }
    else
    { 
        vec3 brown = 0.7 * vec3(0.914, 0.694, 0.639);
        vec3 color = brown * SlopeLight;
        
        if(fog){
            float distanceToCamera = length(FragPos - viewPos);
            float fogFactor = exp(-0.00050 * distanceToCamera);
            vec3 greyColor = vec3(0.5);
            color = mix(greyColor, color, fogFactor);
          }

        FragColor = vec4(color, 1.0);
      }
}

