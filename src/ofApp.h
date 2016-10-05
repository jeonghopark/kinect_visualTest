#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxGui.h"

#include "DrawPointCloud.hpp"
#include "DrawShape.hpp"



class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	
	ofxKinect kinect;
		
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage grayThreshNear;
	ofxCvGrayscaleImage grayThreshFar;
	
	ofxCvContourFinder contourFinder;
	
	bool bThreshWithOpenCV;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
	
	ofEasyCam easyCam;
    
    bool bInformation;
    void information();
    
    
    DrawPointCloud drawPointCloud;
    
    bool bContourDraw;
    
    void drawTransImg(ofxCvGrayscaleImage _img);
    
    DrawShape drawShape;
    bool bDrawShape;
    bool bCVDraw;
    
    ofImage testManDogImg;
    
    ofVec2f imageRatio;
    int kinectSizeOffSet;
    
//    ofColor defaultColor;
    
    ofxPanel gui;
    ofxColorSlider defaultColor;
    ofxColorSlider backGroundColor;
    
    bool bDrawGui;
    
    void randomShape();
};
