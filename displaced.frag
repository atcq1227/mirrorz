#version 150

in vec4 pos;

uniform sampler2DRect depthTex;
uniform vec2 depthTexRes;

uniform sampler2DRect camTex;
uniform vec2 camTexRes;

in vec2 vUv;
out vec4 fragColor;

in vec4 mvPos;

void main() {
    float d0 = texture(depthTex, vUv * depthTexRes).x;
    float d1 = texture(depthTex, vUv * depthTexRes + vec2(1,0)).x;
    float d2 = texture(depthTex, vUv * depthTexRes + vec2(0,1)).x;
    
    float dScl = 200;
    vec3 v1 = vec3(1,0,dScl * (d1 - d0));
    vec3 v2 = vec3(0,1,dScl * (d2 - d0));
    
    vec3 norm = normalize( cross(v1,v2) );
    
    vec3 dir = reflect( norm, normalize(-mvPos.xyz) + texture(depthTex, vUv * depthTexRes).x);
    
    float facingRatio = max(0,dot( vec3(0,1,0), dir));
    
    fragColor = texture(camTex, mix(vUv + norm.xy,vec2(0.5), 0.33) * camTexRes) + (texture(depthTex, vUv * depthTexRes).x) / 1.7;
}
