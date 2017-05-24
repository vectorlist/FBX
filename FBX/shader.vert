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
    mat4 bone[MAX_BONES];
}ubo;

out vec3 fDebug;
out vec3 fNormal;
out vec2 fST;
void main()
{
    //mat4 boneTransform = ubo.bone[boneID[0]] * boneWeight[0];
    //boneTransform += ubo.bone[boneID[1]] * boneWeight[1];
    //boneTransform += ubo.bone[boneID[2]] * boneWeight[2];
    //boneTransform += ubo.bone[boneID[3]] * boneWeight[3];   

    //Link Matrix inverse * local * global 

    mat4 boneTransform = ubo.bone[boneID[0]] * boneWeight[0]
    + ubo.bone[boneID[1]] * boneWeight[1]
    + ubo.bone[boneID[2]] * boneWeight[2]
    + ubo.bone[boneID[3]] * boneWeight[3];
    
    vec4 world = model * boneTransform * vec4(position, 1);
    vec4 skin = boneTransform *  world;
    gl_Position = proj * view * world;

    fNormal = normal;
    fDebug = vec3(normal.xyz);
    fST = st;
}