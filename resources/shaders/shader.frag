#version 330 core

in vec2 UV;
in vec3 outNormal;
in vec3 outPositionWorld;

out vec4 color;

uniform sampler2D texDiffuse;

uniform vec3 lightPos;
uniform vec3 lightEmission;

void main(){
    float dist2 = pow(distance(outPositionWorld, lightPos), 2);
    vec3 dir = normalize(outPositionWorld - lightPos);
    vec3 norm = normalize(outNormal);

    // final_color => Le / d2 * N . u1 * Albedo
    vec3 final_color = (lightEmission / dist2) * max(dot(norm, dir), 0);

    color = texture(texDiffuse, UV) * vec4(final_color, 1.0f);
    //color = vec4(outNormal, 1.0f);
    //color = vec4(1.0, 0.0, 0.0, 1.0);
}