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
    } else if (currentMode == 4.0) { //3d field
        for(int i = 0; i < 1024; i++) {
            history.buffers[iterationCount % 1024][i] = buffer[i];
        }
        ofBackground(0);
        mesh.clear();
        mesh.setMode(OF_PRIMITIVE_POINTS);
        std::cout << "starting" << std::endl;
        
        int idx = 0;
        float *current;
        for (int pos = 0; pos < 1024; pos++) {
            current = history.buffers[(pos + iterationCount) % 1024];
            for (int i = 0; i < 1024; i++) {
                mesh.addVertex(ofPoint(pos, i, current[i] * 1000));
                //mesh.addColor(ofFloatColor(pos / 128, i / 1024, current[i]));
                mesh.addColor(ofFloatColor(1,1,1));
            }
        }
        
        mesh.draw();
    } else if (currentMode == 5.0) { //numeric sphere
        for(int i = 0; i < 1024; i++) {
            history.buffers[iterationCount % 1024][i] = buffer[i];
        }
        ofBackground(0);
        mesh.clear();
        mesh.setMode(OF_PRIMITIVE_POINTS);
        int idx = 0;
        float *current;
        float x = 0;
        //middle of the sphere is 0,0,0
        //radius of the sphere is 1025
        for (int y = 0; y < 1024; y++) {
            //want all points where
            for (int z = 0; z < 1024; z++) {
                x = sqrt((1024 * 1024) - (y*y) - (z * z));
                if (ceil(x) - x < .1) {
                    mesh.addVertex(ofPoint(x, y, z)); //000
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(x, y, -z)); //001
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(x, -y, z)); //010
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(x, -y, -z));//011
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(-x, y, z));//100
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(-x, y, -z));//101
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(-x, -y, z));//110
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(-x, -y, -z));//111
                    mesh.addColor(ofFloatColor(1,1,1));
                }
            }
        }
        mesh.draw();
    } else if (currentMode == 6.0) { //spherical coords
        double count = 300;
        int icount = (int)count;
        for(int i = 0; i < icount; i++) {
            history.buffers[iterationCount % icount][i] = buffer[i];
        }
        mesh.clear();
        mesh.setMode(OF_PRIMITIVE_POINTS);
        ofBackground(0);
        float radius = 2048;
        float theta = 0;
        float phi = 0;
        float *current;
        std::cout << "STARTING" << std::endl;
        for (double i = 0; i < count; i++) {
            current = history.buffers[((int)i + iterationCount) % icount];
            for (double j = 0; j < count; j++) {
                theta = 2 * pi * (i / count);
                phi = pi * (j / count);
                //std::cout << "theta: " << theta << ", phi: " << phi << "radius: " << radius << std::endl;
                mesh.addVertex(fromSpherical(ofPoint(radius + current[(int)j] * 10000, theta, phi)));
                mesh.addColor(ofFloatColor(1,1,1));
            }
        }
        mesh.draw();
    } else if (currentMode == 7.0) { //"4d" spherical coords
        for(int i = 0; i < 1024; i++) {
            history.buffers[iterationCount % 1024][i] = buffer[i];
        }
        ofBackground(0);
        mesh.clear();
        mesh.setMode(OF_PRIMITIVE_POINTS);
        int idx = 0;
        float *current;
        float x = 0;
        //middle of the sphere is 0,0,0
        //radius of the sphere is 1025
        for (int y = 0; y < 1024; y++) {
            //want all points where
            for (int z = 0; z < 1024; z++) {
                x = sqrt((1024 * 1024) - (y*y) - (z * z) - (a * a));
                if (ceil(x) - x < .1) {
                    mesh.addVertex(ofPoint(x, y, z)); //000
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(x, y, -z)); //001
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(x, -y, z)); //010
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(x, -y, -z));//011
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(-x, y, z));//100
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(-x, y, -z));//101
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(-x, -y, z));//110
                    mesh.addColor(ofFloatColor(1,1,1));
                    mesh.addVertex(ofPoint(-x, -y, -z));//111
                    mesh.addColor(ofFloatColor(1,1,1));
                }
            }
        }
        mesh.draw();
    } else if (currentMode == 8.0) { //Ball of lines
        double count = 300;
        int icount = (int)count;
        for(int i = 0; i < icount; i++) {
            history.buffers[iterationCount % icount][i] = buffer[i];
        }
        mesh.clear();
        mesh.setMode(OF_PRIMITIVE_LINES);
        ofBackground(1);
        float radius = 2048;
        float theta = 0;
        float phi = 0;
        float *current;
        std::cout << "STARTING" << std::endl;
        mesh.addVertex(ofPoint(0,0,0));
        mesh.addColor(ofFloatColor(1,1,1));
        for (double i = 0; i < count; i++) {
            current = history.buffers[((int)i + iterationCount) % icount];
            for (double j = 0; j < count; j++) {
                theta = 2 * pi * (i / count);
                phi = pi * (j / count);
                //std::cout << "theta: " << theta << ", phi: " << phi << "radius: " << radius << std::endl;
                mesh.addVertex(fromSpherical(ofPoint(radius + current[(int)j] * 10000, theta, phi)));
                mesh.addColor(ofFloatColor(0,0,0));
            }
        }
        
        for(int i = 0; i < count * count; i++) {
            //std::cout << "Adding: " << i + 1 << std::endl;
            mesh.addIndex(0);
            mesh.addIndex(i + 1);
        }
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
    } else if (key == '5') {
        currentMode = 5.0;
    } else if (key == '6') {
        currentMode = 6.0;
    } else if (key == '7') {
        currentMode = 7.0;
    } else if (key == '8') {
        currentMode = 8.0;
    } else if (key == '-') {
        a--;
    } else if (key == '+') {
        a++;
    } else {
        std::cout << string(1,key) << std::endl;
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

//(r, th, ph) -> (x, y, z)
ofPoint ofApp::fromSpherical(ofPoint spherical) {
    return ofPoint(spherical.x * sin(spherical.y) * cos(spherical.z),
                   spherical.x * sin(spherical.y) * sin(spherical.z),
                   spherical.x * cos(spherical.y));
}

//(x, y, z) -> (r, th, ph)
ofPoint ofApp::fromCartesian(ofPoint cartesian) {
    double r = sqrt(cartesian.x * cartesian.x + cartesian.y * cartesian.y + cartesian.z + cartesian.z);
    return ofPoint(r,
                   acos(cartesian.z / r),
                   atan(cartesian.y / cartesian.x));
}

