//
//  DirectionalBlur.h
//  directionalBlur
//
//  Created by lars berg on 2/26/18.
//
//

#pragma once

#include "ofMain.h"

class DirectionalBlur :: public FinalSketchBases {
public:
	DirectionalBlur();
	~DirectionalBlur();
	
	//	allocate the frame buffers
	void allocate(int w, int h);
	
	//	begin the fbo and draw our content that we'd like to blur.
	//	end will blur or content to our current fbo
	void begin();
	void end();
	
	// draw the blurred content to the screen
	void draw(float x=0, float y=0 );
	void draw(float x, float y, float w, float h);
	
	//	functions for loading our fragment shaders
	void setFragmentShaderFromString( string frag );
	void loadFragmentShader(string filename);
	
	
	//	default shader setup
	void setBlur( string fragShader, int _numPasses, float _offset, int _iterations, float _iterationRotation);
	void setupBlur(int _numPasses = 1, float _offset = 1);
	void setupBokeh(int _numPasses = 1, float _offset = 1, int _iterations=3, float _iterationRotation=60);
    
    int getWidth() {
        return fboA.getWidth();
    }
    
    int getHeight() {
        return fboA.getHeight();
    }
	
    ofTexture& getTexture() {
        return curr->getTexture();
    }
    
	//	functions that generate our default shaders
	string getVertexShader();
	string getBlurFragmentShader( int kernelSize = 9 );
	string getBokehFragmentShader( int kernelSize = 9 );
	string getBloomFragmentShader( int kernelSize = 9 );
	
	
	// returns 1D gaussian kernel
	vector<float> gaussianKernel( int radius, float weight);
	

	// attributes
	ofShader shader;
	ofTexture* weightTex;
	ofImage weightImage;
	ofFbo fboA, fboB, *curr, *prev;
	int iterations, numPasses, kernelSize;
	float iterationRotation, startRotation, offset, threshold;
	vector<float> kernel;
	
};
