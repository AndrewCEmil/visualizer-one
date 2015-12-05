#pragma once

#include "ofMain.h"
#include "ofxFft.h"
#include "ofxUbo.h"

struct Yvals {
    float position[10];
};

struct LineVals {
    float position[1024];
};

struct BufferHistory {
    float buffers[1024][1024];
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void plot(vector<float>& buffer, float scale, float offset);
        void audioIn(float * input, int bufferSize, int nChannels);
    
        vector <float> drawBins, middleBins, audioBins;
    
        int plotHeight;
        int plotWidth;
        int bufferSize;
        int iterationCount;
        int numBuckets = 10;
        float currentMode = 0.0;
    
        ofxFft* fft;
    
        ofMutex soundMutex;
        ofSoundStream soundStream;
        ofxUboShader shader;
    
        Yvals data;
        LineVals lineVals;
        Yvals maxData;
        LineVals maxLineVals;
    
        ofMesh mesh;
        ofEasyCam cam;
        BufferHistory history;
};
