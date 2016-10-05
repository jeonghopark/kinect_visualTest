//
//  drawShape.cpp
//  kinect_visualTest
//
//  Created by JeongHo Park on 05/10/2016.
//
//

#include "DrawShape.hpp"


DrawShape::DrawShape(){
    
    
}



//--------------------------------------------------------------
void DrawShape::setup(int _lineNUm){
    
    movingLines.resize(_lineNUm);

    for (int i=0; i<movingLines.size(); i++) {
        float _x = ofRandom(0, ofGetWidth());
        float _y = ofRandom(0, ofGetHeight());
        movingLines[i].startVerPoint = ofVec3f(_x, 0, 0);
        movingLines[i].endVerPoint = ofVec3f(_x, ofGetHeight(), 0);
        movingLines[i].startHorPoint = ofVec3f(0, _y, 0);
        movingLines[i].endHorPoint = ofVec3f(ofGetWidth(), _y, 0);
        movingLines[i].lineWidth = ofRandom(0, 30);
    }
    
}



//--------------------------------------------------------------
void DrawShape::update(){
    
}



//--------------------------------------------------------------
void DrawShape::drawMovingLines(){
    

}


//--------------------------------------------------------------
void DrawShape::drawMovingLines(ofColor _c){
    
    ofPushStyle();
    ofSetColor(_c);
    
    for (int i=0; i<movingLines.size(); i++) {
        ofDrawRectangle(movingLines[i].startVerPoint.x, movingLines[i].startVerPoint.y, movingLines[i].lineWidth, ofGetHeight());
        ofDrawRectangle(movingLines[i].startHorPoint.x, movingLines[i].startHorPoint.y, ofGetWidth(), movingLines[i].lineWidth);
    }
    
    ofPopStyle();
    
}



