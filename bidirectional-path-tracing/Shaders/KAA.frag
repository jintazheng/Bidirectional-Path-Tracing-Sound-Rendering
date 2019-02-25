#version 330 compatibility

in  vec2  vST;			// texture coords

uniform sampler2D uImageUnit; // the texture unit where the texture is located

uniform float uBlurThreshold;
uniform float uBlurAmount;

vec3 LUMCOEFFS = vec3(0.2125, 0.7154, 0.0721);

#define PI 3.14159265359

void
main( )
{	
	//gl_FragColor = texture(uImageUnit, vST);
	//return;

	// detect edges
	vec2   size = textureSize(uImageUnit, 0);
	float sStep = 1. / size.x;
	float tStep = 1. / size.y;
	
	vec3 topLeft  = texture(uImageUnit, vST + vec2(-sStep, tStep)).rgb;
	vec3 topMid   = texture(uImageUnit, vST + vec2(0, tStep)).rgb;
	vec3 topRight = texture(uImageUnit, vST + vec2(sStep, tStep)).rgb;
	vec3 midLeft  = texture(uImageUnit, vST + vec2(-sStep, 0)).rgb;
	vec3 midMid   = texture(uImageUnit, vST + vec2(0, 0)).rgb;
	vec3 midRight = texture(uImageUnit, vST + vec2(sStep, 0)).rgb;
	vec3 lowLeft  = texture(uImageUnit, vST + vec2(-sStep, -tStep)).rgb;
	vec3 lowMid   = texture(uImageUnit, vST + vec2(0, -tStep)).rgb;
	vec3 lowRight = texture(uImageUnit, vST + vec2(sStep, -tStep)).rgb;

	float topLeftCoeff  = dot(topLeft, LUMCOEFFS);
	float topMidCoeff   = dot(topMid, LUMCOEFFS);
	float topRightCoeff = dot(topRight, LUMCOEFFS);
	float midLeftCoeff  = dot(midLeft, LUMCOEFFS);
	float midMidCoeff   = dot(midMid, LUMCOEFFS);
	float midRightCoeff = dot(midRight, LUMCOEFFS);
	float lowLeftCoeff  = dot(lowLeft, LUMCOEFFS);
	float lowMidCoeff   = dot(lowMid, LUMCOEFFS);
	float lowRightCoeff = dot(lowRight, LUMCOEFFS);

	float horizontalDiff = (-topLeftCoeff - (2. * topMidCoeff) - topRightCoeff + lowLeftCoeff + (2. * lowMidCoeff) + lowRightCoeff) / 6.;
	float verticalDiff   = (-topLeftCoeff - (2. * midLeftCoeff) - lowLeftCoeff + topRightCoeff + (2. * midRightCoeff) + lowRightCoeff) / 6.;

	// Calculate the magnitude of the blurring
	float mag = length(vec2(horizontalDiff, verticalDiff));


	if(mag < uBlurThreshold) {
		gl_FragColor = vec4(midMid, 1.);
		return;
	}


	// Determine which direction to blur
	vec3 blur = topMid;  // allow blurring in multiple directions beyonds some threshold
	float blurDir = abs(topMidCoeff - midMidCoeff);
	if(blurDir < abs(midRightCoeff - midMidCoeff)) {
		blur = midRight;
		blurDir = abs(midRightCoeff - midMidCoeff);
	} 
	if(blurDir < abs(midLeftCoeff - midMidCoeff)){
		blur = midLeft;
		blurDir = abs(midLeftCoeff - midMidCoeff);
	}
	if(blurDir < abs(lowMidCoeff - midMidCoeff)){
		blur = lowMid;
		//blurDir = abs(lowMidCoeff - midMidCoeff);
	}

	gl_FragColor = vec4(mix(midMid, blur, mag * uBlurAmount), 1.);
}
