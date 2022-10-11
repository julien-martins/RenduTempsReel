#version 330 core

in vec3 position;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

uniform mat4 transformation;
uniform mat4 projection;

void main(){
    gl_Position = projection * transformation * vec4(position, 1.0f);

    UV = vertexUV;
}