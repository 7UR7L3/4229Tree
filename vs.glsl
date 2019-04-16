#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texInfo;

out vec3 vertexColour;
out vec3 vertexNormal;
out vec4 texinfo;
out vec3 fragPos;

uniform mat4 objMat, viewCamMat, projMat;

void main()
{
    gl_Position = projMat * viewCamMat * objMat * vec4( aPos, 1.0 );
    vertexColour = mix(
    	mix( vec3( 1.0, 0.0, 0.0 ), vec3( 0.0, 0.0, 1.0 ), 1.0 - 1.0 / (1.0+texInfo.w/4.0) ),
    	vec3( 0.3, 0.3, 0.3 ),
    	0.3 );
    texinfo = texInfo;
    vertexNormal = mat3( transpose( inverse( objMat ) ) ) * normal;
    fragPos = vec3( objMat * vec4( aPos, 1.0 ) );
}