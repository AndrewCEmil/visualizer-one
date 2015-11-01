#version 150

out vec4 outputColor;

uniform float mode;
uniform Yvals {
    float position[10];
} yvals;


void main(void)
{
    float width = 1024.0;
    float height = 768.0;
    float maxdist = sqrt(height*height + (.05 * width) * (.05 * width));
    float mindist = 20.0;
    float curdist = 0.0;
    int idxwidth = 0;
    bool isnear = false;
    vec3 colorSums = vec3(0.0);
    
    if (mode == 1.0) {
        for (float i = 0.0; i < 10.0; i++) {
            idxwidth = int(((i + .5)/10.0) * width);
            if (mod(i, 3) == 0) { //red
                colorSums.x = colorSums.x + distance(gl_FragCoord.xy, vec2(float(idxwidth), yvals.position[int(i)] * height));
            } else if(mod(i, 2) == 0) { //green
                colorSums.y = colorSums.y + distance(gl_FragCoord.xy, vec2(float(idxwidth), yvals.position[int(i)] * height));
            } else { //blue
                colorSums.z = colorSums.z + distance(gl_FragCoord.xy, vec2(float(idxwidth), yvals.position[int(i)] * height));
            }
        }
        outputColor = vec4(colorSums.x / (4 * maxdist), colorSums.y / (3 * maxdist), colorSums.z / (3 * maxdist), 1.0);
    } else if (mode == 2.0) {
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
            outputColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    } else if (mode == 0.0) {
        outputColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}