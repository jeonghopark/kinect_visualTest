#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void drawPointCloud();
	
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
	bool bDrawPointCloud;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
	
	ofEasyCam easyCam;
    
    
    bool bInformation;
    void information();
    
    
    bool bContourDraw;
    
    
};
