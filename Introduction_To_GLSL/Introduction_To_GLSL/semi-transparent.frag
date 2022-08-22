#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

uniform float fromObject;
uniform float toObject;

void main() {    
    float refractionRatio = fromObject/toObject;
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), refractionRatio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}