#version 150

out vec4 outputColor;

uniform Yvals {
    float position[10];
} yvals;


void main(void)
{
    float width = 1024.0;
    float height = 768.0;
    float maxdist = height;
    float mindist = 20.0;
    float curdist = 0.0;
    int idxwidth = 0;
    bool isnear = false;
    
    for (float i = 0.0; i < 10.0; i++) {
        idxwidth = int(((i + .5)/10.0) * width);
        curdist = distance(gl_FragCoord.xy, vec2(float(idxwidth), yvals.position[int(i)] * height));
        if (curdist < mindist) {
            isnear = true;
        }
    }
    if (isnear) {
        outputColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        outputColor = vec4(1.0,1.0, 1.0, 1.0);
    }
}