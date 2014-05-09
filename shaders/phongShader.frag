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
uniform int numberOfSteps;
uniform float stepSize;

uniform sampler3D volumeTexture;
uniform sampler2D transferFunction;
uniform sampler2D jitteringTexture;
uniform int textureDepth;

uniform mat3 lightViewMatrix;

out vec4 out_Color;

void main(void)
{   
    int substeps = 2;
    float pi = 2.1415;
    float shininess = 100.0;
    vec4 acColor = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 acGrad = vec4(0.0,0.0,0.0,0.0);
    vec4 acEye = vec4(0.0);
    vec4 curColor = vec4(0.0);

    vec3 fPos = vec3(gl_FragCoord.x/screenWidth, gl_FragCoord.y/screenHeight, 0.0);
    vec3 wFPos;
    vec3 currentPos;
    vec3 volDimHalf = volDimensions/2.0;

    float eX, eY, maxVal, alpha;

    maxVal = 0.0;

    eX = ((fPos.x*2.0)-1.0)*(diagonal/2.0);
    eY = ((fPos.y*2.0)-1.0)*(diagonal/2.0);
    wFPos = (rendPlaneCenter+eX*uX+eY*uY).xyz;
    currentPos = wFPos;// + normalize(uZ.xyz)*(texelFetch(jitteringTexture, ivec2(gl_FragCoord.xy), 0).x+0.00001)*0.5;

    //vec3 lightDirection = vec3(0.0,1.0,0.0);
    vec3 lightDirection = lightViewMatrix * vec3(0.0, 0.0, 1.0);
    lightDirection = normalize(lightDirection);
    
    float diffuseAcc;

    for(int j; j<((numberOfSteps*substeps)+10); j++){
        //vec3 coord = (currentPos.xyz+volDimensions)*0.5/(volDimensions).xyz;
        vec3 coord = (currentPos.xyz/(volDimensions).xyz)+vec3(0.5);
        if(coord.x<1.0 && coord.x>0.0 && coord.y<1.0 && coord.y>0.0 && coord.z<1.0 && coord.z>0.0){
            vec4 voxelValue;
            voxelValue = texture(volumeTexture, coord);
            //voxelValue.xyz = (voxelValue.xyz*2.0) - vec3(1.0);
            
            //curColor = texture(transferFunction, voxelValue.a); //1D tf
            curColor = texture(transferFunction, vec2(voxelValue.a, length(voxelValue.xyz))); //2D tf

            //Compositing: Methodo 1
            alpha = acColor.a;
            //curColor.xyz = 0.9*curColor.xyz * max(dot(lightDirection, normalize(voxelValue.xyz)), 0.0)
            //            + 0.1*curColor.xyz;
            acColor += (1.0 - alpha) * curColor;

            //Compositing: Methodo 2
            /**
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

            //Compositing: Methodo= 3
            /*
            if(acColor.a < 1.0){
            acColor.rgb += curColor.rgb*curColor.a;
            acColor.a += curColor.a*curColor.a;
            }
            /**/

            //acGrad += abs(voxelValue.a*voxelValue); //GRAD TESTING - method #1

            //GRAD TESTING - method #2
            /*
            if(acGrad.a < 1.0){
                if(curColor.a != 0){
                    acGrad.xyz += (1.0 - alpha)*(voxelValue.xyz);
                    acGrad.a +=  (1.0 - alpha) * curColor.a;   
                }            
            }
            /**/

            //GRAD TESTING - method #2
            /**/
            if(acGrad.a < 1.0){
                if(curColor.a != 0){
                    acGrad += (1.0 - alpha)*(voxelValue);   
                }            
            }
            /**/

            if(acEye.a < 1.0){
                if(curColor.a != 0){
                    acEye.xyz += (1.0 - alpha)*(-normalize(coord));
                    acEye.a +=  (1.0 - alpha) * curColor.a;   
                }            
            }
            
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
    
/*
    float limit = 0.8;

    if (acGrad.a < limit){
        acGrad = vec4(0.0);
    }
    else{
        acGrad.xyz = normalize(acGrad.xyz); //GRAD TESTING
        float comp = acGrad.y; //GRAD TESTING
        out_Color = vec4(comp, comp, comp, 1.0); //GRAD TESTING 
    }
*/
    
    vec3 lightReflection = reflect(-lightDirection, acGrad.xyz);

    acColor.xyz = normalize(acColor.xyz);
    acColor = 0.1 * vec4(1.0) * max(dot(lightDirection, acGrad.xyz), 0.0)
            + 0.7 * acColor * max(dot(lightDirection, normalize(acGrad.xyz)), 0.0)
            + 0.0 * max(pow(dot(lightReflection,acEye.xyz),shininess),0.0)
            + 0.2 * acColor 
            + (0.00) * normalize(acGrad) 
            + 0.0 * vec4(normalize(lightDirection), 0.0) ;//*vec4(1.0);

    acColor.a = 1.0;
    out_Color = acColor ;

    //out_Color = texture(transferFunction, vec2(gl_FragCoord.x/screenWidth, gl_FragCoord.y/screenHeight));
    //out_Color = vec4(vec3((texture(transferFunction, vec2(gl_FragCoord.x/screenWidth, gl_FragCoord.y/screenHeight))).a), 1.0);
}

    //lightDirection = normalize(vec3(1.0,0.0,0.0));
    //lightDirection = normalize(vec3(-1.0,0.0,0.0));
    //lightDirection = normalize(vec3(0.0,1.0,0.0));
    //lightDirection = normalize(vec3(0.0,-1.0,0.0));
    //lightDirection = normalize(vec3(0.0,0.0,1.0));
    //lightDirection = normalize(vec3(0.0,0.0,-1.0));
    //Octante 1
    //lightDirection = normalize(vec3(1.0,1.0,1.0));
    //Octante 2
    //lightDirection = normalize(vec3(1.0,1.0,-1.0));
    //Octante 3
    //lightDirection = normalize(vec3(-1.0,1.0,-1.0));
    //Octante 4
    //lightDirection = normalize(vec3(-1.0,1.0,1.0));
    //Octante 5
    //lightDirection = normalize(vec3(1.0,-1.0,1.0));
    //Octante 6
    //lightDirection = normalize(vec3(1.0,-1.0,-1.0));
    //Octante 7
    //lightDirection = normalize(vec3(-1.0,-1.0,-1.0));
    //Octante 8
    //lightDirection = normalize(vec3(-1.0,-1.0,1.0));
