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
uniform int screenWidth;
uniform int screenHeight;
uniform float layer;
uniform int numberOfSteps;
uniform float stepSize;

uniform sampler3D volumeTexture;
uniform sampler1D transferFunction;
uniform int textureDepth;

out vec4 out_Color;

void main(void)
{

    vec4 acColor = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 curColor = vec4(0.0,0.0,0.0,0.0);

    vec3 fPos = vec3(gl_FragCoord.x/screenWidth, gl_FragCoord.y/screenHeight, 0.0);
    vec3 wFPos;
    vec3 currentPos;
    vec3 volDimHalf = volDimensions/2.0;

    float eX, eY, maxVal;

    maxVal = 0.0;

    eX = ((fPos.x*2.0)-1.0)*diagonal;
    eY = ((fPos.y*2.0)-1.0)*diagonal;
    wFPos = (rendPlaneCenter+eX*uX+eY*uY).xyz;
    currentPos = wFPos;

    for(int j; j<(numberOfSteps+1); j++){
        vec3 coord = (currentPos.xyz+volDimensions)*0.5/(volDimensions).xyz;
        if(coord.x<1.0 && coord.x>0.0 && coord.y<1.0 && coord.y>0.0 && coord.z<1.0 && coord.z>0.0){
            float voxelValue;
            voxelValue = texture(volumeTexture, coord).r;
            curColor = texture(transferFunction, voxelValue);

            if(acColor.a < 1.0) {

                    acColor.rgb += curColor.rgb * curColor.a * acColor.a;
                    acColor.a *= (1.0 - curColor.a);

            }
            else
            {
                    acColor.rgb = curColor.rgb * curColor.a;
                    acColor.a = (1.0 - curColor.a);

            }

            //acColor.x = acColor.x + 2*(voxelValue/numberOfSteps);
            //acColor.xyz = acColor.xyz + coord/numberOfSteps;
            //acColor.x = 1.0;

            //if(voxelValue > acColor.x){
            //   acColor.x = voxelValue;
            // }
        }
        //else{
        //    acColor.xyz = acColor.xyz + vec3(0.0001);
        //}

        currentPos = currentPos - stepSize*uZ.xyz;
        //currentPos = currentPos + 5*uZ.xyz;
    }
    out_Color = acColor;

}
