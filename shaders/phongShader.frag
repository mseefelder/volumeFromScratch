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
uniform sampler2D jitteringTexture;
uniform int textureDepth;

out vec4 out_Color;

void main(void)
{   
    int substeps = 2;
    float pi = 2.1415;
    vec4 acColor = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 curColor = vec4(0.0);

    vec3 fPos = vec3(gl_FragCoord.x/screenWidth, gl_FragCoord.y/screenHeight, 0.0);
    vec3 wFPos;
    vec3 currentPos;
    vec3 volDimHalf = volDimensions/2.0;

    float eX, eY, maxVal;

    maxVal = 0.0;

    eX = ((fPos.x*2.0)-1.0)*(diagonal/2.0);
    eY = ((fPos.y*2.0)-1.0)*(diagonal/2.0);
    wFPos = (rendPlaneCenter+eX*uX+eY*uY).xyz;
    currentPos = wFPos; // - normalize(uZ.xyz)*(texelFetch(jitteringTexture, ivec2(gl_FragCoord.xy), 0).x+0.00001);

    vec3 lightDirection = vec3(1.0,0.0,0.0);
    float diffuseAcc;

    for(int j; j<(numberOfSteps*substeps); j++){
        //vec3 coord = (currentPos.xyz+volDimensions)*0.5/(volDimensions).xyz;
        vec3 coord = (currentPos.xyz/(volDimensions).xyz)+vec3(0.5);
        if(coord.x<1.0 && coord.x>0.0 && coord.y<1.0 && coord.y>0.0 && coord.z<1.0 && coord.z>0.0){
            vec4 voxelValue;
            voxelValue = texture(volumeTexture, coord);
            curColor = texture(transferFunction, voxelValue.a);

            //acColor += (1.0 - acColor.a) * curColor;

            /**/
            if(acColor.a < 1.0) {
                    acColor.rgb += curColor.rgb * curColor.a * acColor.a;
                    acColor.a *= (1.0 - curColor.a);                    
            }
            else
            {
                    acColor.rgb = curColor.rgb * curColor.a;
                    acColor.a = (1.0 - curColor.a);                    
            }
            /**/
            
            //acColor += abs(voxelValue); //GRAD TESTING

            //diffuseAcc = diffuseAcc + max(dot(lightDirection, voxelValue.xyz),0.0);

            //acColor.rgb = acColor.xyz * max(dot(lightDirection, voxelValue.xyz), 0.0); //MAXIMUM ENCOUTERED BY THIS RAY
            //acColor.xyz = acColor.xyz + abs(voxelValue.xyz/numberOfSteps); //GRADIENT MIX
        }
        //else{
        //    acColor.xyz = acColor.xyz + vec3(0.0001);
        //}

        currentPos = currentPos - (stepSize/substeps)*uZ.xyz; //*texture(jitteringTexture, vec2(sin((gl_FragCoord.xy/32.0)*pi/2.0))).x;
        //currentPos = currentPos + 5*uZ.xyz;
    }
    //diffuseAcc = diffuseAcc/numberOfSteps;
    //acColor.xyz = 100*diffuseAcc*acColor.xyz;

    //accGrad = accGrad/(numberOfSteps*substeps);
    //acColor = acColor*max(dot(lightDirection, accGrad), 0.0);
    //out_Color = vec4(normalize(acColor.xyz), 1.0); //GRAD TESTING 

    //acColor.xyz = normalize(acColor.xyz); //GRAD TESTING
    //float comp = acColor.y; //GRAD TESTING
    //out_Color = vec4(comp, comp, comp, 1.0); //GRAD TESTING


    out_Color = acColor ;

}
