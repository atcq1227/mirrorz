#version 150

uniform sampler2DRect img;

out vec4 fragColor;

in vec2 vUv;

in vec4 mvPos;

uniform sampler2DRect depth;

uniform float time;

void main() {
    
    vec2 uv = vUv / vec2(2,1.5);
    
    vec2 kinectUV = clamp(uv, vec2(0,0), vec2(512,512));
    
    vec4 depthVal = texture(depth, uv);
    
    vec3 normal = vec3(sin(gl_FragCoord.x / 10), 1.0, sin(gl_FragCoord.y / 10));
    
    vec3 dir = reflect( normalize(normal), normalize(-mvPos.xyz));
    
    vec4 col = texture(img, uv + dir.xy * 100);
    
    float facingRatio = max(0,dot( vec3(0,1,0), dir));
	
    col += pow(1.0 - distance(facingRatio, 0.5), 10.0) * 0.5;
    
    fragColor = col;
}
