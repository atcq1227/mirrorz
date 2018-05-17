#version 150

uniform sampler2DRect previousTexture;

in vec2 vUv;

out vec4 fragColor;

uniform float feed;
uniform float kill;
uniform float biasStrength;
uniform float timestep;
uniform float da;
uniform float db;
uniform float liveliness;


float weights[9] = float[](0.25, 0.5, 0.25, 0.5, -3, 0.5, 0.25, 0.5, 0.25);

vec2 offsets[9] = vec2[](vec2(-1,-1), vec2( 0,-1), vec2( 1,-1),
						 vec2(-1, 0), vec2( 0, 0), vec2( 1, 0),
						 vec2(-1, 1), vec2( 0, 1), vec2( 1, 1) );

float rando(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


// 3x3 kernel convolution
vec4 convolve(sampler2DRect src, vec2 uv) {
	
	vec4 result = vec4(0);
	
	for(int i=0; i<9; i++) {
		result += texture(previousTexture, uv + offsets[i]) * weights[i];
	}
	
	return result;
}


// Reaction-Diffusion
vec4 react(vec4 pixel, vec4 convolution) {
	
	float a = pixel.r;
	float b = pixel.g;
	float c = pixel.b;
	
	float reactionRate = a * b * b;
	
	float du = da * convolution.r // Diffusion term
	- reactionRate
	+ feed * (1.0 - a); // Replenishment term, f is scaled so A <= 1.0
	
	float dv = db * convolution.g // Diffusion term
	+ reactionRate
	- ((feed + kill) - (c * biasStrength)) * b; // Diminishment term, scaled so b >= 0, must not be greater than replenishment
	
	vec2 dst = pixel.rg + timestep * vec2(du, dv);
	return vec4(dst.rg, c, 1.0);
}

void main () {
	
	fragColor = vec4(1,0,1,1);
	
	vec4 pixel = texture(previousTexture, vUv);
	
	vec4 conv = convolve(previousTexture, vUv);
	
	vec4 final = react(pixel, conv);
	
	fragColor.rgb = final.rgb;
	
}
