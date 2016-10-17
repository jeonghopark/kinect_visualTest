#include "ofApp.h"


using namespace ofxCv;
using namespace cv;


//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(0);
    ofSetFrameRate(60);
    
    nearThreshold = 246;
    farThreshold = 203;
    angle = 16;
    bThreshWithOpenCV = true;

    
#ifdef DEBUG_VIDEO
    player.load("debugMovie2016-10-13-21-25-05-560.mov");
    player.play();
    player.setLoopState(OF_LOOP_NORMAL);
#else
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    kinect.enableDepthNearValueWhite(true);
    kinect.setCameraTiltAngle(angle);
#endif

    
    drawShape.setup(20);
    
    kinectSizeOffSet = 80;
    imageRatio.x = (ofGetWindowSize().x + kinectSizeOffSet) / 640.0;
    imageRatio.y = (ofGetWindowSize().y + kinectSizeOffSet) / 480.0;
    
    
    gui.setup();
    gui.add(fpsView.setup("fps", ""));
    gui.add(ctmffilterValue.setup("Filter", 10, 1, 30));
    gui.add(threshold.setup("Threshold", 15, 0, 255));
    gui.add(invertColor.setup("Invert Color", false));
    gui.add(backGroundColor.setup("Background Color", ofColor(0, 255), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255)));
    gui.add(shapeColor.setup("Shape Color", ofColor(255, 255), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255)));
    gui.add(smallFigureColor.setup("Small Figure Color", ofColor(255, 255), ofColor(0, 0, 0, 0), ofColor(255, 255, 255, 255)));
    
    
    
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
    
    invertColor.addListener(this, &ofApp::changeColorButton);

    
    fileName = "debugMovie";
    fileExt = ".mov";
    vidRecorder.setVideoCodec("mpeg4");
    vidRecorder.setVideoBitrate("800k");
    vidRecorder.setPixelFormat("gray");
    
    ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    bRecording = false;
    
    
}


//--------------------------------------------------------------
void ofApp::changeColorButton(bool & t){
    
    if (invertColor) {
        backGroundColor = ofColor(255);
        shapeColor = ofColor(0);
        smallFigureColor = ofColor(0);
    } else {
        backGroundColor = ofColor(0);
        shapeColor = ofColor(255);
        smallFigureColor = ofColor(255);
    }
    
}





//--------------------------------------------------------------
void ofApp::update() {
    
    fpsView = ofToString(ofGetFrameRate(),2);
    

#ifdef DEBUG_VIDEO
    player.update();

    if(player.isFrameNew()) {
        
        unsigned char * data  = player.getPixels().getData();
        unsigned char * pix = player.getPixels().getData();
//        for (int i = 0; i <640*480; i++){
//            graypixels[i] = data[i*3];
//        }

        for (int i = 0; i<640*480; i++){
            if(pix[i*3] < nearThreshold && pix[i*3] > farThreshold) {
                graypixels[i] = 255;
            } else {
                graypixels[i] = 0;
            }
        }
        
        ctmf(graypixels, medianFiltered, 640, 480, 640, 640, ctmffilterValue, 1);
        
        medianFilteredResult.setFromPixels(medianFiltered, 640, 480, OF_IMAGE_GRAYSCALE);
        medianFilteredResult.mirror(false, true);
        
        finder.setSortBySize(true);
        finder.setMinAreaRadius(10);
        finder.setMaxAreaRadius(200);
        finder.setThreshold(threshold);
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

    
#else

    kinect.update();
    
    if(kinect.isFrameNew()) {
        
        if(bRecording){
            bool success = vidRecorder.addFrame(kinect.getDepthPixels());
            if (!success) {
                ofLogWarning("This frame was not added!");
            }
        }

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
        finder.setMinAreaRadius(10);
        finder.setMaxAreaRadius(200);
        finder.setThreshold(threshold);
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
    
#endif

    //    drawShape.update();
    
}





//--------------------------------------------------------------
void ofApp::draw() {
    
    
    ofBackground(backGroundColor);
    
    if (bCVDraw) {
        drawTransImg(medianFilteredResult);
    }
    
//    easyCam.begin();
//    drawPointCloud.drawPointCloud(kinect, shapeColor);
//    drawPointCloud.drawLinesCloud(kinect, shapeColor);
//    easyCam.end();

    //    if (bDrawShape) {
    //        drawShape.drawMovingLines(shapeColor);
    //    }
    
//    medianFilteredResult.draw(0, 0);
//    player.draw(0, 0);

    
    
    if (finder.size()>0 && bContourDraw) {
        ofPushMatrix();
        ofPushStyle();
        ofSetRectMode(OF_RECTMODE_CENTER);
        
        ofSetColor(shapeColor);
        
        ofTranslate(0, -kinectSizeOffSet);
        
        for (int j=0; j<finder.size(); j++) {
            ofPushMatrix();
            ofScale(imageRatio.x, imageRatio.y, 0);
            finder.getPolyline(j).draw();
            ofPopMatrix();
            
            ofPolyline _polyLines = finder.getPolyline(j);
            vector<glm::vec3> _v = _polyLines.getVertices();
            

            int _step = 10;
            float _ratioSize = 0.25;
            
            for (int i=0; i<_v.size()-1; i+=_step) {
                
                ofPoint _v1 = _polyLines.getPointAtPercent(i / float(_v.size())) * imageRatio;
                ofPoint _v2 = _polyLines.getPointAtPercent((i+1) / float(_v.size())) * imageRatio;
            
                ofPushMatrix();

                ofPoint _diffV = _v2 -_v1;
                ofTranslate(_v1.x, _v1.y, 0);
                
                float _degree = atan2(_diffV.y, _diffV.x);
                ofRotateZDeg(ofRadToDeg(_degree) + 180);
                
                int _index = 5;
                ofTranslate(-silhoutteImg[_index].getWidth() * _ratioSize * 0.5, -silhoutteImg[_index].getHeight() * _ratioSize * 0.5, 0);
                
                ofPushStyle();
                ofSetColor(smallFigureColor);
                silhoutteImg[_index].draw(0, 0, 0, silhoutteImg[_index].getWidth() * _ratioSize, silhoutteImg[_index].getHeight() * _ratioSize);
                
                ofPopStyle();
                
                //                ofDrawBitmapString(ofToString(ofRadToDeg(_degree) + 180), -silhoutteImg[_index].getWidth() * _ratioSize * 0.5, -silhoutteImg[_index].getHeight() * _ratioSize * 0.5);
                
                ofPopMatrix();
                
            }
            
        }
        
        ofPopStyle();
        ofPopMatrix();
    }
    
    
    if (bInformation){
        information();
    }
    
    
    if (bDrawGui) {
        gui.draw();
    }
    
    
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
            ofColor _c = shapeColor;
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
    << "set far threshold " << farThreshold << endl;
    
#ifdef DEBUG_VIDEO
    
#else
    if(kinect.hasCamTiltControl()) {
        reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
        << "press 1-5 & 0 to change the led mode" << endl;
    }
#endif
    
    ofDrawBitmapString(reportStream.str(), 20, 652);
    
    ofPopStyle();
    
}



//--------------------------------------------------------------
void ofApp::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
    cout << "The recoded video file is now complete." << endl;
}



//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
    
    switch (key) {

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
#ifdef DEBUG_VIDEO
            
#else
            kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
#endif
            break;
            
        case 'c':
            bContourDraw = !bContourDraw;
            break;
            
        case 'l':
            drawPointCloud.bLinesPointCloud = !drawPointCloud.bLinesPointCloud;
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
#ifdef DEBUG_VIDEO
            
#else
            kinect.setCameraTiltAngle(angle);
#endif
            break;
            
        case OF_KEY_DOWN:
            angle--;
            if(angle<-30) angle=-30;
#ifdef DEBUG_VIDEO
            
#else
            kinect.setCameraTiltAngle(angle);
#endif
            break;
    }
    
}



//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
#ifdef DEBUG_VIDEO
    
#else
    if(key=='r'){
        bRecording = !bRecording;
        if(bRecording && !vidRecorder.isInitialized()) {
            //            vidRecorder.setup(fileName+ofGetTimestampString()+fileExt, 640, 480, 30, 44100, 2);
            vidRecorder.setup(fileName+ofGetTimestampString()+fileExt, 640, 480, 30); // no audio
            //            vidRecorder.setup(fileName+ofGetTimestampString()+fileExt, 0,0,0, sampleRate, channels); // no video
            //          vidRecorder.setupCustomOutput(vidGrabber.getWidth(), vidGrabber.getHeight(), 30, sampleRate, channels, "-vcodec mpeg4 -b 1600k -acodec mp2 -ab 128k -f mpegts udp://localhost:1234"); // for custom ffmpeg output string (streaming, etc)
            
            // Start recording
            vidRecorder.start();
        }
        else if(!bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(true);
        }
        else if(bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(false);
        }
    }
    if(key=='s'){
        bRecording = false;
        vidRecorder.close();
    }

#endif

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

#ifdef DEBUG_VIDEO
    
#else
    kinect.setCameraTiltAngle(0);
    kinect.close();
#endif
    
    ofRemoveListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    vidRecorder.close();

}





