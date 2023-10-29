#pragma once

#include "ofMain.h"

#include "ofxTEST_scene3D.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void draw();
    void exit();
    void keyPressed(int key);

	ofxTEST_scene3D scene;
};


