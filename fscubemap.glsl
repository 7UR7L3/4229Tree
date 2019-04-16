#version 330 core

in vec3 texCoords;

out vec4 FragColor;

uniform samplerCube cubemap;
uniform vec3 colour;

void main()
{
	FragColor = mix( vec4( colour, 1.0 ), texture( cubemap, texCoords ), .5 );
}