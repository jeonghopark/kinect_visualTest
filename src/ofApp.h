#pragma once

#include "ofMain.h"
//#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxGui.h"
#include "ofxCv.h"

#include "ctmf.h"

#include "DrawPointCloud.hpp"
#include "DrawShape.hpp"

using namespace milton;



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
		
	bool bThreshWithOpenCV;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
	
	ofEasyCam easyCam;
    
    bool bInformation;
    void information();
    
    
    DrawPointCloud drawPointCloud;
    
    bool bContourDraw;
    
    void drawTransImg(ofImage _img);
    
    DrawShape drawShape;
    bool bDrawShape;
    bool bCVDraw;
    
    vector<ofImage> silhoutteImg;
    
    ofVec2f imageRatio;
    int kinectSizeOffSet;
    
    ofxPanel gui;
    ofxColorSlider defaultColor;
    ofxColorSlider backGroundColor;
    ofxIntSlider ctmffilterValue;
    
    bool bDrawGui;
    
    void randomShape();
    
    unsigned char * thresholdedImage;
    
    unsigned char * graypixels;
    unsigned char * medianFiltered;
    
    ofImage medianFilteredResult;
    ofxCv::ContourFinder finder;



};



