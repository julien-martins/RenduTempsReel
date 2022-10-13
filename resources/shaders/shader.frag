#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

void main(){
    color = mix(texture(tex2, UV), texture(tex1, UV), 0.5);
    //color = mix(texture(tex1, UV), texture(tex2, UV), texture(tex3, UV));
    //color = vec4(vec3(1, 1, 1), 1);
}