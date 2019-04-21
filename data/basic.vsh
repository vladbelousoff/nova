#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntencity;

uniform vec4 LightPosition;
uniform vec3 Kd;
uniform vec3 Ld;

uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjection;

void main()
{
    vec3 tnorm = normalize(VertexNormal);
    vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition, 1.0);
    vec3 s = normalize(vec3(LightPosition - eyeCoords));

    LightIntencity = Ld * Kd * max(dot(s, tnorm), 0.0);

    gl_Position = ModelViewProjection * vec4(VertexPosition, 1.0);
}