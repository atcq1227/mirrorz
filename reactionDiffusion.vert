#version 150

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

uniform sampler2DRect depth;

in vec4 position;
in vec2 texcoord;

out vec2 vUv;

out vec4 mvPos;

void main() {
	
	vUv = texcoord;
    
    vec2 uv = vUv / vec2(2,1.5);
    
    vec4 depthVal = texture(depth, uv);
    
    mvPos = modelViewMatrix * position;
    
    vec4 pos = position + vec4(0,0,depthVal.x,0);
	
	gl_Position = modelViewProjectionMatrix * pos;
}
