#pragma once
#include "ofMain.h"

//--

// OPTIONAL

// disable/comment to only use legacy mode for OF 0.11 without PBR features!
#define SURFING__USE_PBR

#define SURFING__DISABLE_LEGACY_LIGHTS

//--

#ifdef SURFING__USE_PBR
	#include "ofxSurfingPBR.h"
#endif

#include "DisplacementSphereMesh.h"
#include "ofxAssimpModelLoader.h"
#include "ofxCameraSaveLoad.h"
#include "ofxGui.h"
#include "ofxSurfingHelpers.h"

#include "ofxBackgroundGradient.h"

#define MAX_MAGNITUDE 1000 // useful for camera distances

//--

class ofxTEST_scene3D : public ofBaseApp {

public:
	ofxTEST_scene3D();
	~ofxTEST_scene3D();

	void setup();
	void draw();
	void exit();
	void keyPressed(int & key);
	void keyPressed(ofKeyEventArgs & eventArgs);
	void keyReleased(ofKeyEventArgs & eventArgs);

	void drawScene();
	void drawSceneComplete();
	void drawBackground();
	void drawGui();

private:
	void update();
	void update(ofEventArgs & args);

private:
	ofEasyCam cam;
	ofxPanel gui;
	ofParameter<bool> bUseCameraInternal;

public:
	ofxBackgroundGradient background;

	void setUseCamera(bool b) {
		bUseCameraInternal = b;
	}
	void setCameraAddonEnable(bool b) {
		ofLogNotice(__FUNCTION__) << b;
		bMouseCamera = b;
		bUseCameraInternal = b;
		if (bMouseCamera) {
			cam.enableMouseInput();
		} else {
			cam.disableMouseInput();
		}
	}

	void setVisibleGui(bool b) {
		bGui = b;
	}
	void setToggleVisibleGui() {
		bGui = !bGui;
	}
	void setGuiPosition(glm::vec2 pos) {
		float x = pos.x;
		float y = pos.y;
		setGuiPosition(x, y);
	}
	void setGuiPosition(float x, float y) {
		gui.setPosition(x, y);
	}

	//--

	// select scene/object
	//--------------------------------------------------------------
	void nextObject() {
		indexTypeObject++;
		if (indexTypeObject > indexTypeObject.getMax()) indexTypeObject = 0;
	}
	//--------------------------------------------------------------
	void setObject(int index) {
		if (indexTypeObject > indexTypeObject.getMax())
			indexTypeObject = indexTypeObject.getMax();
		else if (indexTypeObject < 0)
			indexTypeObject = 0;

		indexTypeObject = index;
	}
	//--------------------------------------------------------------
	void setObjectDefault(int index) {
		if (indexObjectDefault > indexTypeObject.getMax())
			indexTypeObject = indexTypeObject.getMax();
		else if (indexObjectDefault < 0)
			indexObjectDefault = 0;

		indexObjectDefault = index;
	}
	//--------------------------------------------------------------
	void setObjectDefault() {
		indexTypeObject = indexObjectDefault;
	}

public:
	ofParameter<bool> bEnable { "Enable", true };
	ofParameter<bool> bGui;

	ofParameterGroup paramsScene { "ofxTEST_scene3D" };
	ofParameterGroup paramsModes { "Scene" };
	ofParameterGroup paramsCamera { "Camera" };
	ofParameterGroup paramsStyle { "Style" };

private:
	ofParameter<bool> bDrawScene;
	ofParameter<bool> bDrawDebug;
	ofParameter<bool> bResetCamera { "Reset Camera", false };
	ofParameter<bool> bRotate;
	ofParameter<bool> bMouseCamera;

	float _sizePrimWidth;
	float _sizePrimHeight;

	//--

private:
	ofParameter<int> yPos { "yPos", -MAX_MAGNITUDE / 4, -MAX_MAGNITUDE / 2, MAX_MAGNITUDE / 2 };
	ofParameter<int> indexTypeObject { "Type Object", 0, 0, 5 };
	ofParameter<string> nameTypeObject { "Type Name" };
	ofParameter<int> indexObjectDefault { "Object Default Primitive", 0, 0, 5 };
	ofParameter<bool> bFace { "Face", true };
	ofParameter<bool> bWire { "Wire", true };
	ofParameter<float> wireWidth { "Wire Width", 1, 1, 10 };
	ofParameter<void> bWhiteBlack { "White/Black" };
	ofParameter<bool> bFlipColors { "Flip Colors", false };
	ofParameter<float> scaleFace { "Scale Face", 1.f, 0.1f, 2.f };
	ofParameter<float> scaleWire { "Scale Wire", 1.f, 0.1f, 2.f };
	ofParameter<void> bResetScales { "Reset Scales" };
	ofParameter<void> bResetColors { "Reset Colors" };
	ofParameter<bool> bKeys { "Keys", true };

	ofParameter<ofColor> colorA { "color A", ofColor(0, 255), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255) };
	ofParameter<ofColor> colorB { "color B", ofColor(255, 255), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255) };

	//--

private:
	// mesh
	ofMesh world;
	void setupMeshAnimated();
	void drawMeshAnimated();
	void updateMeshAnimated();
	float radius;
	int total;
	float rotMesh;
	float animateZPos;
	bool animation;
	float xoff, yoff;

	//--

	// model
	ofxAssimpModelLoader model;
	vector<ofVboMesh> meshesModel;

	// model mesh
	ofMesh meshForm;

	//--

private:
	// displace mesh
	void setupMeshDisplace();
	void updateMeshDisplace();

	DisplacementSphereMesh displacement;

	// params
	ofParameterGroup paramsDisplace;
	ofParameter<bool> bDisplacementPart1;
	ofParameter<bool> bDisplacementPart2;
	ofParameter<int> mat1;
	ofParameter<int> mat2;
	ofParameter<float> modulate1;
	ofParameter<float> modulate2;
	ofParameter<void> resetDisplace;
	void doResetMeshDisplace();

	std::string path_Displace;

	ofxPanel gui_Displacement;

	//--

	// callback
	void Changed_params(ofAbstractParameter & e);

	//--

	// settings
	ofParameterGroup g { "ofxTEST_scene3D_Settings" };

	//--

public:
	std::string path_GLOBAL_Folder; //top parent folder for all other subfolders

	//--------------------------------------------------------------
	void setPath_GlobalFolder(string folder) {
		ofLogNotice(__FUNCTION__) << folder;
		path_GLOBAL_Folder = folder;
		ofxSurfingHelpers::CheckFolder(folder);
	}

	//--

	std::string path_Settings = "SceneSettings.json";
	std::string path_Camera = "SceneCamera.ini";

	//--

#ifdef SURFING__USE_PBR
	ofxSurfingPBR pbr;
	void renderScene();
#endif

	//--

	// lights
#ifndef SURFING__DISABLE_LEGACY_LIGHTS
	ofLight light;
	void setupLights();
	void beginLights();
	void endLights();
	float xx;
	float yy;
	float zz;
	ofParameter<bool> bLightsAnim { "Lights animate", false };
	ofParameter<bool> bLights { "Lights", true };

	//--

	// helper function
	// disable to power-off or true to enable back. (only acts if lights are enabled)
	void lightHandle(bool b) {
		if (bLights) {
			if (b) {
				light.enable();
				ofEnableLighting();
			} else {
				light.disable();
				ofDisableLighting();
			}
		}
	}
#endif
};
