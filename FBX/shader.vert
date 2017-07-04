#version 430

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 st;
layout (location = 3) in vec4 boneWeight;
layout (location = 4) in ivec4 boneID;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

#define MAX_BONES 64

layout (location = 0) uniform UBO
{
    mat4 current[MAX_BONES];
    mat4 next[MAX_BONES];
    float weight;
    bool isAnimation;
}ubo;

out vec3 fDebug;
out vec3 fNormal;
out vec2 fST;

mat4 blendCrossFade(mat4 current, mat4 next, float weight)
{
    float nextWeight = 1.0f - weight;
    return (current * nextWeight) + (next * weight);
}

void main()
{
    mat4 current = ubo.current[boneID[0]] * boneWeight[0];
        current += ubo.current[boneID[1]] * boneWeight[1];
        current += ubo.current[boneID[2]] * boneWeight[2];
        current += ubo.current[boneID[3]] * boneWeight[3];

    mat4 next = ubo.next[boneID[0]] * boneWeight[0];
        next += ubo.next[boneID[1]] * boneWeight[1];
        next += ubo.next[boneID[2]] * boneWeight[2];
        next += ubo.next[boneID[3]] * boneWeight[3]; 

    mat4 skin = blendCrossFade(current, next, ubo.weight);
    
    if(!ubo.isAnimation)
    {
        skin = mat4(1);
    }
    
    vec4 skinWorld = model * skin * vec4(position, 1);
    vec4 world = model * vec4(position, 1);
    gl_Position = proj * view * skinWorld;

    fNormal = normal;
    fDebug = vec3(normal.xyz);
    fST = st;
}