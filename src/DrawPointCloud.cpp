//
//  DrawPointCloud.cpp
//  kinectExample
//
//  Created by jeonghopark on 12/09/16.
//
//

#include "DrawPointCloud.hpp"


DrawPointCloud::DrawPointCloud(){
    
    bDrawPointCloud = false;
    bLinesPointCloud = false;
    
}






//--------------------------------------------------------------
void DrawPointCloud::drawPointCloud(ofxKinect & kinect, ofColor _c) {
    
    int w = 640;
    int h = 480;
    
    if (bDrawPointCloud) {
        
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_POINTS);
        
        int step = 2;
        for(int y = 0; y < h; y += step) {
            for(int x = 0; x < w; x += step) {
                if(kinect.getDistanceAt(x, y) > 0) {
                    mesh.addColor(_c);
                    mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
                }
            }
        }

        
        glPointSize(10);
//        glColor3b(255, 0, 0);
        ofPushMatrix();
        // the projected points are 'upside down' and 'backwards'
        ofScale(1, -1, -1);
        ofTranslate(0, 0, -1000); // center the points a bit
        ofEnableDepthTest();
        mesh.drawVertices();
        ofDisableDepthTest();
        ofPopMatrix();
        
    }
    
}




//--------------------------------------------------------------
void DrawPointCloud::drawLinesCloud(ofxKinect & kinect, ofColor _c) {
    
    int w = 640;
    int h = 480;
    
    if (bLinesPointCloud) {
        
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_LINES);
        
        int step = 2;
        for(int y = 0; y < h; y += step) {
            for(int x = 0; x < w; x += step) {
                if(kinect.getDistanceAt(x, y) > 0) {
//                    mesh.addColor(kinect.getColorAt(x,y));
                    mesh.addColor(_c);
                    mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
                }
            }
        }
        
        glPointSize(10);
        ofPushMatrix();
        // the projected points are 'upside down' and 'backwards'
        ofScale(1, -1, -1);
        ofTranslate(0, 0, -1000); // center the points a bit
        ofEnableDepthTest();
        mesh.drawVertices();
        ofDisableDepthTest();
        ofPopMatrix();
        
        
    }
    
}


