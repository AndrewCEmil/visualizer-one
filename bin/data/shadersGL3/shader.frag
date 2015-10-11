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
    float distSum = 0.0;
    for(int i = -10; i < 10; i++) {
        distSum = distSum + abs(gl_FragCoord.y - (yvals.position[abs(int(gl_FragCoord.x) + i)] * height)) / 768;
    }

    float a = 1.0;
    float dist = 1.0 - (distSum / 20.0);

    //float dist = abs(gl_FragCoord.y - (yvals.position[int(gl_FragCoord.x)] * height)) / 768;
    outputColor = vec4(1.-dist, 1.-dist, 1.-dist, a);
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