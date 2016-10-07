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
    
    //    colorImg.allocate(kinect.width, kinect.height);
    //    grayImage.allocate(kinect.width, kinect.height);
    //    grayThreshNear.allocate(kinect.width, kinect.height);
    //    grayThreshFar.allocate(kinect.width, kinect.height);
    
    nearThreshold = 91;
    farThreshold = 22;
    bThreshWithOpenCV = true;
    
    ofSetFrameRate(60);
    
    angle = 0;
    kinect.setCameraTiltAngle(angle);
    
    drawShape.setup(20);
    
    kinectSizeOffSet = 80;
    imageRatio.x = (ofGetWindowSize().x + kinectSizeOffSet) / 640.0;
    imageRatio.y = (ofGetWindowSize().y + kinectSizeOffSet) / 480.0;
    
    
    gui.setup();
    gui.add(ctmffilterValue.setup("Filter", 5, 1, 30));
    gui.add(defaultColor.setup("Default Color", ofColor(255, 255, 255, 255), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255)));
    gui.add(backGroundColor.setup("Default Color", ofColor(0, 0), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255)));
    
    
    
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
    
    
    //    randomShape();
    
    
    kinect.update();
    
    if(kinect.isFrameNew()) {
        
        //        grayImage.setFromPixels(kinect.getDepthPixels());
        //
        //        if(bThreshWithOpenCV) {
        //            grayThreshNear = grayImage;
        //            grayThreshFar = grayImage;
        //            grayThreshNear.threshold(nearThreshold, true);
        //            grayThreshFar.threshold(farThreshold);
        //            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        //        } else {
        //            ofPixels & pix = grayImage.getPixels();
        //            int numPixels = pix.size();
        //            for(int i = 0; i < numPixels; i++) {
        //                if(pix[i] < nearThreshold && pix[i] > farThreshold) {
        //                    pix[i] = 255;
        //                } else {
        //                    pix[i] = 0;
        //                }
        //            }
        //        }
        
        unsigned char * data  = kinect.getDepthPixels().getData();
        for (int i = 0; i < 640*480; i++){
            graypixels[i] = data[i];
        }
        
        
        ctmf(graypixels, medianFiltered,
             640, 480, 640, 640, ctmffilterValue, 1);
        
        medianFilteredResult.setFromPixels(medianFiltered, 640, 480, OF_IMAGE_GRAYSCALE);
        
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
    
    //    kinect.draw(0, 0, 1024, 768);
    
    if (bCVDraw) {
        //        drawTransImg(grayImage);
    }
    //
    //    easyCam.begin();
    //    drawPointCloud.drawPointCloud(kinect, defaultColor);
    //    drawPointCloud.drawLinesCloud(kinect, defaultColor);
    //    easyCam.end();
    //
    //
    //    if (bDrawShape) {
    //        drawShape.drawMovingLines(defaultColor);
    //    }
    
    //    medianFilteredResult.draw(0,0);
    
    
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
            finder.getPolyline(j).draw();
            
            ofPolyline _polyLines = finder.getPolyline(j);
            vector<glm::vec3> _v = _polyLines.getVertices();
            
            for (int i=0; i<_v.size(); i+=17) {
                float _ratioSize = 0.25;
                int _index = i % silhoutteImg.size();
                silhoutteImg[_index].draw(_v[i] * imageRatio, silhoutteImg[_index].getWidth() * _ratioSize, silhoutteImg[_index].getHeight() * _ratioSize);
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
//void ofApp::drawTransImg(ofxCvGrayscaleImage _img){
//
//    ofImage _transImg;
//
//    _transImg.allocate(640, 480, OF_IMAGE_COLOR_ALPHA);
//
//    ofPixels & pix = _img.getPixels();
//    ofPixels & pixT = _transImg.getPixels();
//    int numPixels = pix.size();
//    for(int i = 0; i < numPixels; i++) {
//        if(pix[i] == 255) {
//            ofColor _c = defaultColor;
//            pixT[i*4+0] = _c.r;
//            pixT[i*4+1] = _c.g;
//            pixT[i*4+2] = _c.b;
//            pixT[i*4+3] = _c.a;
//        } else {
//            pixT[i*4+0] = pix[i];
//            pixT[i*4+1] = 255;
//            pixT[i*4+2] = 255;
//            pixT[i*4+3] = 0;
//        }
//    }
//
//    _transImg.update();
//    _transImg.draw(0, -kinectSizeOffSet, ofGetWindowSize().x + kinectSizeOffSet, ofGetWindowSize().y + kinectSizeOffSet);
//
//
//}




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
    //    << "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
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


