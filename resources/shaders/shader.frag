#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D tex;

void main(){
    //color = texture(tex, UV);
    color = vec4(vec3(1, 1, 1), 1);
}