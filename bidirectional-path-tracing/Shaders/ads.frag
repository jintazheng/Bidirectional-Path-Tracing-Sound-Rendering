#version 330 compatibility

uniform float uKa, uKd, uKs;	// coefficients of each type of lighting
uniform float uShininess;		// specular exponent
uniform vec4  uColor;			// the color of the material

in  vec2  vST;			// texture coords
in  vec3  vN;			// normal vector
in  vec3  vL;			// vector from point to light
in  vec3  vE;			// vector from point to eye

void
main( )
{
	vec3 Normal     = normalize(vN);
	vec3 Light      = normalize(vL);
	vec3 Eye        = normalize(vE);
	
	vec3 color = uColor.rgb;
	
	vec3 ambient = uKa * color;

	float d = max(dot(Normal, Light), 0.);       // only do diffuse if the light can see the point
	vec3 diffuse = uKd * d * color;

	float s = 0.;
	if(dot(Normal, Light) > 0.)	          // only do specular if the light can see the point
	{
		vec3 ref = normalize(reflect(-Light, Normal));
		s = pow(max(dot(Eye, ref), 0.), uShininess);
	}
	vec3 specular = uKs * s * vec3(1., 1., 1.);

	gl_FragColor = vec4( ambient + diffuse + specular,  1.);
}
