#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
    ofSetCircleResolution(80);
    ofBackground(54, 54, 54);
    
    // 0 output channels,
    // 2 input channels
    // 44100 samples per second
    // 256 samples per buffer
    // 4 num buffers (latency)
    
    soundStream.listDevices();
    
    //if you want to set a different device id
    soundStream.setDeviceID(1); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
    
    smoothedVol     = 0.0;
    scaledVol		= 0.0;
    
    //FFT stuff
    plotHeight = 768;
    plotWidth = 1024;
    bufferSize = 2048;
    
    fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING, OF_FFT_FFTW);
    
    drawBins.resize(fft->getBinSize());
    middleBins.resize(fft->getBinSize());
    audioBins.resize(fft->getBinSize());
    
    shader.load("shadersGL3/shader");
    
    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    for(int i = 0; i < 1024; i++) {
        data.position[i] = 1.0;
    }
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
    float n = buffer.size();
    float curAvg = 0;
    std::cout << "buffer size: " << n << std::endl;
    for (int i = 0; i < 1024.0; i++) {
        data.position[i] = (data.position[i] * 5 + sqrt(buffer[i])) / 6.0;
        if(data.position[i] != data.position[i]) {
            data.position[i] = 1.0;
        }
        //std::cout << "pos: " << i << ", val: " << data.position[i] << std::endl;

    }
    
    shader.begin();
    /*
    std::cout << "data 0   : " << data.position[0] << std::endl;
    std::cout << "data 200 : " << data.position[200] << std::endl;
    std::cout << "data 400 : " << data.position[400] << std::endl;
    std::cout << "data 1000: " << data.position[1000] << std::endl;*/
    
    shader.setUniformBuffer("Yvals", data);
    ofRect(0, 0, plotWidth, plotHeight);
    shader.end();
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels) {
    //FFT section
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
    
    maxValue = 0;
    for(int i = 0; i < fft->getBinSize(); i++) {
        if(abs(audioBins[i]) > maxValue) {
            maxValue = abs(audioBins[i]);
        }
    }
    for(int i = 0; i < fft->getBinSize(); i++) {
        audioBins[i] /= maxValue;
    }
    
    soundMutex.lock();
    middleBins = audioBins;
    soundMutex.unlock();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    
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

