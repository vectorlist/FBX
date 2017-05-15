#version 430

out vec4 outColor;

in vec3 fNormal;
in vec2 fST;
void main()
{
    vec3 lightPos = vec3(10,10,10);
    vec3 uNormal = normalize(fNormal);
    vec3 uLightDir = normalize(lightPos);

    float NdotL = max(dot(uLightDir, uNormal),0.0);
    vec3 diffuse = vec3(0.78,0.82,0.87) * NdotL;
    outColor = vec4(diffuse, 1);
}