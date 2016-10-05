//
//  DrawPointCloud.hpp
//  kinectExample
//
//  Created by jeonghopark on 12/09/16.
//
//
#pragma once


#ifndef DrawPointCloud_hpp
#define DrawPointCloud_hpp

#include <ofMain.h>
#include "ofxKinect.h"


class DrawPointCloud {
    
//    ofxKinect kinectT;
    
public:
    
    DrawPointCloud();
    
    bool bDrawPointCloud;
    bool bLinesPointCloud;
    
    
    void drawPointCloud(ofxKinect & _kinect, ofColor _c);
    void drawLinesCloud(ofxKinect & _kinect, ofColor _c);
    
    
};


#endif /* DrawPointCloud_hpp */
