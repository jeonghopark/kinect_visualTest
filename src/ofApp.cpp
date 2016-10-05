#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofBackground(0);

    kinect.setRegistration(true);
    
    kinect.init();
    kinect.open();

    colorImg.allocate(kinect.width, kinect.height);
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    
    nearThreshold = 91;
    farThreshold = 22;
    bThreshWithOpenCV = true;
    
    ofSetFrameRate(60);
    
    angle = 21;
    kinect.setCameraTiltAngle(angle);
    
    drawShape.setup(20);
    
    testManDogImg.load("man_dog.png");
    
    kinectSizeOffSet = 80;
    imageRatio.x = (ofGetWindowSize().x + kinectSizeOffSet) / 640.0;
    imageRatio.y = (ofGetWindowSize().y + kinectSizeOffSet) / 480.0;
    
    
    gui.setup();
    gui.add(defaultColor.setup("Default Color", ofColor(255, 255, 255, 255), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255)));
    gui.add(backGroundColor.setup("Default Color", ofColor(0, 0), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255)));

    
    bDrawGui = false;
    
    bCVDraw = true;
    
}






//--------------------------------------------------------------
void ofApp::update() {
    
    
    randomShape();
    
    
    kinect.update();
    
    if(kinect.isFrameNew()) {
        
        grayImage.setFromPixels(kinect.getDepthPixels());
        
        if(bThreshWithOpenCV) {
            grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(nearThreshold, true);
            grayThreshFar.threshold(farThreshold);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        } else {
            ofPixels & pix = grayImage.getPixels();
            int numPixels = pix.size();
            for(int i = 0; i < numPixels; i++) {
                if(pix[i] < nearThreshold && pix[i] > farThreshold) {
                    pix[i] = 255;
                } else {
                    pix[i] = 0;
                }
            }
        }
        
        grayImage.flagImageChanged();
        
        contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
    }
    
    
    drawShape.update();
    
}




//--------------------------------------------------------------
void ofApp::draw() {
    
    ofBackground(backGroundColor);
    
//    kinect.draw(0, 0, 1024, 768);
    
    if (bCVDraw) {
        drawTransImg(grayImage);
    }
    
    easyCam.begin();
    drawPointCloud.drawPointCloud(kinect, defaultColor);
    drawPointCloud.drawLinesCloud(kinect, defaultColor);
    easyCam.end();
    
    
    if (bDrawShape) {
        drawShape.drawMovingLines(defaultColor);
    }
    
    
    if (contourFinder.blobs.size()>0 && bContourDraw) {
        ofPushMatrix();
        ofPushStyle();
        ofSetRectMode(OF_RECTMODE_CENTER);
        
        ofSetColor(defaultColor);
        
        ofTranslate(0, -kinectSizeOffSet);
        vector<ofxCvBlob> _b = contourFinder.blobs;
        for (int j=0; j<_b.size(); j++) {
            for (int i=0; i<_b[j].pts.size(); i+=20) {
                testManDogImg.draw(_b[j].pts[i] * imageRatio, 80, 88);
            }
        }
        ofPopStyle();
        ofPopMatrix();
    }

    
    if (bInformation){
        information();
    }
    
    
    ofDrawBitmapString(ofToString(ofGetFrameRate(),2), 10, 10);
    
    if (bDrawGui) {
        gui.draw();
    }
    
}






//--------------------------------------------------------------
void ofApp::drawTransImg(ofxCvGrayscaleImage _img){
 
    ofImage _transImg;
    
    _transImg.allocate(640, 480, OF_IMAGE_COLOR_ALPHA);
    
    ofPixels & pix = _img.getPixels();
    ofPixels & pixT = _transImg.getPixels();
    int numPixels = pix.size();
    for(int i = 0; i < numPixels; i++) {
        if(pix[i] == 255) {
            ofColor _c = defaultColor;
            pixT[i*4+0] = _c.r;
            pixT[i*4+1] = _c.g;
            pixT[i*4+2] = _c.b;
            pixT[i*4+3] = _c.a;
        } else {
            pixT[i*4+0] = pix[i];
            pixT[i*4+1] = 255;
            pixT[i*4+2] = 255;
            pixT[i*4+3] = 0;
        }
    }
    
    _transImg.update();
    _transImg.draw(0, -kinectSizeOffSet, ofGetWindowSize().x + kinectSizeOffSet, ofGetWindowSize().y + kinectSizeOffSet);

    
}




//--------------------------------------------------------------
void ofApp::randomShape(){

    if (ofGetFrameNum() % 60 == 0) {
        drawShape.setup(20);
    }
   
}






//--------------------------------------------------------------
void ofApp::information(){
    
    ofSetColor(255, 255, 255);
    stringstream reportStream;
    
    reportStream << "using opencv threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
    << "set near threshold " << nearThreshold << " (press: + -)" << endl
    << "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
    << ", fps: " << ofGetFrameRate() << endl;
    
    if(kinect.hasCamTiltControl()) {
        reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
        << "press 1-5 & 0 to change the led mode" << endl;
    }
    
    ofDrawBitmapString(reportStream.str(), 20, 652);
    
}





//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
    
    switch (key) {
        case 'r':
            drawShape.setup(20);
            break;

        case ' ':
            bThreshWithOpenCV = !bThreshWithOpenCV;
            break;
            
        case'p':
            drawPointCloud.bDrawPointCloud = !drawPointCloud.bDrawPointCloud;
            break;
            
        case '>':
        case '.':
            farThreshold ++;
            if (farThreshold > 255) farThreshold = 255;
            break;
            
        case '<':
        case ',':
            farThreshold --;
            if (farThreshold < 0) farThreshold = 0;
            break;
            
        case '+':
        case '=':
            nearThreshold ++;
            if (nearThreshold > 255) nearThreshold = 255;
            break;
            
        case '-':
            nearThreshold --;
            if (nearThreshold < 0) nearThreshold = 0;
            break;
            
        case 'w':
            kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
            break;
            
        case 'c':
            bContourDraw = !bContourDraw;
            break;

        case 'l':
            drawPointCloud.bLinesPointCloud = !drawPointCloud.bLinesPointCloud;
            break;

        case 's':
            bDrawShape = !bDrawShape;
            break;
            
        case 'i':
            bInformation = !bInformation;
            bDrawGui = !bDrawGui;
            break;

        case 'v':
            bCVDraw = !bCVDraw;
            break;
            
            
        case OF_KEY_UP:
            angle++;
            if(angle>30) angle=30;
            kinect.setCameraTiltAngle(angle);
            break;
            
        case OF_KEY_DOWN:
            angle--;
            if(angle<-30) angle=-30;
            kinect.setCameraTiltAngle(angle);
            break;
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

    imageRatio.x = (ofGetWindowSize().x + kinectSizeOffSet) / 640.0;
    imageRatio.y = (ofGetWindowSize().y + kinectSizeOffSet) / 480.0;
   
}



//--------------------------------------------------------------
void ofApp::exit() {
    kinect.setCameraTiltAngle(0);
    kinect.close();
}


