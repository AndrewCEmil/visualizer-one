#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
    ofSetCircleResolution(80);
    ofBackground(54, 54, 54);
    
    soundStream.listDevices();
    soundStream.setDeviceID(0);
    
    plotHeight = 768;
    plotWidth = 1024;
    bufferSize = 2048; //Seems like this is too large actually
    
    fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING, OF_FFT_FFTW);
    
    drawBins.resize(fft->getBinSize());
    middleBins.resize(fft->getBinSize());
    audioBins.resize(fft->getBinSize());
    
    shader.load("shadersGL3/shader");
    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    for(int i = 0; i < 10; i++) {
        data.position[i] = 0.0;
        maxData.position[i] = 0.0;
    }
    iterationCount = 0.0;
    
    cam = ofEasyCam();
    cam.enableMouseInput();
    cam.enableMouseMiddleButton();
}

//--------------------------------------------------------------
void ofApp::update(){ }

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();
    ofPushStyle();
    ofPushMatrix();
    
    ofSetColor(225);
    ofDrawBitmapString("Frequency domain", 4, 18);
    string msg = ofToString((int) ofGetFrameRate()) + " fps";
    ofDrawBitmapString(msg, 50, 100);
    
    soundMutex.lock();
    drawBins = middleBins;
    soundMutex.unlock();
    
    plot(drawBins, -plotHeight, plotHeight / 2);
    
    ofPopMatrix();
    ofPopStyle();
    cam.end();
}

//--------------------------------------------------------------
void ofApp::plot(vector<float>& buffer, float scale, float offset) {
    ofSetColor(255);
    iterationCount++;
    
    if(currentMode == 1.0 || currentMode == 2.0) {
        int range = buffer.size() / numBuckets;
        float range_avg = 0.0;
        for(int idx = 0; idx < numBuckets; idx++) {
            range_avg = 0.0;
            for(int offset = 0; offset < range; offset++) {
                range_avg += sqrt(buffer[(idx * numBuckets) + offset]);
            }
            range_avg = range_avg / range;
            data.position[idx] = range_avg;
            if(data.position[idx] != data.position[idx]) { //protect from NaN
                data.position[idx] = 1.0;
            }

            if (iterationCount > 200) { //Wait 200 iterations before setting max
                if(data.position[idx] > maxData.position[idx] && iterationCount > 200) {
                    maxData.position[idx] = data.position[idx];
                }
                data.position[idx] = data.position[idx] / maxData.position[idx];
            }
        }

        //Adjust for current set of values
        float curmax = 0.0;
        float curmin = 1.0;
        for(int i = 0; i < numBuckets; i++) {
            if (data.position[i] > curmax) {
                curmax = data.position[i];
            }
            if(data.position[i] < curmin) {
                curmin = data.position[i];
            }
        }
        for(int i = 0; i < numBuckets; i++) {
            data.position[i] = (data.position[i] - curmin) / (curmax - curmin);
        }

        shader.begin();
        shader.setUniformBuffer("LineVals", lineVals);
        shader.setUniformBuffer("Yvals", data);
        shader.setUniform1f("mode", currentMode);
        ofRect(0, 0, plotWidth, plotHeight);
        shader.end();
    } else if (currentMode == 3.0) { //Lines
        for(int i = 0; i < buffer.size(); i++) {
            lineVals.position[i] = buffer[i];
            if(lineVals.position[i] != lineVals.position[i]) {
                lineVals.position[i] = 1.0;
            }
            if (iterationCount > 200) { //Wait 200 iterations before setting max
                if(lineVals.position[i] > maxLineVals.position[i] && iterationCount > 200) {
                    maxLineVals.position[i] = lineVals.position[i];
                }
                lineVals.position[i] = lineVals.position[i] / maxLineVals.position[i];
            }
        }
   
        shader.begin();
        shader.setUniformBuffer("LineVals", lineVals);
        shader.setUniformBuffer("Yvals", data);
        shader.setUniform1f("mode", currentMode);
        ofRect(0, 0, plotWidth, plotHeight);
        shader.end();
    } else if (currentMode == 4.0) {
        ofBackground(0);
        mesh.clear();
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        
        /*

        for (int x = 0; x < plotWidth; x++){
            mesh.addVertex(ofPoint(x,0,0));
            mesh.addColor(ofFloatColor(buffer[x]*plotHeight, x, 256-x));
            
            mesh.addVertex(ofPoint(x, buffer[x] * plotHeight * 10,0)); // make a new vertex
            mesh.addColor(ofFloatColor(buffer[x]*plotHeight, x, 256-x));

            mesh.addVertex(ofPoint(x,plotHeight - 1, 0));
            mesh.addColor(ofFloatColor(buffer[x]*plotHeight, x, 256-x));
        }
        for (int y = 0; y<plotWidth; y++){
            mesh.addIndex((4 * y) - 2); //Previous bottom point
            mesh.addIndex((4*y)); //current top point
            mesh.addIndex(4*y + 2);//current point
        }
       */
        mesh.addVertex(ofPoint(plotWidth, plotHeight, 0));
        mesh.addColor(ofFloatColor(0, 0, 1));
        mesh.addVertex(ofPoint(0,0,0));
        mesh.addColor(ofFloatColor(0,1,0));
        mesh.addVertex(ofPoint(plotWidth, plotHeight / 2, 0));
        mesh.addColor(ofFloatColor(1,0,0));
        mesh.addVertex(ofPoint(plotWidth, 0, 200));
        mesh.addColor(ofFloatColor(.5,.5,.5));
        mesh.addVertex(ofPoint(0,plotHeight,50));
        mesh.addColor(ofFloatColor(.6,.6,.6));
        
        mesh.addIndex(3);
        mesh.addIndex(1);
        mesh.addIndex(2);
        mesh.addIndex(0);
        mesh.addIndex(4);
        mesh.addIndex(2);
        /*
        for (int y = 0; y < plotHeight; y++){
            for (int x = 0; x<plotWidth; x++){
                mesh.addVertex(ofPoint(x,y,abs(x - y))); // make a new vertex
                mesh.addColor(ofFloatColor(float(y) / float(plotHeight),0,0));  // add a color at that vertex
            }
        }

        // now it's important to make sure that each vertex is correctly connected with the
        // other vertices around it. This is done using indices, which you can set up like so:
        for (int y = 0; y<plotHeight-1; y++){
            for (int x=0; x<plotWidth-1; x++){
                mesh.addIndex(x+y*plotWidth);               // 0
                mesh.addIndex((x+1)+y*plotWidth);           // 1
                mesh.addIndex(x+(y+1)*plotWidth);           // 10
                
                mesh.addIndex((x+1)+y*plotWidth);           // 1
                mesh.addIndex((x+1)+(y+1)*plotWidth);       // 11
                mesh.addIndex(x+(y+1)*plotWidth);           // 10
            }
         }*/
        
        mesh.draw();
    }
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels) {
    //FFT section
    //Why am I scaling this input? is this a requirement for ofxfft?
    float maxValue = 0;
    for(int i = 0; i < bufferSize; i++) {
        if(abs(input[i]) > maxValue) {
            maxValue = abs(input[i]);
        }
    }
    for(int i = 0; i < bufferSize; i++) {
        input[i] /= maxValue;
    }
    
    fft->setSignal(input);
    
    float* curFft = fft->getAmplitude();
    memcpy(&audioBins[0], curFft, sizeof(float) * fft->getBinSize());
    
    soundMutex.lock();
    middleBins = audioBins;
    soundMutex.unlock();
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    if (key == '0') {
        currentMode = 0.0;
    } else if (key == '1') {
        currentMode = 1.0;
    } else if (key == '2') {
        currentMode = 2.0;
    } else if (key == '3') {
        currentMode = 3.0;
    } else if (key == '4') {
        currentMode = 4.0;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}

