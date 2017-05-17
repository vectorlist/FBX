#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 st;
layout (location = 3) in vec4 weights;
layout (location = 4) in ivec4 IDs;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;


out vec3 fDebug;
out vec3 fNormal;
out vec2 fST;
void main()
{
    vec4 world = model * vec4(position, 1);
    gl_Position = proj * view * world;
    //fNormal = (model * vec4(normal,0)).xyz;
    fNormal = normal;
    
    fDebug = vec3(weights.xyz);
    fST = st;
}