#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
#ifdef OF_APP_DEFINED_ofxSurfingHelpers
	ofxSurfingHelpers::setWindowTitleAsProjectName();
	ofxSurfingHelpers::setMonitorsLayout(-1, false, true);
#endif

	// Set to false to use a local camera here in ofApp 
	// (not the included camera inside the addon) 
	//scene.setUseCamera(false);
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofClear(ofColor::blue);//will be overwritten when addon enabled!

    scene.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	scene.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::exit() {
}