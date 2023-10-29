#include "ofxTEST_scene3D.h"

//--------------------------------------------------------------
ofxTEST_scene3D::ofxTEST_scene3D() {
	ofAddListener(ofEvents().update, this, &ofxTEST_scene3D::update);

	setup();
}

//--------------------------------------------------------------
ofxTEST_scene3D::~ofxTEST_scene3D() {
	ofRemoveListener(ofEvents().update, this, &ofxTEST_scene3D::update);

	exit();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::setup() {
	setPath_GlobalFolder("ofxTEST_scene3D");

	if (1) {
		ofxSurfingHelpers::setThemeDark_ofxGui();
		//ofxSurfingHelpers::setThemeDarkMini_ofxGui();
	}

	//--

	_sizePrimWidth = MAX_MAGNITUDE * 0.2;
	_sizePrimHeight = _sizePrimWidth * 2;
	//_sizePrimHeight = _sizePrimWidth * 1.6180;//aurea proportion

	bGui.set("Scene3D", true);
	bMouseCamera.set("Mouse Camera", true);
	bUseCameraInternal.set("Internal Camera", true);
	bDrawScene.set("Draw Scene", true);
	bRotate.set("Rotate", false);
	bDrawDebug.set("Debug", false);

	bResetScales.setSerializable(false);
	bResetColors.setSerializable(false);
	nameTypeObject.setSerializable(false);

	// scene
	paramsScene.add(bEnable);
	paramsScene.add(bDrawScene);
	paramsScene.add(background.bDrawBg);
	paramsScene.add(background.bGui);
	paramsScene.add(bKeys);

	paramsCamera.add(bMouseCamera);
	paramsCamera.add(bUseCameraInternal);
	paramsCamera.add(bRotate);
	paramsCamera.add(bDrawDebug);
	paramsCamera.add(bResetCamera);
	paramsScene.add(paramsCamera);

	// mode
	paramsModes.add(nameTypeObject);
	paramsModes.add(indexTypeObject);
	paramsModes.add(yPos);

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
	paramsModes.add(bLights);
	paramsModes.add(bLightsAnim);
#endif

	// style
	paramsStyle.add(colorA);
	paramsStyle.add(colorB);
	paramsStyle.add(bFlipColors);
	paramsStyle.add(bWhiteBlack);
	paramsStyle.add(bResetColors);
	paramsStyle.add(bFace);
	paramsStyle.add(scaleFace);
	paramsStyle.add(bWire);
	paramsStyle.add(wireWidth);
	paramsStyle.add(scaleWire);
	paramsStyle.add(bResetScales);

	paramsModes.add(paramsStyle);

	bEnable.setSerializable(false); //force true
	bUseCameraInternal.setSerializable(false); //force true

	//--

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
	setupLights();
#endif

	//--

	// scene

#ifdef SURFING__USE_PBR
	pbr.setup();
	pbr.setCameraPtr(&cam);

	//force
	pbr.bDrawPlane = false;

	// render scene
	callback_t myFunctionDraw = std::bind(&ofxTEST_scene3D::renderScene, this);
	pbr.setFunction_renderScene(myFunctionDraw);
#endif

	//--

	// mesh sphere
	setupMeshAnimated();

	//-

	// mesh displacement
	setupMeshDisplace();

	// prims
	ofSetConeResolution(20, 20, 20);

	//-

	// models path
	string path_models = path_GLOBAL_Folder + "/models/";

	// object
	// we will load only one model!
	// try/search for common filenames...
	bool b = false;
	b = model.loadModel(path_models + "head25k.obj");
	if (b) model.setRotation(0, 180, 0, 1, 0);

	// check files
	if (!b)
		b = model.load(path_models + "basic_form.ply");
	else if (!b)
		b = model.loadModel(path_models + "obj_free_base_female_head.OBJ", 10);
	else if (!b)
		b = model.loadModel(path_models + "head.obj", 10);
	else if (!b)
		b = model.loadModel(path_models + "young_boy_head_obj.obj", 10);

	if (!b)
		ofLogError("ofxTEST_scene3D") << "3d model file not found!";

	// mesh
	//meshForm.load(path_models + "basic_form.ply");
	//meshForm.load(path_models + "head.obj");//not loading

	//--

	// callbacks
	ofAddListener(paramsScene.parameterChangedE(), this, &ofxTEST_scene3D::Changed_params);
	ofAddListener(paramsModes.parameterChangedE(), this, &ofxTEST_scene3D::Changed_params);
	ofAddListener(paramsDisplace.parameterChangedE(), this, &ofxTEST_scene3D::Changed_params);
	//for modulate2 only!

	// gui
	gui.setup("TEST Scene");
	gui.add(paramsScene);
	gui.add(paramsModes);
	gui.setPosition(5, 10);

	// refresh
	auto & gg = gui.getGroup(paramsScene.getName());
	gg.minimize();

	//--

	// settings
	g.add(paramsScene);
	g.add(paramsModes);

	ofxSurfingHelpers::loadGroup(g, path_GLOBAL_Folder + "/" + path_Settings);
	if (bUseCameraInternal) ofxLoadCamera(cam, path_GLOBAL_Folder + "/" + path_Camera);
	ofxSurfingHelpers::loadGroup(paramsDisplace, path_Displace);
}

//--------------------------------------------------------------
void ofxTEST_scene3D::update(ofEventArgs & args) {
	update(); //auto
}

//--------------------------------------------------------------
void ofxTEST_scene3D::update() {

#ifdef SURFING__USE_PBR
	// move gui
	static bool b = false;
	if (!b) {
		b = true;
		float w = 425;
		glm::vec3 p = glm::vec3(ofGetWidth() - w, 10, 0);
		pbr.gui.setPosition(p);
	}
#endif

	//--

	if (!bEnable) return;

	if (indexTypeObject == 5)
		updateMeshAnimated();

	else if (indexTypeObject == 4)
		updateMeshDisplace();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::keyPressed(int & key) {
	if (!bEnable || !bKeys) return;

	// workaround
	// not passed
	bool mod_CONTROL = false;

	if (key == ' ') {
		nextObject();
	}

	// reset "addon local" camera to solve bug that happens sometimes
	else if (key == OF_KEY_BACKSPACE) {
		if (bUseCameraInternal) {
			cam.setupPerspective();
			cam.setVFlip(false);
			ofxSaveCamera(cam, path_GLOBAL_Folder + "/cameraSettings.ini");
		}
	}

	//select object
	else if (key == OF_KEY_F1) {
		if (!mod_CONTROL)
			setObject(0);
		else
			setObjectDefault(0);
	} else if (key == OF_KEY_F2) {
		if (!mod_CONTROL)
			setObject(1);
		else
			setObjectDefault(1);
	} else if (key == OF_KEY_F3) {
		if (!mod_CONTROL)
			setObject(2);
		else
			setObjectDefault(2);
	} else if (key == OF_KEY_F4) {
		if (!mod_CONTROL)
			setObject(3);
		else
			setObjectDefault(3);
	} else if (key == OF_KEY_F5) {
		if (!mod_CONTROL)
			setObject(4);
		else
			setObjectDefault(4);
	} else if (key == OF_KEY_F6) {
		if (!mod_CONTROL)
			setObject(5);
		else
			setObjectDefault(5);
	}

	//// floor
	//else if (key == 'f') {
	//	setToggleVisibleFloor();
	//}

	// next object
	else if (key == 'o') {
		nextObject();
	}
	if (key == 'g') {
		setToggleVisibleGui();
	}
}

//--------------------------------------------------------------
void ofxTEST_scene3D::keyPressed(ofKeyEventArgs & eventArgs) {
	if (!bEnable) return;

	int key = eventArgs.key;
	//bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	//bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	//bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	keyPressed(key);
}

//--------------------------------------------------------------
void ofxTEST_scene3D::keyReleased(ofKeyEventArgs & eventArgs) {
	if (!bEnable) return;

	const int key = eventArgs.key;
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	//select scene object
	if ((key == OF_KEY_F1) || (key == OF_KEY_F2) || (key == OF_KEY_F3) || (key == OF_KEY_F4) || (key == OF_KEY_F5) || (key == OF_KEY_F6))
		setObjectDefault();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::exit() {
	ofRemoveListener(paramsScene.parameterChangedE(), this, &ofxTEST_scene3D::Changed_params);
	ofRemoveListener(paramsModes.parameterChangedE(), this, &ofxTEST_scene3D::Changed_params);
	ofRemoveListener(paramsDisplace.parameterChangedE(), this, &ofxTEST_scene3D::Changed_params);

	ofxSurfingHelpers::saveGroup(g, path_GLOBAL_Folder + "/" + path_Settings);
	if (bUseCameraInternal) ofxSaveCamera(cam, path_GLOBAL_Folder + "/" + path_Camera);
	ofxSurfingHelpers::saveGroup(paramsDisplace, path_Displace);

#ifdef SURFING__USE_PBR
	pbr.exit();
#endif
}

//--------------------------------------------------------------
void ofxTEST_scene3D::draw() {
	if (bEnable) {
		drawBackground();
		drawSceneComplete();
	}

	drawGui();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::drawGui() {
	if (bGui) {
		gui.draw();

		auto p = gui.getShape().getTopRight() + glm::vec2(2, 0);
		background.setGuiPosition(p);
		background.drawGui();

		if (indexTypeObject == 4 || indexTypeObject == 5) {
			auto p = gui.getShape().getBottomLeft() + glm::vec2(0, 6);
			gui_Displacement.setPosition(p);
			gui_Displacement.draw();
		}

		//--

#ifdef SURFING__USE_PBR
		pbr.drawGui();
#endif
	}
}

//--------------------------------------------------------------
void ofxTEST_scene3D::drawBackground() {
	if (!bEnable) return;

	//ofBackgroundGradient(ofColor(40, 40, 40), ofColor(0, 0, 0), OF_GRADIENT_CIRCULAR);

	background.drawBackground();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::drawSceneComplete() {
	if (!bEnable) return;

	ofEnableDepthTest();

	if (bUseCameraInternal) cam.begin();
	{
		//// floor
		////if (bDrawFloor)
		//if (background.bDrawFloor)
		//{
		//	background.drawFloor();
		//}

		//------

		// scene

#ifndef SURFING__USE_PBR
		if (bDrawScene) {
			drawScene();
		}
#endif

#ifdef SURFING__USE_PBR
		if (bDrawScene) {
			pbr.draw();
		}
#endif

		//------

		// debug
		ofDisableDepthTest();

		if (bDrawDebug) {
			ofDrawAxis(50);
		}
	}
	if (bUseCameraInternal) cam.end();
}

#ifdef SURFING__USE_PBR
//--------------------------------------------------------------
void ofxTEST_scene3D::renderScene() {

	// floor plane
	pbr.drawPlane();

	// objects
	pbr.beginMaterial();
	{
		drawScene();
	}
	pbr.endMaterial();
}
#endif

//--------------------------------------------------------------
void ofxTEST_scene3D::drawScene() {
	if (!bEnable) return;

	ofEnableDepthTest();

	ofPushMatrix();
	ofTranslate(0, 250 + yPos.get(), 0); // above floor
	ofPushStyle();

	if (bRotate) ofRotateDeg(ofGetElapsedTimef() * 10, 0, 1, 0);

		//--

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
	if (bLights) beginLights();
#endif

	{
		//--

		// box

		if (indexTypeObject == 0) {
			ofPushMatrix();
			ofPushStyle();
			ofTranslate(0, -_sizePrimWidth * 0.5f);
			ofRotateYDeg(45);

			if (bFace) {
				ofFill();
				ofSetColor(!bFlipColors ? colorA.get() : colorB.get());
				ofPushMatrix();
				{
					ofScale(scaleFace);
					ofDrawBox(0, 0, 0, _sizePrimWidth);
				}
				ofPopMatrix();
			}
			if (bWire) {

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
				lightHandle(false);
#endif
				ofNoFill();
				ofSetColor(bFlipColors ? colorA.get() : colorB.get());
				ofSetLineWidth(wireWidth);
				ofPushMatrix();
				{
					ofScale(scaleWire);
					ofDrawBox(0, 0, 0, _sizePrimWidth);
				}
				ofPopMatrix();

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
				lightHandle(true);
#endif
			}

			ofPopStyle();
			ofPopMatrix();
		}

		//--

		// cone

		else if (indexTypeObject == 1) {
			ofPushMatrix();
			ofPushStyle();

			ofConePrimitive myCone;
			myCone.setOrientation(glm::vec3(0, 0, 180));
			myCone.set(_sizePrimWidth, _sizePrimHeight);
			myCone.setResolution(4, 1, 1);

			if (bFace) {
				ofSetColor(!bFlipColors ? colorA.get() : colorB.get());

				myCone.setScale(scaleFace);
				myCone.draw();
			}
			if (bWire) {

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
				lightHandle(false);
#endif
				ofSetColor(bFlipColors ? colorA.get() : colorB.get());
				ofSetLineWidth(wireWidth);
				myCone.setScale(scaleWire);
				myCone.drawWireframe();

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
				lightHandle(true);
#endif
			}
			ofPopStyle();
			ofPopMatrix();
		}

		//--

		// 3d model

		else if (indexTypeObject == 2) {
			ofPushMatrix();
			ofPushStyle();

			// object
			ofTranslate(0, 20, 40);
			ofRotateXDeg(180);
			ofRotateYDeg(180);
			float _scale = 0.4f; //this mode.3d

			if (bFace) {
				//ofSetColor(255);
				ofSetColor(!bFlipColors ? colorA.get() : colorB.get());
				ofPushMatrix();
				{
					ofScale(_scale * scaleFace);
					model.drawFaces();
				}
				ofPopMatrix();

				////mesh
				//scale = 250;
				//ofScale(scale, scale, scale);
				//ofRotateYDeg(75);
				//meshForm.draw();
			}

			if (bWire) {

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
				lightHandle(false);
#endif
				ofSetColor(bFlipColors ? colorA.get() : colorB.get());
				ofSetLineWidth(wireWidth);
				ofPushMatrix();
				{
					ofScale(_scale * scaleWire);
					model.drawWireframe();
				}
				ofPopMatrix();

				////mesh
				//scale = 250;
				//ofScale(scale, scale, scale);
				//ofRotateYDeg(75);
				//meshForm.draw();

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
				lightHandle(true);
#endif
			}

			ofPopStyle();
			ofPopMatrix();
		}

		//--

		// 3 prims

		else if (indexTypeObject == 3) {
			ofPushMatrix();
			ofPushStyle();
			ofTranslate(0, -50, 0);

			float speedRot = 0.25f;
			//ofRotateY(ofGetFrameNum() * speedRot);
			//ofTranslate(0, 300, 0);
			ofRotateX(ofGetFrameNum() * speedRot);

			if (bFace) {
				ofSetColor(!bFlipColors ? colorA.get() : colorB.get());
				ofFill();
				ofPushMatrix();
				{
					ofScale(scaleFace);
					ofDrawCone(100, 140);
					ofDrawSphere(200, 0, 70);
					ofDrawBox(-200, 0, 0, 80);
				}
				ofPopMatrix();
			}
			if (bWire) {

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
				lightHandle(false);
#endif
				ofSetColor(bFlipColors ? colorA.get() : colorB.get());
				ofSetLineWidth(wireWidth);
				ofNoFill();
				ofPushMatrix();
				{
					ofScale(scaleWire);
					ofDrawCone(100, 140);
					ofDrawSphere(200, 0, 70);
					ofDrawBox(-200, 0, 0, 80);
				}
				ofPopMatrix();

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
				lightHandle(true);
#endif
			}

			ofPopStyle();
			ofPopMatrix();
		}

		//--

		// mesh displace

		else if (indexTypeObject == 4) {
			ofPushStyle();
			ofPushMatrix();
			ofTranslate(0, 15, 0);
			float _scale = 0.7f;

			if (bFace) {
				ofSetColor(!bFlipColors ? colorA.get() : colorB.get());
				ofFill();
				ofPushMatrix();
				{
					ofScale(_scale * scaleFace);
					if (bDisplacementPart1) displacement.draw(false, false);
					if (bDisplacementPart2) ofDrawSphere(0, 0, 180);
				}
				ofPopMatrix();
			}
			if (bWire) {

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
				lightHandle(false);
#endif
				ofNoFill();
				ofSetColor(bFlipColors ? colorA.get() : colorB.get());
				ofSetLineWidth(wireWidth);
				ofPushMatrix();
				{
					ofScale(_scale * scaleFace);
					if (bDisplacementPart1) displacement.draw(true, false);
					if (bDisplacementPart2) ofDrawSphere(0, 0, 180);
				}
				ofPopMatrix();

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
				lightHandle(true);
#endif
			}

			ofPopStyle();
			ofPopMatrix();
		}

		//--

		// mesh animated

		else if (indexTypeObject == 5) {
			drawMeshAnimated();
		}
	}

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
	if (bLights) endLights();
#endif

	ofPopStyle();
	ofPopMatrix();
}

//--

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
//--------------------------------------------------------------
void ofxTEST_scene3D::setupLights() {
	//light.setDirectional();

	//ofFloatColor ambient_color(1., 1.0, 1.0, 1.);
	//ofFloatColor diffuse_color(1., 1., 1.);
	//ofFloatColor specular_color(0, 1., 0);
	//
	//light.setAmbientColor(ambient_color);
	//light.setDiffuseColor(diffuse_color);
	//light.setSpecularColor(specular_color);

	//light.setSpotlight();
	//light.setSpotlightCutOff(200);
	//light.setSpotConcentration(0.1);

	light.setPointLight();
	//light.setAttenuation(1.2,0.01,0.0);
	light.setAttenuation(0.2, 0.01, 0.0);

	//bLightsAnim = true;

	xx = 0.25f * MAX_MAGNITUDE;
	yy = 0.4f * MAX_MAGNITUDE;
	zz = 0.25f * MAX_MAGNITUDE;
}

//--------------------------------------------------------------
void ofxTEST_scene3D::beginLights() {
	if (!bEnable) return;

	if (bLightsAnim) {
		//yy = cos(ofGetElapsedTimef() * 0.4) * 0.5f*MAX_MAGNITUDE;
		xx = cos(ofGetElapsedTimef() * 0.4) * 0.25f * MAX_MAGNITUDE;
		zz = sin(ofGetElapsedTimef() * 0.4) * 0.25f * MAX_MAGNITUDE;
	}

	light.setPosition(xx, yy, zz);
	ofPushStyle();

	if (bDrawDebug) light.draw();

	light.enable();
	ofEnableLighting();

	//scene
	//ofSetColor(255, 255, 255);
	//ofDrawBox(0,0,0,MAX_MAGNITUDE);
}

//--------------------------------------------------------------
void ofxTEST_scene3D::endLights() {
	if (!bEnable) return;

	ofDisableLighting();
	light.disable();

	ofPopStyle();
}
#endif

//--

//--------------------------------------------------------------
void ofxTEST_scene3D::Changed_params(ofAbstractParameter & e) {
	static bool bAttending = false;
	if (bAttending) return;

	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << " : " << e;

	if (name == bMouseCamera.getName() && bUseCameraInternal) {
		if (bMouseCamera) {
			cam.enableMouseInput();
		} else {
			cam.disableMouseInput();
		}
	} else if (name == bResetCamera.getName() && bResetCamera) {
		bResetCamera = false;

		//cam.reset();
		cam.setupPerspective();
		cam.setVFlip(false);
	}

	else if (name == bResetScales.getName()) {
		scaleFace = 1.f;
		scaleWire = 1.f;
		bFace = true;
		bWire = true;
		wireWidth = 1.f;
	}

	else if (name == bResetColors.getName()) {
		bAttending = true;
		colorA.set(ofColor(0, 255));
		colorB.set(ofColor(255, 255));
		bAttending = false;
	} else if (name == bWhiteBlack.getName()) {
		bAttending = true;
		colorA = ofColor(0, 255);
		colorB = ofColor(255, 255);
		bAttending = false;
	}

	else if (name == modulate1.getName()) {
		modulate2 = 2 * modulate1.get();
		//displacement.setMod(2 * modulate1.get());
	} else if (name == modulate2.getName()) {
		displacement.setMod(modulate2);
	} else if (name == resetDisplace.getName()) {
		doResetMeshDisplace();
	}

	else if (name == indexTypeObject.getName()) {
		switch (indexTypeObject) {
		case 0:
			nameTypeObject = "Box";
			break;
		case 1:
			nameTypeObject = "Cone";
			break;
		case 2:
			nameTypeObject = "Model";
			break;
		case 3:
			nameTypeObject = "Three Prims";
			break;
		case 4:
			nameTypeObject = "Mesh Displace";
			break;
		case 5:
			nameTypeObject = "Mesh Animated";
			break;
		}
	}
}

//--

// mesh
//--------------------------------------------------------------
void ofxTEST_scene3D::setupMeshAnimated() {
	radius = MAX_MAGNITUDE * 0.12;
	total = 20;
	rotMesh = 0;
	animateZPos = 1;

	world.setMode(OF_PRIMITIVE_TRIANGLES);

	animation = true;
	for (int i = 0; i < total; i++) {
		float lat = ofMap(i, 0, total - 1, 0.0, PI);

		for (int j = 0; j < total; j++) {
			float lon = ofMap(j, 0, total - 1, 0.0, TWO_PI);

			float x = radius * sin(lat) * cos(lon);
			float y = radius * sin(lat) * sin(lon);
			float z = radius * cos(lat);

			world.addVertex(ofVec3f(x, y, z));
		}
	}

	for (int j = 0; j < total - 1; j++) {
		for (int i = 0; i < total - 1; i++) {

			world.addIndex(i + j * total); // 0
			world.addIndex((i + 1) + j * total); // 1
			world.addIndex(i + (j + 1) * total); // 6

			world.addIndex((i + 1) + j * total); // 1
			world.addIndex((i + 1) + (j + 1) * total); // 7
			world.addIndex(i + (j + 1) * total); // 6
		}
	}
}

//--------------------------------------------------------------
void ofxTEST_scene3D::drawMeshAnimated() {
	if (!bEnable) return;

	ofPushMatrix();
	ofPushStyle();

	ofRotateX(rotMesh);
	ofRotateY(rotMesh);

	// modified sphere from processed world mesh
	for (int i = 0; i < world.getVertices().size(); i++) {
		float x = world.getVertex(i).x;
		float y = world.getVertex(i).y;
		float z = world.getVertex(i).z;

		/*float length = sqrt(x*x+y*y+z*z);
		 x = x / length;
		 y = y / length;
		 z = z / length;
		 //+ sin(ofGetElapsedTimef()) * 20;*/
		float sanim = 1.1 * modulate1 * 1.2;
		//ofDrawCircle(x*sanim, y*sanim, z*sanim,3);

		//-

		// face
		if (bFace) {
			ofSetColor(!bFlipColors ? colorA.get() : colorB.get());
			ofFill();
			ofPushMatrix();
			ofScale(scaleFace);
			{
				ofDrawSphere(x * sanim, y * sanim, z * sanim, 1);
				world.drawFaces();
			}
			ofPopMatrix();
		}

		//if (bWire) {
		//	ofNoFill();
		//	if (bWhiteBlack) ofSetColor(ofColor::black);
		//	else ofSetColor(ofColor::white);
		//	ofDrawSphere(x*sanim, y*sanim, z*sanim, 1);
		//	world.drawWireframe();
		//}
	}

	// wire
	if (bWire) {
#ifndef SURFING__DISABLE_LEGACY_LIGHTS
		lightHandle(false);
#endif

		ofSetColor(bFlipColors ? colorA.get() : colorB.get());
		ofSetLineWidth(wireWidth);
		ofNoFill();
		ofPushMatrix();
		ofScale(scaleWire);
		{
			world.drawWireframe();
			world.drawVertices();
		}
		ofPopMatrix();

#ifndef SURFING__DISABLE_LEGACY_LIGHTS
		lightHandle(true);
#endif
	}
	ofPopStyle();
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::updateMeshAnimated() {
	if (!bEnable) return;
	//ofLogNotice(__FUNCTION__);

	world.clear();

	for (int i = 0; i < total; i++) {
		float lat = ofMap(i, 0, total - 1, 0.0, PI);

		for (int j = 0; j < total; j++) {
			float lon = ofMap(j, 0, total - 1, 0.0, TWO_PI);

			if (animation)
				animateZPos = modulate1 * ofMap(ofNoise(i * ofGetElapsedTimef() * 0.05, j * ofGetElapsedTimef() * 0.05), 0, 1, -10, 10);

			float x = (animateZPos + radius) * sin(lat) * cos(lon);
			float y = (animateZPos + radius) * sin(lat) * sin(lon);
			float z = (animateZPos + radius) * cos(lat);

			world.addVertex(ofVec3f(x, y, z));
			//ofSetColor(255, 0, 0);
		}
	}

	for (int j = 0; j < total - 1; j++) {
		for (int i = 0; i < total - 1; i++) {
			world.addIndex(i + j * total); // 0
			world.addIndex((i + 1) + j * total); // 1
			world.addIndex(i + (j + 1) * total); // 6

			world.addIndex((i + 1) + j * total); // 1
			world.addIndex((i + 1) + (j + 1) * total); // 7
			world.addIndex(i + (j + 1) * total); // 6
		}
	}

	if (animation) {
		rotMesh = rotMesh + 0.01;
	}
}

//--------------------------------------------------------------
void ofxTEST_scene3D::setupMeshDisplace() {
	ofLogNotice(__FUNCTION__);

	//--

	displacement.setup();

	//--

	//TODO: delete all this params from here
	// and use directly from the class

	paramsDisplace.setName("Displacement");

	paramsDisplace.add(modulate1.set("Modulate 1", 0.5f, 0.f, 1.f));
	paramsDisplace.add(modulate2.set("Modulate 2", 1, 0, 2));
	paramsDisplace.add(bDisplacementPart1.set("Section 1", true));
	paramsDisplace.add(bDisplacementPart2.set("section 2", true));
	//paramsDisplace.add(mat1.set("MAT 1", 0, 0, 20));
	//paramsDisplace.add(mat2.set("MAT 2", 0, 0, 20));
	paramsDisplace.add(displacement.params);
	paramsDisplace.add(resetDisplace.set("Reset Displace"));

	gui_Displacement.setup("DisplaceMesh");
	gui_Displacement.add(paramsDisplace);

	path_Displace = path_GLOBAL_Folder + "/SceneDisplace.json";

	doResetMeshDisplace();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::updateMeshDisplace() {
	//ofLogNotice(__FUNCTION__);

	displacement.update();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::doResetMeshDisplace() {
	ofLogNotice(__FUNCTION__);

	modulate1 = 0.5f;
	modulate2 = 1.f;

	bDisplacementPart1 = true;
	bDisplacementPart2 = true;

	displacement.details = 0.06;
	displacement.displace = 0.8;
	displacement.speed = 0.1;
}
