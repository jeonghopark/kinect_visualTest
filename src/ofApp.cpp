#include "ofApp.h"


using namespace ofxCv;
using namespace cv;


//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofBackground(0);
    
    kinect.setRegistration(true);
    
    kinect.init();
    kinect.open();
    kinect.enableDepthNearValueWhite(false);
    
    nearThreshold = 54;
    farThreshold = 0;
    bThreshWithOpenCV = true;
    
    ofSetFrameRate(60);
    
    angle = 13;
    kinect.setCameraTiltAngle(angle);
    
    drawShape.setup(20);
    
    kinectSizeOffSet = 80;
    imageRatio.x = (ofGetWindowSize().x + kinectSizeOffSet) / 640.0;
    imageRatio.y = (ofGetWindowSize().y + kinectSizeOffSet) / 480.0;
    
    
    gui.setup();
    gui.add(ctmffilterValue.setup("Filter", 15, 1, 30));
    gui.add(defaultColor.setup("Background Color", ofColor(255, 255), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255)));
    gui.add(backGroundColor.setup("Shape Color", ofColor(0, 255), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255)));
    
    
    
    bDrawGui = false;
    bCVDraw = true;
    bContourDraw = true;
    
    
    ofDirectory _dir;
    _dir.listDir("silhoutteImg/");
    _dir.allowExt("jpg");
    _dir.sort();
    silhoutteImg.resize(_dir.size());
    for(int i = 0; i<silhoutteImg.size(); i++){
        silhoutteImg[i].load(_dir.getPath(i));
    }
    
    
    graypixels = new unsigned char[640*480];
    medianFiltered = new unsigned char[640*480];
    medianFilteredResult.allocate(640, 480, OF_IMAGE_GRAYSCALE);
    
    

}






//--------------------------------------------------------------
void ofApp::update() {
    
    kinect.update();
    
    if(kinect.isFrameNew()) {
        
        unsigned char * data  = kinect.getDepthPixels().getData();
        unsigned char * pix = kinect.getDepthPixels().getData();
        for (int i = 0; i < 640*480; i++){
            if(pix[i] < nearThreshold && pix[i] > farThreshold) {
                pix[i] = 255;
            } else {
                pix[i] = 0;
            }
        }

        ctmf(pix, medianFiltered, 640, 480, 640, 640, ctmffilterValue, 1);
        
        medianFilteredResult.setFromPixels(medianFiltered, 640, 480, OF_IMAGE_GRAYSCALE);
        medianFilteredResult.mirror(false, true);
        
        finder.setSortBySize(true);
        finder.setThreshold(100);
        finder.setMinAreaRadius(10);
        finder.setMaxAreaRadius(200);
        finder.setThreshold(127);
        finder.findContours(medianFilteredResult);
        finder.setFindHoles(false);

        //        finder.findContours(medianFilteredResult);
        
        //        grayImage.flagImageChanged();
        
        //        grayImage.setFromPixels(medianFiltered, 640, 480);
        
        //        ofPixels & pix = grayImage.getPixels();
        //        int numPixels = pix.size();
        //        for(int i = 0; i < numPixels; i++) {
        //            if(pix[i] < nearThreshold && pix[i] > farThreshold) {
        //                pix[i] = 255;
        //            } else {
        //                pix[i] = 0;
        //            }
        //        }
        //
        //
        //        contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);

        
        
        
    }
    
    
    //    drawShape.update();
    
}




//--------------------------------------------------------------
void ofApp::draw() {
    
    ofBackground(backGroundColor);
    
    if (bCVDraw) {
        drawTransImg(medianFilteredResult);
    }
    
    easyCam.begin();
    drawPointCloud.drawPointCloud(kinect, defaultColor);
    drawPointCloud.drawLinesCloud(kinect, defaultColor);
    easyCam.end();

    //    if (bDrawShape) {
    //        drawShape.drawMovingLines(defaultColor);
    //    }
    
//    medianFilteredResult.draw(0, 0);
    
    
    if (finder.size() > 0){
        ofPushMatrix();
        ofTranslate(0, 0);
        for (int j=0; j<finder.size(); j++) {
            //            finder.getPolyline(j).draw();
        }
        ofPopMatrix();
    }
    
    
    if (finder.size()>0 && bContourDraw) {
        ofPushMatrix();
        ofPushStyle();
        ofSetRectMode(OF_RECTMODE_CENTER);
        
        ofSetColor(defaultColor);
        
        ofTranslate(0, -kinectSizeOffSet);
        
        for (int j=0; j<finder.size(); j++) {
            ofPushMatrix();
            ofScale(imageRatio.x, imageRatio.y, 0);
            finder.getPolyline(j).draw();
            ofPopMatrix();
            
            ofPolyline _polyLines = finder.getPolyline(j);
            vector<glm::vec3> _v = _polyLines.getVertices();
            

            // FIXME: error up Points
            for (int i=0; i<_v.size()-1; i++) {
                float _step = 50;
                float _ratioSize = 0.25;
                ofPoint _v1 = _polyLines.getPointAtPercent(i/_step) * imageRatio;
                ofPoint _v2 = _polyLines.getPointAtPercent((i+1)/_step) * imageRatio;
            
                ofPushMatrix();
                ofPoint _diffV = _v2 -_v1;
                float _degree = atan2(_diffV.y, _diffV.x);
                ofTranslate(_v1.x, _v1.y, 0);
                ofRotateZDeg(ofRadToDeg(_degree) + 180);
                int _index = i % silhoutteImg.size();
                ofTranslate(-silhoutteImg[_index].getWidth() * _ratioSize * 0.5, -silhoutteImg[_index].getHeight() * _ratioSize * 0.5, 0);
                silhoutteImg[_index].draw(0, 0, 0, silhoutteImg[_index].getWidth() * _ratioSize, silhoutteImg[_index].getHeight() * _ratioSize);
                ofPopMatrix();
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
    
    
//    ofImage _test;
//    _test.setFromPixels(recordBuff, 160, 120, OF_IMAGE_COLOR_ALPHA);
//    _test.draw(100, 100);
    
}





//--------------------------------------------------------------
void ofApp::drawTransImg(ofImage _img){

    ofImage _transImg;

    _transImg.allocate(640, 480, OF_IMAGE_COLOR_ALPHA);

    ofPixels & pix = _img.getPixels();
    ofPixels & pixT = _transImg.getPixels();
    int numPixels = pix.size();
    for(int i = 0; i < numPixels; i++) {
        if(pix[i] > 200) {
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
    
    ofPushStyle();
    
    ofSetColor(255);
    stringstream reportStream;
    
    reportStream << "using opencv threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
    << "set near threshold " << nearThreshold << " (press: + -)" << endl
    << "set far threshold " << farThreshold << ", fps: " << ofGetFrameRate() << endl;
    
    if(kinect.hasCamTiltControl()) {
        reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
        << "press 1-5 & 0 to change the led mode" << endl;
    }
    
    ofDrawBitmapString(reportStream.str(), 20, 652);
    
    ofPopStyle();
    
}





//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
    
    switch (key) {

        case 'r':
            break;

        case 'f':
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





