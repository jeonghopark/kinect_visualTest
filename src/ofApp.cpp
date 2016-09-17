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
    
    nearThreshold = 230;
    farThreshold = 70;
    bThreshWithOpenCV = true;
    
    ofSetFrameRate(60);
    
    angle = 0;
    kinect.setCameraTiltAngle(angle);
    
}




//--------------------------------------------------------------
void ofApp::update() {
    
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
    
}




//--------------------------------------------------------------
void ofApp::draw() {
    
    ofSetColor(255, 255, 255);
    
    
    easyCam.begin();
    drawPointCloud.drawPointCloud(kinect);
    drawPointCloud.drawLinesCloud(kinect);
    easyCam.end();
    
    
    drawTransImg(grayImage);

    
    if (bContourDraw) {
        contourFinder.draw(0, 0);
    }
    
    
    if (contourFinder.blobs.size()>0 && bContourDraw) {
        ofPushStyle();
        ofNoFill();
        vector<ofxCvBlob> _b = contourFinder.blobs;
        for (int j=0; j<_b.size(); j++) {
            vector<ofPoint> _p = _b[j].pts;
            for (int i=0; i<_p.size(); i++) {
                ofDrawCircle(_p[i], 10);
            }
        }
        ofPopStyle();
    }

    
    if (bInformation){
        information();
    }
    
    
    ofDrawBitmapString(ofToString(ofGetFrameRate(),2), 10, 10);
    
    
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
            pixT[i*4+0] = 255;
            pixT[i*4+1] = 0;
            pixT[i*4+2] = 0;
            pixT[i*4+3] = 255;
        } else {
            pixT[i*4+0] = pix[i];
            pixT[i*4+1] = 255;
            pixT[i*4+2] = 255;
            pixT[i*4+3] = 0;
        }
    }
    
    _transImg.update();
    _transImg.draw(0, 0);

    
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
void ofApp::exit() {
    kinect.setCameraTiltAngle(0);
    kinect.close();
}



//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
    switch (key) {
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
            
        case '1':
            kinect.setLed(ofxKinect::LED_GREEN);
            break;
            
        case '2':
            kinect.setLed(ofxKinect::LED_YELLOW);
            break;
            
        case '3':
            kinect.setLed(ofxKinect::LED_RED);
            break;
            
        case '4':
            kinect.setLed(ofxKinect::LED_BLINK_GREEN);
            break;
            
        case '5':
            kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
            break;
            
        case '0':
            kinect.setLed(ofxKinect::LED_OFF);
            break;
            
        case 'i':
            bInformation = !bInformation;
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
    
}
