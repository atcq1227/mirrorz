#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    kinect.init();
    
    kinect.open();
	
    displaceShader.load("shaders/displaced");
    
	img.load("shaders/background.jpeg");
    
    texDepth.allocate(640, 480, GL_RGB);
    
    dBlur.allocate(texDepth.getWidth(), texDepth.getHeight());
    
    dBlur.iterations = 4;
    
    plane.set(1, 1, ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::update(){
    
    kinect.update();
    
    if(kinect.isFrameNew()) {
        
        grayDepth.setFromPixels(kinect.getDepthPixels());

        grayDepth.convertToRange(-700, 355);

        texDepth.loadData(grayDepth.getPixels());
        
        texRGB.loadData(img.getPixels());
        
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    dBlur.begin();
    texDepth.draw(0,0, dBlur.getWidth(), dBlur.getHeight());
    dBlur.end();
    
    dBlur.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    ofScale(ofGetWidth(), ofGetHeight());

    auto& dt = dBlur.getTexture();
    
    displaceShader.begin();
    displaceShader.setUniformTexture("depthTex", dt, 0);
    displaceShader.setUniform2f( "depthTexRes", dt.getWidth(), dt.getHeight() );

    displaceShader.setUniformTexture("camTex", texRGB, 1);
    displaceShader.setUniform2f( "camTexRes", texRGB.getWidth(), texRGB.getHeight() );

    plane.draw();

    displaceShader.end();
    
    ofPopMatrix();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
