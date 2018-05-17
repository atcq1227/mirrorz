#version 150

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

in vec4 position;
in vec2 texcoord;

uniform sampler2DRect depthTex;
uniform vec2 depthTexRes;

out vec2 vUv;

out vec4 pos;

out vec4 mvPos;

void main() {
    
    vUv = texcoord;
    
    float displacement = texture(depthTex, texcoord * depthTexRes).x;
    
    pos = position;
    
    mvPos = modelViewMatrix * position + texture(depthTex, vUv);
	
	gl_Position = modelViewProjectionMatrix * pos + displacement;
}
