#version 330 compatibility

in  vec2  vST;			// texture coords

uniform sampler2D uImageUnit; // the texture unit where the texture is located


uniform float uContrastThreshold;
uniform float uRelativeThreshold;
uniform float uBlurThreshold;
uniform float uBlurAmount;

vec3 LUMCOEFFS = vec3(0.2125, 0.7154, 0.0721);

float GetLuminance(vec2 offset) {
	return dot(texture(uImageUnit, vST + offset).rgb, LUMCOEFFS);
}

void
main( )
{	
	// detect edges
	vec2  size  = textureSize(uImageUnit, 0);
	float sStep = 1. / size.x;
	float tStep = 1. / size.y;

	// get the luminosity of surrounding texels
	float topLeftLum = GetLuminance(vec2(-sStep, tStep));
	float topMidLum = GetLuminance(vec2(0, tStep));
	float topRightLum = GetLuminance(vec2(sStep, tStep));
	float midLeftLum = GetLuminance(vec2(-sStep, 0));
	float midMidLum = GetLuminance(vec2(0, 0));
	float midRightLum = GetLuminance(vec2(sStep, 0));
	float bottomLeftLum = GetLuminance(vec2(-sStep, -tStep));
	float bottomMidLum = GetLuminance(vec2(0, -tStep));
	float bottomRightLum = GetLuminance(vec2(sStep, -tStep));

	// determine the local contrast
	float highestLum = max(max(max(max(topMidLum, bottomMidLum), midRightLum), midLeftLum), midMidLum);
	float lowestLum  = min(min(min(min(topMidLum, bottomMidLum), midRightLum), midLeftLum), midMidLum);
	float contrast = highestLum - lowestLum;

	// determine if this pixel should be skipped
	if(contrast < uContrastThreshold) {
		gl_FragColor = vec4(texture(uImageUnit, vST).rgb, 1);
		//gl_FragColor = vec4(1, 0, 0, 1);
		return;
	}
	if(contrast < uRelativeThreshold * highestLum) {
		gl_FragColor = vec4(texture(uImageUnit, vST).rgb, 1);
		//gl_FragColor = vec4(0, 1, 0, 1);
		return;
	}

	// calculate a blending factors
	float mag = 2 * (topMidLum + bottomMidLum + midRightLum + midLeftLum);
	mag += topLeftLum + topRightLum + bottomLeftLum + bottomRightLum;
	mag /= 12.;
	mag = abs(mag - midMidLum);
	mag = clamp(mag / contrast, 0., 1.);
	float blend = smoothstep(0., 1., mag);
	float blendFactor = blend * blend;

	// determine which direction the edge is
	float horizontalFilter = abs(topMidLum + bottomMidLum - 2 * midMidLum) * 2;
	horizontalFilter += abs(topLeftLum + bottomLeftLum - 2 * midLeftLum);
	horizontalFilter += abs(topRightLum + bottomRightLum - 2 * midRightLum);
	float verticalFilter = abs(midLeftLum + midRightLum - 2 * midMidLum) * 2;
	verticalFilter += abs(topLeftLum + topRightLum - 2 * topMidLum);
	verticalFilter += abs(bottomLeftLum + bottomRightLum - 2 * bottomMidLum);

	// calculate the direction to blur (NOT WORKING)
	vec2 blendDir;
	if (horizontalFilter < verticalFilter) {
		// Blend horizontally
		float gradientPos = abs(midRightLum - midMidLum);
		float gradientNeg = abs(midLeftLum - midMidLum);
		//blendDir = gradientPos > gradientNeg ?  : 
		if (gradientPos > gradientNeg) {
			blendDir = vec2(sStep, 0);
		} else {
			blendDir = vec2(-sStep, 0);
		}
	} else {
		// Blend vertically
		float gradientPos = abs(topMidLum - midMidLum);
		float gradientNeg = abs(bottomMidLum - midMidLum);
		if (gradientPos > gradientNeg) {
			blendDir = vec2(0, tStep);
		} else {
			blendDir = vec2(0, -tStep);
		}
	}

	vec3 blendCol = texture(uImageUnit, vST + blendDir).rgb;
	gl_FragColor = vec4(mix(texture(uImageUnit, vST).rgb, blendCol, blendFactor), 1);
	return;

	/*const float PI = 3.14159265359;
	const float PI_3_4 = 3. * PI / 4.;
	const float PI_2   = PI / 2.;
	const float PI_4   = PI / 4.;

	// detect edges
	vec2   size = textureSize(uImageUnit, 0);
	float sStep = 1. / size.x;
	float tStep = 1. / size.y;
	
	// Should only detect directly next to...
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

	float horizontalDiff = (-topLeftCoeff - (2. * topMidCoeff) - topRightCoeff + lowLeftCoeff + (2. * lowMidCoeff) + lowRightCoeff);
	float verticalDiff   = (-topLeftCoeff - (2. * midLeftCoeff) - lowLeftCoeff + topRightCoeff + (2. * midRightCoeff) + lowRightCoeff);

	// Calculate the magnitude of the blurring
	float mag = length(vec2(horizontalDiff, verticalDiff));
	float dir = atan(verticalDiff, horizontalDiff);

	// Get the texels to mix from the direction
	float lowLeftMix  = smoothstep(-PI, -PI_3_4, dir)   - smoothstep(-PI_3_4, -PI_2, dir);
	float lowMidMix   = smoothstep(-PI_3_4, -PI_2, dir) - smoothstep(-PI_2, -PI_4, dir);
	float lowRightMix = smoothstep(-PI_2, -PI_4, dir)   - smoothstep(-PI_4, 0, dir);
	float midRightMix = smoothstep(-PI_4, 0, dir)       - smoothstep(0, PI_4, dir);
	float topRightMix = smoothstep(0, PI_4, dir)        - smoothstep(PI_4, PI_2, dir);
	float topMidMix   = smoothstep(PI_4, PI_2, dir)     - smoothstep(PI_2, PI_3_4, dir);
	float topLeftMix  = smoothstep(PI_2, PI_3_4, dir)   - smoothstep(PI_3_4, PI, dir);
	float midLeftMix  = smoothstep(-PI_3_4, -PI, dir)   + smoothstep(PI_3_4, PI, dir);

	//vec3 blurCol = (lowLeftMix * lowLeft) + (lowMidMix * lowMid) + (lowRightMix * lowRight) + (midRightMix * midRight) + (topRightMix * topRight) + (topMidMix * topMid) + (topLeftMix * topLeft) + (midLeftMix * midLeft);
	vec3 blurCol = (lowLeftMix * vec3(1, 0, 0)) + (lowMidMix * vec3(1, 0, 0)) + (lowRightMix * vec3(1, 0, 0)) + (midRightMix * vec3(1, 0, 0)) + (topRightMix * vec3(1, 0, 0)) + (topMidMix * vec3(1, 0, 0)) + (topLeftMix * vec3(1, 0, 0)) + (midLeftMix * vec3(1, 0, 0));

	mag /= 16.;
	
	// Not sure if mag is being correctly calculated
	gl_FragColor = vec4(mix(midMid, blurCol, mag * uBlurAmount), 1.);*/

	/*if(mag < uBlurThreshold) {
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

	gl_FragColor = vec4(mix(midMid, blur, mag * uBlurAmount), 1.);*/
}
