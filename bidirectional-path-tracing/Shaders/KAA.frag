#version 330 compatibility

in  vec2  vST;			// texture coords

uniform sampler2D uImageUnit; // the texture unit where the texture is located


uniform float uContrastThreshold;	// The minimum contrast required to blur a pixel
uniform float uRelativeThreshold;	// The minimum luminance difference required based on the max luminance of surrounding pixels
uniform float uBlurAmount;	// Multiplied as a factor in blurring

uniform bool  uEnabled;   // When enabled is false, the shader will not do any work

vec3 LUMCOEFFS = vec3(0.2125, 0.7154, 0.0721);

float GetLuminance(vec2 offset) {
	return dot(texture(uImageUnit, vST + offset).rgb, LUMCOEFFS);
}

void
main( )
{	
	// Early out
	if (!uEnabled) {
		gl_FragColor = vec4(texture(uImageUnit, vST).rgb, 1);
		return;
	}

	// detect edges
	vec2  size  = textureSize(uImageUnit, 0);
	float sStep = 1. / size.x;
	float tStep = 1. / size.y;

	// get the luminosity of surrounding pixels
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
		return;
	}
	if(contrast < uRelativeThreshold * highestLum) {
		gl_FragColor = vec4(texture(uImageUnit, vST).rgb, 1);
		return;
	}

	// calculate a blending factors
	float mag = 2 * (topMidLum + bottomMidLum + midRightLum + midLeftLum);
	mag += topLeftLum + topRightLum + bottomLeftLum + bottomRightLum;
	mag /= 12.;
	mag = abs(mag - midMidLum);
	mag = clamp(mag / contrast, 0., 1.);
	float blend = smoothstep(0., 1., mag);
	float blendFactor = blend * blend * uBlurAmount;

	// determine which direction the edge is
	float horizontalFilter = abs(topMidLum + bottomMidLum - 2 * midMidLum) * 2;
	horizontalFilter += abs(topLeftLum + bottomLeftLum - 2 * midLeftLum);
	horizontalFilter += abs(topRightLum + bottomRightLum - 2 * midRightLum);
	float verticalFilter = abs(midLeftLum + midRightLum - 2 * midMidLum) * 2;
	verticalFilter += abs(topLeftLum + topRightLum - 2 * topMidLum);
	verticalFilter += abs(bottomLeftLum + bottomRightLum - 2 * bottomMidLum);

	// calculate the direction to blur
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
}
