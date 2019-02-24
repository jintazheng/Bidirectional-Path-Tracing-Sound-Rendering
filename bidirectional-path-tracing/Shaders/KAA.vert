#version 330 compatibility

out vec2  vST;		// texture coords

void
main( )
{
	vST = gl_MultiTexCoord0.st;

	// Output
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}