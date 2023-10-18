#pragma once

#include "ofMain.h"

#include "ofxGui.h"
#include "ofxCameraSaveLoad.h"
#include "ofxAssimpModelLoader.h"
#include "DisplacementSphereMesh.h"
#include "ofxSurfingHelpers.h"

#include "ofxBackgroundGradient.h"

#define MAX_MAGNITUDE 1000

class ofxTEST_scene3D : public ofBaseApp {

public:
	ofxTEST_scene3D();
	~ofxTEST_scene3D();
	
	ofParameter<bool>bEnable{ "Enable", true };

	void setup();
	void draw();
	void exit();
	void keyPressed(int &key);
	void keyPressed(ofKeyEventArgs &eventArgs);
	void keyReleased(ofKeyEventArgs &eventArgs);

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
		ENABLE_Mouse = b;
		bUseCameraInternal = b;
		if (ENABLE_Mouse) {
			cam.enableMouseInput();
		}
		else {
			cam.disableMouseInput();
		}
	}
	void setVisibleGui(bool b) {
		bGui = b;
		//background.setVisibleGui(bGui);
	}
	void setVisibleFloor(bool b) {
		SHOW_floor = b;
	}
	void setToggleVisibleFloor() {
		SHOW_floor = !SHOW_floor;
	}
	void setToggleVisibleGui() {
		bGui = !bGui;
		//background.setVisibleGui(bGui);
	}
	void setGuiPosition(glm::vec2 pos) {
		float x = pos.x;
		float y = pos.y;
		setGuiPosition(x, y);
	}
	void setGuiPosition(float x, float y) {
		gui.setPosition(x, y);
		//background.setGuiPosition(glm::vec2(x + 205, y));
	}

	void lightHandle(bool b) {//disable to poweroff or true to enable back. (only acts if lights are enabled)
		if (bLights) {
			if (b) {
				light.enable();
				ofEnableLighting();
			}
			else {
				light.disable();
				ofDisableLighting();
			}
		}
	}

	//select scene/object
	void nextObject() {
		indexObject++;
		if (indexObject > indexObject.getMax()) indexObject = 0;
	}
	void setObject(int index) {
		if (indexObject > indexObject.getMax()) indexObject = indexObject.getMax();
		else if (indexObject < 0) indexObject = 0;

		indexObject = index;
	}
	void setObjectDefault(int index) {
		if (indexObjectDefault > indexObject.getMax()) indexObject = indexObject.getMax();
		else if (indexObjectDefault < 0) indexObjectDefault = 0;

		indexObjectDefault = index;
	}
	void setObjectDefault() {
		indexObject = indexObjectDefault;
	}

public:
	ofParameter<bool> bGui;

private:
	ofParameterGroup params_drawHelpers{ "ofxTEST_scene3D" };
	ofParameter<bool> SHOW_drawScene;
	ofParameter<bool> SHOW_drawDebug;
	ofParameter<bool>bResetCamera{ "Reset Camera", false };
	ofParameter<bool> SHOW_frustrum;
	ofParameter<bool> SHOW_floor;
	ofParameter<bool> ENABLE_Rotate;
	ofParameter<bool> ENABLE_Mouse;
	//ofParameter<bool> SHOW_gridSphere;

	float _sizePrimWidth;
	float _sizePrimHeight;

	//-

	//light
	ofLight light;
	void setupLights();
	void beginLights();
	void endLights();
	float xx;
	float yy;
	float zz;
	ofParameter<bool> bLightsAnim{ "Lights animate", false };

	//-

	ofParameterGroup params_renderMode{ "RENDER MODE" };
	ofParameter<bool>bKeys{ "Keys", true };
	ofParameter<bool>bLights{ "Lights", true };
	ofParameter<int>yOffset{ "yOffset", -MAX_MAGNITUDE / 4, -MAX_MAGNITUDE / 2,MAX_MAGNITUDE / 2 };
	ofParameter<int>indexObject{ "Object Primitive", 0,0,5 };
	ofParameter<int>indexObjectDefault{ "Object Default Primitive", 0,0,5 };
	ofParameter<bool>bFace{ "Face", true };
	ofParameter<bool>bWire{ "Wire", true };
	ofParameter<float>wireWidth{ "Wire Width", 1, 1, 10 };
	ofParameter<bool>bWhiteBlack{ "White/Black", false };
	ofParameter<bool>bFlipColors{ "Swap Colors", false };
	ofParameter<float>scaleFace{ "Scale Face", 1.f, 0.1f, 2.f };
	ofParameter<float>scaleWire{ "Scale Wire", 1.f, 0.1f, 2.f };
	ofParameter<bool>bResetScales{ "Reset Scales", false };
	ofParameter<bool>bResetColors{ "Reset Colors", false };
	ofParameter<float>modulate{ "Modulate", 0.5f, 0.f, 1.f };

	ofParameter<ofColor>colorA{ "color A", ofColor(0,255),ofColor(0,0,0,0),ofColor(255,255,255,255) };
	ofParameter<ofColor>colorB{ "color B", ofColor(255,255),ofColor(0,0,0,0),ofColor(255,255,255,255) };

	//-

	//mesh
	ofMesh world;
	void setupMesh();
	void drawMesh();
	void updateAnimateMesh();
	float radius;
	int total;
	float rotMesh;
	float animateZPos;
	bool animation;
	float xoff, yoff;

	//--

	//model
	ofxAssimpModelLoader model;
	//model mesh
	ofMesh meshForm;

	//--

	//displacement example
	void setupDisplacement();
	void updateDisplacement();

	DisplacementSphereMesh displacement;

	//params
	ofParameterGroup params_Displacement;
	ofParameter<bool> bDisplacementPart1;
	ofParameter<bool> bDisplacementPart2;
	ofParameter<int> mat1;
	ofParameter<int> mat2;
	ofParameter<float> mod;

	ofxPanel guiDisplacement;

	//-

	//callback
	void Changed_params(ofAbstractParameter &e);

	//-

	//settings
	ofParameterGroup g{ "ofxTEST_scene3D_Settings" };

	//-

public:
	std::string path_GLOBAL_Folder;//top parent folder for all other subfolders
	//--------------------------------------------------------------
	void setPath_GlobalFolder(string folder)
	{
		ofLogNotice(__FUNCTION__) << folder;
		path_GLOBAL_Folder = folder;
		ofxSurfingHelpers::CheckFolder(folder);
	}
};


