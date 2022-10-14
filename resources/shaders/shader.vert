#version 330 core

in vec3 position;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

out vec2 UV;
out vec3 outNormal;
out vec3 outPositionWorld;
out vec3 outLightPos;

uniform mat4 transformation;
uniform mat4 projection;

void main(){
    vec4 positionWorld = projection * transformation * vec4(position, 1.0f);
    gl_Position = positionWorld;

    UV = vertexUV;
    outNormal = (transformation * vec4(normal, 1.0f)).xyz;
    outPositionWorld = (transformation * vec4(position, 1.0f)).xyz;
}