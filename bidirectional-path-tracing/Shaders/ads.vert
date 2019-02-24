#version 330 compatibility

out vec2  vST;		// texture coords
out vec3  vN;		// normal vector
out vec3  vL;		// vector from point to light
out vec3  vE;		// vector from point to eye

uniform vec4 uLightPosition; // The location of the light in world coordinates
uniform vec4 uEyePosition;   // The location of the eye offset from (0,0,0)

void
main( )
{
	// Pre-compute
	vec4 ECposition = gl_ModelViewMatrix * gl_Vertex;

	// Prepare for fragment shader
	vST = gl_MultiTexCoord0.st;
	vN = normalize(gl_NormalMatrix * gl_Normal);
	vL = (gl_ModelViewMatrix * uLightPosition).xyz - ECposition.xyz;
	vE = vec3(0,0,0) - ECposition.xyz;
	
	// Output
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}