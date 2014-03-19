in vec4 ex_Color;
in vec4 gl_FragCoord;

uniform sampler3D volumeTexture;
uniform vec3 volumeResolution;
uniform int rootOfDepth;

out vec4 out_Color;

void main(void)
{
    float PI = 3.14159265358979323846264;
    float xLayer, yLayer, zLayer;
    float x, y, z;

    xLayer = gl_FragCoord.x/rootOfDepth;
    yLayer = gl_FragCoord.y/rootOfDepth;
    //zLayer = gl_FragCoord.x/volumeResolution.x;

    x = sin((xLayer)*(pi/2));
    y = sin((yLayer)*(pi/2));
    z = (256*int(yLayer)+int(xLayer))/
}
