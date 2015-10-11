#version 150

out vec4 outputColor;

uniform Yvals {
    float position[1024];
} yvals;


void main(void)
{
    float width = 1024.0;
    float height = 768.0;
    float maxdist = height;
    float mindist = height;
    float curdist = 0.0;
    int checkindex = 0;
    int xindex = int(gl_FragCoord.x);
    /*
    for (int i = -200; i < 200; i++) {
        checkindex = xindex + i;
        if ((checkindex >= 0) && (checkindex < 1024)) {
            curdist = distance(gl_FragCoord.xy, vec2(float(checkindex), yvals.position[checkindex] * height));
            if (curdist < mindist) {
                mindist = curdist;
            }
        }
    }*/
    
    for (int i = 0; i < 1024; i++) {
        curdist = distance(gl_FragCoord.xy, vec2(float(i), yvals.position[i] * height));
        if (curdist < mindist) {
            mindist = curdist;
        }
    }
    mindist = mindist / height;
    outputColor = vec4(mindist, mindist, mindist, 1.0);
    
    
    /*
    float distSum = 0.0;
    for(int i = -10; i < 10; i++) {
        distSum = distSum + abs(gl_FragCoord.y - (yvals.position[abs(int(gl_FragCoord.x) + i)] * height)) / 768;
    }

    float a = 1.0;
    float dist = 1.0 - (distSum / 20.0);

    //float dist = abs(gl_FragCoord.y - (yvals.position[int(gl_FragCoord.x)] * height)) / 768;
    outputColor = vec4(1.-dist, 1.-dist, 1.-dist, a);*/
}