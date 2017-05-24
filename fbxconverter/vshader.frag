#version 430

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

out vec4 outColor;

in vec3 fDebug;
in vec3 fNormal;
in vec2 fST;
void main()
{
    vec3 lightPos = vec3(10,10,10);
    vec3 uNormal = normalize(fNormal);
    vec3 uLightDir = normalize(lightPos);

    float NdotL = max(dot(uLightDir, uNormal),0.0);
    //vec3 diffuse = vec3(0.78,0.82,0.87) * NdotL;
    vec3 diffuse = fDebug;
    outColor = vec4(diffuse, 1);
}