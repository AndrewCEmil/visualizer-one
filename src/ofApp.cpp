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
    scaledVol       = 0.0;
    
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
    float curAvg = 0;
    std::cout << "buffer size: " << buffer.size() << std::endl;
    int range = buffer.size() / 10;
    float range_avg = 0.0;
    for(int idx = 0; idx < 10; idx++) {
        range_avg = 0.0;
        for(int offset = 0; offset < range; offset++) {
            range_avg += sqrt(buffer[(idx * 10) + offset]);
        }
        range_avg = range_avg / range;
        data.position[idx] = range_avg;
        if(data.position[idx] != data.position[idx]) { //protect from NaN
            data.position[idx] = 1.0;
        }
        
        //update max and scale
        if (data.position[idx] > maxData.position[idx] && iterationCount > 100) {
            maxData.position[idx] = data.position[idx];
        }
        data.position[idx] = data.position[idx] / maxData.position[idx];
    }
    shader.begin();
    
    std::cout << "data 0   : " << data.position[0] << std::endl;
    std::cout << "data 2 : " << data.position[2] << std::endl;
    std::cout << "data 4 : " << data.position[4] << std::endl;
    std::cout << "data 9: " << data.position[9] << std::endl;
    
    shader.setUniformBuffer("Yvals", data);
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
    std::cout << "RAW maxvalue: " << maxValue << std::endl;
    fft->setSignal(input);
    
    float* curFft = fft->getAmplitude();
    memcpy(&audioBins[0], curFft, sizeof(float) * fft->getBinSize());
    
    /*
    maxValue = 0;
    for(int i = 0; i < fft->getBinSize(); i++) {
        if(abs(audioBins[i]) > maxValue) {
            maxValue = abs(audioBins[i]);
        }
    }
    for(int i = 0; i < fft->getBinSize(); i++) {
        audioBins[i] /= maxValue;
    }*/
    //std::cout << "FFT maxvalue: " << maxValue << std::endl;

    
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

