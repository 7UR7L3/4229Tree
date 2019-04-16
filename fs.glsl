#version 330 core

in vec3 vertexColour;
in vec3 vertexNormal;
in vec4 texinfo;
in vec3 fragPos;

out vec4 FragColor;

uniform sampler2D tex;
uniform sampler2D tex2;

uniform float time;
uniform float texSwitchDepth;

void main()
{
    // FragColor = mix( vec4( vertexColour, 1.0 ), mix( texture( tex, TexCoord ), texture( tex2, TexCoord ), 0.0 ), 0.4 );
    //FragColor = texture( tex, texinfo.xy );
    //FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
    //FragColor = mix( texture( tex, texinfo.xy ), texture( tex2, texinfo.xy ), 0.5 );

/*
    if( texinfo.w <= 27.0 )
    	FragColor = texture( tex, texinfo.xy );
    else
    	FragColor = texture( tex2, texinfo.xy );
*/




	vec3 lightPos = vec3( cos( time * 1.5 ) * 5.0, 4.0, sin( time * 1.5 ) * 5.0 );
	vec3 lightDir = normalize( lightPos - fragPos );
	vec3 diffuse = max( dot( normalize( vertexNormal ), lightDir ), 0.0 ) * vec3( 1.0 );

	FragColor = vec4( diffuse, 1.0 ) * mix( texture( texinfo.w <= texSwitchDepth ? tex : tex2, texinfo.xy ), vec4( 0.1, 0.5, 0.7, 1.0 ), texinfo.w <= texSwitchDepth ? 0 : 0.2 );

if( texinfo.z == -1.0 ) FragColor = vec4( 1.0, 1.0, 1.0, 1.0 );


	//FragColor = vec4( (vertexNormal + 1.0 )/2.0, 1.0 );








/*    	mix(
    		mix( vec4( (vertexNormal + 1.0)/2.0, 1.0 ), texinfo.z == 2.0 ? texture( tex2, texinfo.xy ) : texture( tex, texinfo.xy ), 0.0 ),
    		vec4( vertexColour, 1.0 ),
    		0.0
    	);
*/
    // vec4( vertexColour, 1.0 ), 0.5 ); //vec4( vertexColour, 1.0 );//vec4(1.0f, 0.5f, 0.2f, 1.0f);
}