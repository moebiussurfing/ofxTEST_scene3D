# ofxTEST_scene3D

## Overview
**ofxTEST_scene3D** is an **openFrameworks** helper add-on to easy-and-fast draw a basic 3D Scene/sandbox to test other add-ons.

## Screenshot
![image](/readme_images/Capture.PNG?raw=true "image")

## Features
- Includes configurable background system.
- Draws different primitives including 3D models.
- Coloreable and Black and White fill colors with wireframe.
- Basic ligth.
- Floor grid and dark styled background.
- Auto rotator.
- EasyCam with debug draws.
- Auto Save/Load settings.

## Usage

# ofApp.h
```.cpp
#include "ofxTEST_scene3D.h"

ofxTEST_scene3D scene;
```

# ofApp.cpp
```.cpp
void ofApp::draw() {
	scene.draw();
}
```

## Dependencies
[ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)  
[ofxBackgroundGradient](https://github.com/moebiussurfing/ofxBackgroundGradient)  

- _ofxCameraSaveLoad_ already bundled on _/libs_.

## Tested Systems
- **Windows10** / **VS2017** / **OF ~0.11**

## Author
Addon by **@moebiusSurfing**  
*(ManuMolina). 2020.*

## License
*MIT License.*