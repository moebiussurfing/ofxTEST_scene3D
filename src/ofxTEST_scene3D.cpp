#include "ofxTEST_scene3D.h"

//--------------------------------------------------------------
ofxTEST_scene3D::ofxTEST_scene3D()
{
	ofAddListener(ofEvents().update, this, &ofxTEST_scene3D::update);

	setup();
}

//--------------------------------------------------------------
ofxTEST_scene3D::~ofxTEST_scene3D()
{
	ofRemoveListener(ofEvents().update, this, &ofxTEST_scene3D::update);

	exit();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::setup() {
	setPath_GlobalFolder("ofxTEST_scene3D");

	if (0)
	ofxSurfingHelpers::setThemeDark_ofxGui();

	background.setAutoSaveLoad(true);
	//background.setup();
	background.setVisibleGui(true);
	//background.setPosition(glm::vec2(500, 500));

	//-

	_sizePrimWidth = MAX_MAGNITUDE * 0.2;
	_sizePrimHeight = _sizePrimWidth * 2;
	//_sizePrimHeight = _sizePrimWidth * 1.6180;//aurea proportion

	bGui.set("Gui scene3D", true);
	ENABLE_Mouse.set("Mouse", true);
	bUseCameraInternal.set("Use Internal Camera", true);
	SHOW_drawScene.set("Scene", true);
	ENABLE_Rotate.set("Rotate", false);
	SHOW_drawDebug.set("Debug", false);
	SHOW_frustrum.set("Frustrum", false);
	SHOW_floor.set("Bg 3D Editor/Floor", true);
	SHOW_floor.makeReferenceTo(background.bFloor);//link

	bResetScales.setSerializable(false);
	bResetColors.setSerializable(false);

	//helpers
	params_drawHelpers.add(bGui);
	params_drawHelpers.add(background.bGui);
	params_drawHelpers.add(bEnable);
	params_drawHelpers.add(bKeys);
	params_drawHelpers.add(SHOW_drawScene);
	params_drawHelpers.add(SHOW_floor);
	params_drawHelpers.add(ENABLE_Rotate);
	params_drawHelpers.add(SHOW_drawDebug);

	ofParameterGroup _gCam{ "Camera" };
	//_gCam.add(SHOW_frustrum);
	_gCam.add(bUseCameraInternal);
	_gCam.add(ENABLE_Mouse);
	_gCam.add(bResetCamera);
	params_drawHelpers.add(_gCam);

	//renderer
	params_renderMode.add(indexObject);
	//params_renderMode.add(indexObjectDefault);//hide functionality
	params_renderMode.add(modulate);//->only prim types 4-5
	params_renderMode.add(bLights);
	params_renderMode.add(bLightsAnim);
	params_renderMode.add(yOffset);

	ofParameterGroup _params_Style{ "Style" };
	_params_Style.add(colorA);
	_params_Style.add(colorB);
	_params_Style.add(bWhiteBlack);
	_params_Style.add(bFlipColors);
	_params_Style.add(bFace);
	_params_Style.add(scaleFace);
	_params_Style.add(scaleWire);
	_params_Style.add(bWire);
	_params_Style.add(wireWidth);

	params_renderMode.add(_params_Style);

	_params_Style.add(bResetColors);
	_params_Style.add(bResetScales);

	//params_renderMode.add(bResetGlobalTransform);
	//params_renderMode.add(ENABLE_RotateGlobal);

	bEnable.setSerializable(false);//force true
	bUseCameraInternal.setSerializable(false);//force true

	setupLights();

	//-

	//scene

	//mesh sphere
	setupMesh();

	//-

	//mesh displacement
	setupDisplacement();

	//prims
	//ofSetConeResolution(8, 8, 8);
	ofSetConeResolution(20, 20, 20);
	//ofSetConeResolution(40, 40, 40);

	//-

	//models
	string path_models = path_GLOBAL_Folder + "/models/";

	//object
	//model.loadModel(path_models + "obj_free_base_female_head.OBJ", 10);
	//model.loadModel(path_models + "head.obj", 10);
	//model.loadModel(path_models + "young_boy_head_obj.obj", 10);
	model.load(path_models + "basic_form.ply");

	//mesh
	//meshForm.load(path_models + "basic_form.ply");
	//meshForm.load(path_models + "head.obj");//not loading

	//--

	//callbacks
	ofAddListener(params_drawHelpers.parameterChangedE(), this, &ofxTEST_scene3D::Changed_params);
	ofAddListener(params_renderMode.parameterChangedE(), this, &ofxTEST_scene3D::Changed_params);

	//gui
	gui.setup("ofxTEST_scene3D");
	gui.add(params_drawHelpers);
	gui.add(params_renderMode);
	gui.setPosition(5, 10);

	//refresh
	auto &gg = gui.getGroup(params_drawHelpers.getName());
	gg.minimize();

	//-

	//settings
	g.add(params_drawHelpers);
	g.add(params_renderMode);
	ofxSurfingHelpers::loadGroup(g, path_GLOBAL_Folder + "/sceneSettings.xml");

	if (bUseCameraInternal) ofxLoadCamera(cam, path_GLOBAL_Folder + "/cameraSettings.ini");
}

//--------------------------------------------------------------
void ofxTEST_scene3D::update(ofEventArgs & args) {
	update();//auto
}

//--------------------------------------------------------------
void ofxTEST_scene3D::update() {
	if (!bEnable) return;
	//ofLogNotice(__FUNCTION__);

	if (indexObject == 5) updateAnimateMesh();
	else if (indexObject == 4) updateDisplacement();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::keyPressed(int &key) {
	if (!bEnable || !bKeys) return;

	//workaround
	//not passed
	bool mod_CONTROL = false;

	if (key == ' ') {
		nextObject();
	}

	//reset "addon local" camera to solve bug that happens sometimes
	else if (key == OF_KEY_BACKSPACE) {
		if (bUseCameraInternal) {
			cam.setupPerspective();
			cam.setVFlip(false);
			ofxSaveCamera(cam, path_GLOBAL_Folder + "/cameraSettings.ini");
		}
	}

	//select object
	else if (key == OF_KEY_F1) {
		if (!mod_CONTROL) setObject(0);
		else setObjectDefault(0);
	}
	else if (key == OF_KEY_F2) {
		if (!mod_CONTROL) setObject(1);
		else setObjectDefault(1);
	}
	else if (key == OF_KEY_F3) {
		if (!mod_CONTROL) setObject(2);
		else setObjectDefault(2);
	}
	else if (key == OF_KEY_F4) {
		if (!mod_CONTROL) setObject(3);
		else setObjectDefault(3);
	}
	else if (key == OF_KEY_F5) {
		if (!mod_CONTROL) setObject(4);
		else setObjectDefault(4);
	}
	else if (key == OF_KEY_F6) {
		if (!mod_CONTROL) setObject(5);
		else setObjectDefault(5);
	}

	//floor
	else if (key == 'f') {
		setToggleVisibleFloor();
	}

	//next object
	else if (key == 'o') {
		nextObject();
	}
	if (key == 'g') {
		setToggleVisibleGui();
	}
}

//--------------------------------------------------------------
void ofxTEST_scene3D::keyPressed(ofKeyEventArgs &eventArgs) {
	if (!bEnable) return;

	int key = eventArgs.key;
	//bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	//bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	//bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	keyPressed(key);
}

//--------------------------------------------------------------
void ofxTEST_scene3D::keyReleased(ofKeyEventArgs &eventArgs) {
	if (!bEnable) return;

	const int key = eventArgs.key;
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	//select scene object
	if ((key == OF_KEY_F1) ||
		(key == OF_KEY_F2) ||
		(key == OF_KEY_F3) ||
		(key == OF_KEY_F4) ||
		(key == OF_KEY_F5) ||
		(key == OF_KEY_F6))
		setObjectDefault();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::exit() {
	ofRemoveListener(params_drawHelpers.parameterChangedE(), this, &ofxTEST_scene3D::Changed_params);
	ofRemoveListener(params_renderMode.parameterChangedE(), this, &ofxTEST_scene3D::Changed_params);

	ofxSurfingHelpers::saveGroup(g, path_GLOBAL_Folder + "/sceneSettings.xml");
	if (bUseCameraInternal) ofxSaveCamera(cam, path_GLOBAL_Folder + "/cameraSettings.ini");
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

		background.setGuiPosition(gui.getShape().getTopRight() + glm::vec2(2, 0));
		background.drawGui();

		if (indexObject == 4) {
			auto p = gui.getShape().getBottomLeft() + glm::vec2(0, 2);
			guiDisplacement.setPosition(p);
			guiDisplacement.draw();
		}
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
		//floor
		if (SHOW_floor)
		{
			background.drawFloor();
		}

		//------

		//scene
		if (SHOW_drawScene)
		{
			drawScene();
		}

		//------

		//tools
		if (SHOW_frustrum)
		{
			ofPushStyle();
			ofSetColor(ofColor::black);
			cam.drawFrustum();
			ofPopStyle();
		}

		//-

		ofDisableDepthTest();
		if (SHOW_drawDebug)
		{
			ofDrawAxis(50);
		}
	}
	if (bUseCameraInternal) cam.end();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::drawScene()
{
	if (!bEnable) return;

	ofEnableDepthTest();

	ofPushMatrix();
	ofTranslate(0, 250 + yOffset.get(), 0);//above floor
	ofPushStyle();

	if (ENABLE_Rotate) ofRotateDeg(ofGetElapsedTimef() * 10, 0, 1, 0);

	if (bLights) beginLights();
	{
		//-

		//box
		if (indexObject == 0)
		{
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
				lightHandle(false);
				ofNoFill();
				ofSetColor(bFlipColors ? colorA.get() : colorB.get());
				ofSetLineWidth(wireWidth);
				ofPushMatrix();
				{
					ofScale(scaleWire);
					ofDrawBox(0, 0, 0, _sizePrimWidth);
				}
				ofPopMatrix();
				lightHandle(true);
			}

			ofPopStyle();
			ofPopMatrix();
		}

		//-

		//cone
		else if (indexObject == 1)
		{
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
				lightHandle(false);
				ofSetColor(bFlipColors ? colorA.get() : colorB.get());
				ofSetLineWidth(wireWidth);
				myCone.setScale(scaleWire);
				myCone.drawWireframe();
				lightHandle(true);
			}
			ofPopStyle();
			ofPopMatrix();
		}

		//-

		//3d models
		else if (indexObject == 2)
		{
			ofPushMatrix();
			ofPushStyle();

			//object
			ofTranslate(0, 20, 40);
			ofRotateXDeg(180);
			ofRotateYDeg(180);
			float _scale = 0.4f;//this mode.3d

			if (bFace) {
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
				lightHandle(false);
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
				lightHandle(true);
			}

			ofPopStyle();
			ofPopMatrix();
		}

		//-

		//many prims
		else if (indexObject == 3)
		{
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
				lightHandle(false);
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
				lightHandle(true);
			}

			ofPopStyle();
			ofPopMatrix();
		}

		//-

		//displacement form
		else if (indexObject == 4)
		{
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
				lightHandle(false);
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
				lightHandle(true);
			}

			ofPopStyle();
			ofPopMatrix();
		}

		//-

		//mesh deformed sphere
		else if (indexObject == 5)
		{
			drawMesh();
		}
	}
	if (bLights) endLights();

	ofPopStyle();
	ofPopMatrix();
}

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

	xx = 0.25f*MAX_MAGNITUDE;
	yy = 0.4f*MAX_MAGNITUDE;
	zz = 0.25f*MAX_MAGNITUDE;
}

//--------------------------------------------------------------
void ofxTEST_scene3D::beginLights() {
	if (!bEnable) return;

	if (bLightsAnim)
	{
		//yy = cos(ofGetElapsedTimef() * 0.4) * 0.5f*MAX_MAGNITUDE;
		xx = cos(ofGetElapsedTimef() * 0.4) * 0.25f*MAX_MAGNITUDE;
		zz = sin(ofGetElapsedTimef() * 0.4) * 0.25f*MAX_MAGNITUDE;
	}

	light.setPosition(xx, yy, zz);
	ofPushStyle();

	if (SHOW_drawDebug) light.draw();

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

//--------------------------------------------------------------
void ofxTEST_scene3D::Changed_params(ofAbstractParameter &e) {
	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << " : " << e;

	if (name == ENABLE_Mouse.getName() && bUseCameraInternal)
	{
		if (ENABLE_Mouse) {
			cam.enableMouseInput();
		}
		else {
			cam.disableMouseInput();
		}
	}
	if (name == bResetCamera.getName() && bResetCamera)
	{
		bResetCamera = false;

		//cam.reset();
		cam.setupPerspective();
		cam.setVFlip(false);
	}
	if (name == bResetScales.getName() && bResetScales)
	{
		bResetScales = false;
		scaleFace = 1.f;
		scaleWire = 1.f;
		bFace = true;
		bWire = true;
		wireWidth = 1.f;
	}
	if (name == bResetColors.getName() && bResetColors)
	{
		bResetColors = false;
		colorA = ofColor(0, 255);
		colorB = ofColor(255, 255);
	}
	if (name == bWhiteBlack.getName() && bWhiteBlack)
	{
		bWhiteBlack = false;
		colorA = ofColor(0, 255);
		colorB = ofColor(255, 255);
	}
	if (name == modulate.getName())
	{
		mod = 2 * modulate.get();
		//displacement.setMod(2 * modulate.get());
	}
}

////--------------------------------------------------------------
//void ofxTEST_scene3D::loadGroup(ofParameterGroup &g, string path)
//{
//	ofLogNotice(__FUNCTION__) << g.getName() << " to " << path;
//	ofLogVerbose(__FUNCTION__) << "parameters: \n" << g.toString();
//	ofXml settings;
//	settings.load(path);
//	ofDeserialize(settings, g);
//}
//
////--------------------------------------------------------------
//void ofxTEST_scene3D::saveGroup(ofParameterGroup &g, string path)
//{
//	ofLogNotice(__FUNCTION__) << g.getName() << " to " << path;
//	ofLogVerbose(__FUNCTION__) << "parameters: \n" << g.toString();
//	ofXml settings;
//	ofSerialize(settings, g);
//	settings.save(path);
//}

//mesh
//--------------------------------------------------------------
void ofxTEST_scene3D::setupMesh() {
	radius = MAX_MAGNITUDE * 0.12;
	total = 20;
	rotMesh = 0;
	animateZPos = 1;

	world.setMode(OF_PRIMITIVE_TRIANGLES);

	animation = true;
	for (int i = 0; i < total; i++)
	{
		float lat = ofMap(i, 0, total - 1, 0.0, PI);

		for (int j = 0; j < total; j++)
		{
			float lon = ofMap(j, 0, total - 1, 0.0, TWO_PI);

			float x = radius * sin(lat) * cos(lon);
			float y = radius * sin(lat) * sin(lon);
			float z = radius * cos(lat);

			world.addVertex(ofVec3f(x, y, z));
		}
	}

	for (int j = 0; j < total - 1; j++)
	{
		for (int i = 0; i < total - 1; i++)
		{

			world.addIndex(i + j * total);         // 0
			world.addIndex((i + 1) + j * total);     // 1
			world.addIndex(i + (j + 1)*total);     // 6

			world.addIndex((i + 1) + j * total);     // 1
			world.addIndex((i + 1) + (j + 1)*total); // 7
			world.addIndex(i + (j + 1)*total);     // 6
		}
	}
}

//--------------------------------------------------------------
void ofxTEST_scene3D::drawMesh() {
	if (!bEnable) return;

	ofPushMatrix();
	ofPushStyle();

	ofRotateX(rotMesh);
	ofRotateY(rotMesh);

	//modified sphere from processed world mesh
	for (int i = 0; i < world.getVertices().size(); i++)
	{
		float x = world.getVertex(i).x;
		float y = world.getVertex(i).y;
		float z = world.getVertex(i).z;

		/*float length = sqrt(x*x+y*y+z*z);
		 x = x / length;
		 y = y / length;
		 z = z / length;
		 //+ sin(ofGetElapsedTimef()) * 20;*/
		float sanim = 1.1 * modulate * 1.2;
		//ofDrawCircle(x*sanim, y*sanim, z*sanim,3);

		//-

		//face
		if (bFace) {
			ofSetColor(!bFlipColors ? colorA.get() : colorB.get());
			ofFill();
			ofPushMatrix();
			ofScale(scaleFace);
			{
				ofDrawSphere(x*sanim, y*sanim, z*sanim, 1);
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

	//wire
	if (bWire) {
		lightHandle(false);
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
		lightHandle(true);
	}
	ofPopStyle();
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofxTEST_scene3D::updateAnimateMesh() {
	if (!bEnable) return;
	//ofLogNotice(__FUNCTION__);

	world.clear();

	for (int i = 0; i < total; i++)
	{
		float lat = ofMap(i, 0, total - 1, 0.0, PI);

		for (int j = 0; j < total; j++)
		{
			float lon = ofMap(j, 0, total - 1, 0.0, TWO_PI);

			if (animation)
				animateZPos = modulate * ofMap(ofNoise(i*ofGetElapsedTimef()*0.05, j*ofGetElapsedTimef()*0.05), 0, 1, -10, 10);

			float x = (animateZPos + radius) * sin(lat) * cos(lon);
			float y = (animateZPos + radius) * sin(lat) * sin(lon);
			float z = (animateZPos + radius) * cos(lat);

			world.addVertex(ofVec3f(x, y, z));
			//ofSetColor(255, 0, 0);
		}
	}

	for (int j = 0; j < total - 1; j++)
	{
		for (int i = 0; i < total - 1; i++)
		{
			world.addIndex(i + j * total);         // 0
			world.addIndex((i + 1) + j * total);     // 1
			world.addIndex(i + (j + 1)*total);     // 6

			world.addIndex((i + 1) + j * total);     // 1
			world.addIndex((i + 1) + (j + 1)*total); // 7
			world.addIndex(i + (j + 1)*total);     // 6
		}
	}

	if (animation)
	{
		rotMesh = rotMesh + 0.01;
	}
}

//--------------------------------------------------------------
void ofxTEST_scene3D::setupDisplacement() {
	//ofSetConeResolution(40, 40, 40);
	ofLogNotice(__FUNCTION__);

	//--

	displacement.setup();

	//--

	//TODO: delete all this params from here and use directly from the class
	params_Displacement.setName("Displacement");
	params_Displacement.add(mod.set("Modulate", 1, 0, 2));
	params_Displacement.add(bDisplacementPart1.set("Section 1", true));
	params_Displacement.add(bDisplacementPart2.set("section 2", true));
	//params_Displacement.add(mat1.set("MAT 1", 0, 0, 20));
	//params_Displacement.add(mat2.set("MAT 2", 0, 0, 20));
	//params_Displacement.add(bLights.set("LIGHTS", false));
	params_Displacement.add(displacement.params);

	guiDisplacement.setup("DisplacementSphereMesh");
	guiDisplacement.add(params_Displacement);
	//guiDisplacement.setPosition(5, ofGetHeight()-220);
}

//--------------------------------------------------------------
void ofxTEST_scene3D::updateDisplacement() {
	//ofLogNotice(__FUNCTION__);

	displacement.setMod(mod);
	displacement.update();
}
