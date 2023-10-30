/*

    TODO:

    fix constructor/setup/exit workflow

    fix too many shadows error

    pbr fails on model
        convert to mesh first

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
