/*

    TODO:

    fix too many shadows error

    pbr fails on model
        convert to mesh first
        try more models
        add file loader dialog
        fix cul/face
        try import model into forum example

    save internal camera mode

    add presets manager and randomizer

*/

//----

#pragma once
#include "ofMain.h"

#include "ofxTEST_scene3D.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void draw();
	void exit();
	void keyPressed(int key);

	ofxTEST_scene3D scene;
};
