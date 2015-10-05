#version 150

out vec4 outputColor;

uniform Yvals {
    float position[1024];
} yvals;


void main(void)
{
    float width = 1024.0;
    float height = 768.0;
    float xPos = 0.0;
    float isClose = 0.0;
    for(int i = 0; i < 1024; i++) {
        xPos = float(i);
        if (distance(gl_FragCoord.xy, vec2(xPos, yvals.position[i]*height)) < 10.0) {
            isClose = 1.0;
        }
    }

    float a = 1.0;
    outputColor = vec4(isClose, 0.0, isClose, a);
}

/*
void main(void)
{
    if (gl_FragCoord.y > 300 && gl_FragCoord.y <= 400.0) {
        outputColor = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        outputColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}*/