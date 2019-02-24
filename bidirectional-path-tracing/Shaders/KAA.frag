#version 330 compatibility

in  vec2  vST;			// texture coords

uniform sampler2D uImageUnit; // the texture unit where the texture is located

vec3 LUMCOEFFS = vec3(0.2125, 0.7154, 0.0721);

void
main( )
{	
	// detect edges
	vec2   size = textureSize(uImageUnit, 0);
	float sStep = 1. / size.x;
	float tStep = 1. / size.y;
	
	float topLeft  = dot(texture(uImageUnit, vST + vec2(-sStep, tStep)).rgb, LUMCOEFFS);
	float topMid   = dot(texture(uImageUnit, vST + vec2(0,      tStep)).rgb, LUMCOEFFS);
	float topRight = dot(texture(uImageUnit, vST + vec2(sStep,  tStep)).rgb, LUMCOEFFS);
	float midLeft  = dot(texture(uImageUnit, vST + vec2(-sStep,     0)).rgb, LUMCOEFFS);
	float midMid   = dot(texture(uImageUnit, vST + vec2(0,          0)).rgb, LUMCOEFFS);
	float midRight = dot(texture(uImageUnit, vST + vec2(sStep,      0)).rgb, LUMCOEFFS);
	float lowLeft  = dot(texture(uImageUnit, vST + vec2(-sStep,-tStep)).rgb, LUMCOEFFS);
	float lowMid   = dot(texture(uImageUnit, vST + vec2(0,     -tStep)).rgb, LUMCOEFFS);
	float lowRight = dot(texture(uImageUnit, vST + vec2(sStep, -tStep)).rgb, LUMCOEFFS);

	float horizontalDiff = -topLeft - (2. * topMid) - topRight + lowLeft + (2. * lowMid) + lowRight;
	float verticalDiff   = -topLeft - (2. * midLeft) - lowLeft + topRight + (2. * midRight) + lowRight;

	float mag = length(vec2(horizontalDiff, verticalDiff));

	vec3 target = vec3(mag, mag, 0);

	gl_FragColor = vec4(mix(texture(uImageUnit, vST).rgb, target, 0.5f), 1.);
}
