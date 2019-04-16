#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 viewCamMat, projMat;

void main()
{
    gl_Position = projMat * viewCamMat * vec4( aPos.x, aPos.y, aPos.z, 1.0 );
    texCoords = aPos;
}