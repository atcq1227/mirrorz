//
//  DirectionalBlur.cpp
//  directionalBlur
//
//  Created by laserstorm on 2/26/18.
//
//

#include "DirectionalBlur.h"


/**
 constructor
 */
DirectionalBlur::DirectionalBlur() :
curr(NULL),
prev(NULL),
weightTex(NULL),
iterations(2),
numPasses(1),
iterationRotation(90),
startRotation(0),
offset(1),
threshold(0.125),
kernelSize(9)
{
	//	make a blur kernel
	kernel = gaussianKernel(kernelSize, 1.0);
	
	//	load the blur shader
	setupBlur();
	
	curr = &fboA;
	prev = &fboB;
	
	//	a small texture for scaling in case we don't pass one
	weightImage.allocate(1, 1, OF_IMAGE_COLOR);
	weightImage.setColor(0, 0, ofColor(255));
	weightImage.update();
	weightTex = &weightImage.getTexture();
};


DirectionalBlur::~DirectionalBlur(){
	fboA.clear();
	fboB.clear();
};


/**
 allocate the fram buffers

 @param w width in pixels
 @param h height in pixels
 */
void DirectionalBlur::allocate(int w, int h) {
	fboA.allocate(w, h, GL_RGBA16F);
	fboB.allocate(w, h, GL_RGBA16F);
}


/**
 reload the fragment shader with a string. keep in mind that the shader is expecting several specific uniforms

 @param frag string
 */
void DirectionalBlur::setFragmentShaderFromString( string frag ) {
	shader.setupShaderFromSource(GL_VERTEX_SHADER, getVertexShader() );
	shader.setupShaderFromSource(GL_FRAGMENT_SHADER, frag );
	shader.linkProgram();
}


/**
 load a fragment shader from file path

 @param filename string
 */
void DirectionalBlur::loadFragmentShader(string filename) {
	
	ofBuffer buffer = ofBufferFromFile(filename);
	
	if(buffer.size()) {
		
		shader.setupShaderFromSource(GL_VERTEX_SHADER, getVertexShader() );
		shader.setupShaderFromSource(GL_FRAGMENT_SHADER, buffer.getText() );
		shader.linkProgram();
		
	} else {
		ofLogError("ofShader") << "setupShaderFromFile(): couldn't load GL_FRAGMENT_SHADER shader " << " from \"" << filename << "\"";
	}
	
}



/**
 basic setup for the the blur

 @param fragShader string
 @param _numPasses int
 @param _offset float - scales the blur radius
 @param _iterations int - how many ping pong rotation iterations we make per pass
 @param _iterationRotation float - how much to rotate the blur between iterations
 */
void DirectionalBlur::setBlur( string fragShader, int _numPasses, float _offset, int _iterations, float _iterationRotation)
{
	setFragmentShaderFromString( fragShader );
	
	numPasses = _numPasses;
	offset = _offset;
	iterationRotation = _iterationRotation;
	iterations = _iterations;
}



/**
 convenience function for setting up a gaussian blur

 @param _numPasses int
 @param _offset _offset float - scales the blur radius
 */
void DirectionalBlur::setupBlur(int _numPasses, float _offset) {
	setBlur( getBlurFragmentShader(), _numPasses, _offset, 2, 90);
}


/**
 convenience function for setting up a bokeh style blur
 
 @param _numPasses int
 @param _offset float - scales the blur radius
 @param _iterations int - how many ping pong rotation iterations we make per pass
 @param _iterationRotation float - how much to rotate the blur between iterations
 */
void DirectionalBlur::setupBokeh(int _numPasses, float _offset, int _iterations, float _iterationRotation) {
	setBlur( getBokehFragmentShader(), _numPasses, _offset, _iterations, _iterationRotation);
}



/**
 begins the current fbo so that we can draw our scene into the blur
 */
void DirectionalBlur::begin() {
	//	fill the first fbo
	curr->begin();
}



/**
 ends the fbo and ping pong blurs the frame buffers
 */
void DirectionalBlur::end() {
	// end the first pass
	curr->end();
	
	ofVec2f dir(0,offset);
	dir.rotate(startRotation);
	
	//	Ping Pong: incrementall blur the fbos
	for(int i=0; i<numPasses * iterations; i++){
		
		//	draw the current fbo to the previous fbo
		prev->begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shader.begin();
		shader.setUniformTexture("tex", curr->getTexture(), 0);
		shader.setUniform2f("texRes", curr->getWidth(), curr->getHeight());
		shader.setUniform2f("dir", dir);
		shader.setUniform1fv("kernel", &kernel[0], kernel.size());
		shader.setUniform1f("threshold", threshold);
		
		if(weightTex){
			shader.setUniformTexture("weightTex", *weightTex, 1);
			shader.setUniform2f("weightTexRes", weightTex->getWidth(), weightTex->getHeight());
		}
		
		//	our fragment shader doesn't use matrix transforms
		//	so draw the plane to the screen in gl screen space(ie. (-1,-1) (1,1) )
		ofDrawRectangle(-1, -1, 2, 2);
		
		shader.end();
		
		prev->end();
		
		//	incrementally rotate our direction
		dir.rotate(iterationRotation);
		
		//	swap the fbo pointers
		swap(curr, prev);
	}
}



/**
 draw the blur

 @param x float
 @param y float
 */
void DirectionalBlur::draw(float x, float y ) {
	draw(x,y,curr->getWidth(), curr->getHeight());
}



/**
 draw the blurred fbo

 @param x float
 @param y float
 @param w float
 @param h float
 */
void DirectionalBlur::draw(float x, float y, float w, float h) {
	curr->draw(x,y,w,h);
}



/**
 returns a 1D Gaussian Kernel

 @param kernel_size int
 @param weight float
 @return vector<float>
 */
vector<float> DirectionalBlur::gaussianKernel( int kernel_size, float weight)
{
	//	ensure the kernel size is odd
	if( kernel_size % 2 == 0){
		kernel_size += 1;
	}
	
	int radius = (kernel_size - 1) / 2;
	vector<float> _kernel( kernel_size );
	
	float twoRadiusSquaredRecip = 1.0 / (2.0 * radius * radius);
	float sqrtTwoPiTimesRadiusRecip = 1.0 / (sqrt(TWO_PI) * radius);
	
	// Create Gaussian Kernel
	int r = -radius;
	float sum = 0, x, v;
	for (int i = 0; i < _kernel.size(); i++)
	{
		v = sqrtTwoPiTimesRadiusRecip * exp(-pow(r * weight, 2) * twoRadiusSquaredRecip);
		_kernel[i] = v;
		
		sum+=v;
		r++;
	}
	
	// normalize
	for (auto& k: _kernel)	k /= sum;
	
	return _kernel;
}



/**
 get a vertex shader as a string

 @return string
 */
string DirectionalBlur::getVertexShader() {
	return R"(
	
	#version 150
	
	in vec4 position;
	out vec2 vUv;
	
	void main() {
		vUv = position.xy * 0.5 + 0.5;
		gl_Position = position;
	}
	
	)";
}



/**
 get a basic directional blur shader

 @param kernelSize itn
 @return string
 */
string DirectionalBlur::getBlurFragmentShader(int kernelSize) {
	
	string frag =  R"(
	
#version 150
	
#define KERNEL_SIZE //KERNEL_SIZE_HERE
	
	out vec4 fragColor;
	
	
	uniform sampler2DRect tex;
	uniform vec2 texRes;
	uniform sampler2DRect weightTex;
	uniform vec2 weightTexRes;
	uniform vec2 dir;
	uniform float kernel[KERNEL_SIZE];
	
	in vec2 vUv;
	
	float radius = (KERNEL_SIZE - 1) / 2;
	
	vec4 blur(sampler2DRect image, vec2 uv, vec2 direction, float scl) {
		
		vec4 color = vec4(0.0);
		
		for(int i=0; i<KERNEL_SIZE; i++) {
			vec2 offset = direction * (-radius + i);
			color += texture(image, uv + offset * scl) * kernel[i];
		}
		
		return color;
	}
	
	
	void main() {
		
		float scl = texture(weightTex, vUv * weightTexRes).x;
		
		vec3 col = blur(tex, vUv * texRes, dir, scl).xyz;
		
		fragColor = vec4(col, 1.0);
		
	}
	
	)";
	
	ofStringReplace(frag, "//KERNEL_SIZE_HERE", ofToString(kernelSize));
	
	return frag;
}



/**
 return a directional bokeh shader

 @param kernelSize int
 @return string
 */
string DirectionalBlur::getBokehFragmentShader(int kernelSize) {
	string frag =  R"(
	
#version 150
	
#define KERNEL_SIZE //KERNEL_SIZE_HERE
	
	out vec4 fragColor;
	
	
	uniform sampler2DRect tex;
	uniform vec2 texRes;
	uniform sampler2DRect weightTex;
	uniform vec2 weightTexRes;
	
	uniform vec2 dir;
	uniform float kernel[KERNEL_SIZE];
	
	in vec2 vUv;
	
	float radius = (KERNEL_SIZE - 1) / 2;
	
	
	float luma(vec3 rgb) {
		return dot(vec3(0.299, 0.587, 0.114), rgb);
	}
	
	vec4 bokeh(sampler2DRect image, vec2 uv, vec2 direction, float scl) {
		
		vec4 color = vec4(0.0);
		
		for(int i=0; i<KERNEL_SIZE; i++) {
			vec2 offset = direction * (-radius + i);
			color = max( texture( image, uv + offset * scl ), color );
		}
		
		return color;
	}
	
	
	void main() {
		
		float scl = texture(weightTex, vUv * weightTexRes).x;
		
		vec3 col = bokeh(tex, vUv * texRes, dir, scl).xyz;
		
		fragColor = vec4(col, 1.0);
		
	}
	
	)";
	
	ofStringReplace(frag, "//KERNEL_SIZE_HERE", ofToString(kernelSize));
	
	return frag;
}



