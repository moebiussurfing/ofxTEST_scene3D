//#include "ofMain.h"
//#include "ofApp.h"
//
////========================================================================
//int main( ){
//	ofSetupOpenGL(1920,1080,OF_WINDOW);			// <-------- setup the GL context
//
//	// this kicks off the running of my app
//	// can be OF_WINDOW or OF_FULLSCREEN
//	// pass in width and height too:
//	ofRunApp(new ofApp());
//
//}

#include "ofApp.h"
#include "ofMain.h"

//========================================================================
int main() {

#ifdef TARGET_OPENGLES
	ofGLESWindowSettings settings;
	settings.glesVersion = 2;
#else
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
#endif

	auto window = ofCreateWindow(settings);

	ofRunApp(window, make_shared<ofApp>());
	ofRunMainLoop();
}