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
}

//--------------------------------------------------------------
void ofApp::update(){ }

//--------------------------------------------------------------
void ofApp::draw(){
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
}

//--------------------------------------------------------------
void ofApp::plot(vector<float>& buffer, float scale, float offset) {
    ofSetColor(255);
    iterationCount++;
    
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
    shader.setUniformBuffer("Yvals", data);
    shader.setUniform1f("mode", currentMode);
    
    ofRect(0, 0, plotWidth, plotHeight);
    shader.end();
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

