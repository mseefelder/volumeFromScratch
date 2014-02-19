#version 420

in vec4 ex_Color;
in vec3 normalDirection;
in float depth;
in vec4 gl_FragCoord;

uniform float diagonal;
uniform vec4 uX;
uniform vec4 uY;
uniform vec4 uZ;
uniform vec4 rendPlaneCenter;
uniform vec3 volDimensions;

//in int ins;

uniform sampler3D volumeTexture;
uniform sampler1D transferFunction;
uniform int textureDepth;

out vec4 out_Color;

void main(void)
{

    int numberOfSteps;
    float stepSize;
    numberOfSteps = textureDepth;
    stepSize = diagonal/numberOfSteps;

    out_Color = vec4(1.0, 1.0, 1.0 ,1.0);
    vec4 acColor = vec4(0.0, 0.0, 0.0, 1.0);
/*
    vec2 coord = gl_FragCoord.xy/256.0;

    for (int i; i<(numberOfSteps-1); i++){
        float voxelValue;
        voxelValue = texture(volumeTexture, vec3(coord, i/256.0)).r;
        acColor.x = acColor.x + voxelValue/numberOfSteps;
    }
*/

    out_Color = acColor;

///----------------------------------------------------------------------------------

    acColor = vec4(0.0, 0.0, 0.0, 1.0);

    vec3 fPos = gl_FragCoord.xyz/256.0;
    vec3 wFPos;
    vec3 currentPos;

    float eX, eY;

    eX = (fPos.x-0.5)*2.0*diagonal;
    eY = (fPos.y-0.5)*2.0*diagonal;
    wFPos = (rendPlaneCenter+eX*uX+eY*uY).xyz;
    currentPos = wFPos;

    for(int j; j<(numberOfSteps+1); j++){
        bvec3 isInsideTheVolume = lessThan(abs(currentPos), volDimensions);
        //bvec3 isInsideTheVolume = bvec3();
        vec3 coord = ((currentPos.xyz+volDimensions)*0.5/(volDimensions)).xyz;
        if(all(isInsideTheVolume)){// && all(lessThan(coord, vec3(1.0))) && all(greaterThan(coord, vec3(0.0)))){
            float voxelValue;
            voxelValue = texture(volumeTexture, vec3(currentPos)/10).r;
            acColor.x = acColor.x + voxelValue;
        }
        else{
            acColor.y = acColor.y + 0.001;
       }
        currentPos = currentPos + stepSize*uZ.xyz;
    }

    out_Color = acColor;

//-------------------------------------------------------------------------------------

    //out_Color = vec4(uX.xyz,1.0);


}
