//
//  drawShape.hpp
//  kinect_visualTest
//
//  Created by JeongHo Park on 05/10/2016.
//
//

#ifndef DrawShape_hpp
#define DrawShape_hpp

#include <ofMain.h>


struct MovingLine{

    ofVec3f startVerPoint;
    ofVec3f startHorPoint;
    ofVec3f endVerPoint;
    ofVec3f endHorPoint;
    ofColor color;
    
    float lineWidth;

};


class DrawShape {
    
    
public:
 
    DrawShape();
    
    void setup(int _lineNUm);
    void update();
    void drawMovingLines();
    void drawMovingLines(ofColor _c);
    
    vector<MovingLine> movingLines;
    
};


#endif /* DrawShape_hpp */
