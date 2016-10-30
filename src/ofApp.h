#pragma once

#include "ofMain.h"
//#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxGui.h"
#include "ofxCv.h"
#include "ofxVideoRecorder.h"
#include "ofxPSBlend.h"

#include "ctmf.h"

#include "DrawPointCloud.hpp"
#include "DrawShape.hpp"


#define DEBUG_VIDEO



using namespace milton;


class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	    
	void keyPressed(int key);
    void keyReleased(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	
#ifdef DEBUG_VIDEO
    ofVideoPlayer player;
#else
    ofxKinect kinect;
#endif
		
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
    void drawTransImgColor(ofImage _img, ofColor _c);
    void drawTransShadowImg(ofImage _img);


    ofImage drawTransColorImage(ofImage _img, ofColor _c);

    
    DrawShape drawShape;
    bool bDrawShape;
    bool bCVDraw;
    
    vector<ofImage> silhoutteImg;
    
    ofVec2f imageRatio;
    
    ofxPanel gui;
    ofxLabel fpsView;
    ofxColorSlider shapeColor;
    ofxColorSlider backGroundColor;
    ofxColorSlider smallFigureColor;
    ofxIntSlider ctmffilterValue;
    ofxIntSlider threshold;
    ofxToggle invertColor;
    ofxToggle delayBackground;
    
    bool bDrawGui;
    
    void randomShape();
    
    unsigned char * thresholdedImage;
    
    unsigned char * graypixels;
    unsigned char * medianFiltered;
    
    ofImage medianFilteredResult;
    ofxCv::ContourFinder finder;

    void changeColorButton(bool & t);

    
    ofxVideoRecorder    vidRecorder;
//    ofSoundStream       soundStream;
    bool bRecording;
    string fileName;
    string fileExt;
    
    void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);

    
    ofxPSBlend psBlend;
    int blendMode;

    
    ofFbo mainFbo;
    
    void drawSmallFigure();
    
};
