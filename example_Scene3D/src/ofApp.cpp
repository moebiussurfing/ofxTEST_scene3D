#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);

	// Set to false to use a local camera here in ofApp 
	// (not the included camera inside the addon) 
	//scene.setUseCamera(false);
}

//--------------------------------------------------------------
void ofApp::draw() {
    scene.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	scene.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::exit() {
}