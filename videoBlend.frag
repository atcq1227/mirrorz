#version 150

uniform sampler2DRect img;

uniform float alpha;

out vec4 fragColor;

in vec2 vUv;

void main() {
	vec4 col = texture(img, vUv);
	
	col.a = alpha;
	
	fragColor = col;
}
