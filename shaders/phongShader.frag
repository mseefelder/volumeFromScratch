#version 420
	 
in vec4 ex_Color;
in vec3 normalDirection;
in float depth;
in vec4 gl_FragCoord ;

uniform sampler3D volumeTexture;
uniform sampler1D transferFunction;
uniform int textureDepth;

out vec4 out_Color;
	 
void main(void)
{

        out_Color = vec4(1.0, 1.0, 1.0 ,1.0);
        vec4 acColor = vec4(0.0, 0.0, 0.0, 1.0);

        vec2 coord = gl_FragCoord.xy/256.0;

        for (int i; i<(textureDepth-1); i++){
            float voxelValue;
            voxelValue = texture(volumeTexture, vec3(coord, i)).r;
            acColor.x = acColor.x + voxelValue/textureDepth;
        }


        out_Color = acColor;
        //out_Color = vec4(texture(volumeTexture, coord).r, 0.0, 0.0, 1.0);
/*
        acColor = vec4(0.0, 0.0, 0.0, 1.0);

        float opacity, accumulatedTransparency;

        vec2 voxelValue;

        vec4 currentColors;

        //It's important to remeber that in order to access the volume position I need to normalize coordinates.

        for (int i; i<(textureDepth-1); i++){

            voxelValue = texture(volumeTexture, vec3(coord, i)).rg;

            currentColors = texture(transferFunction, voxelValue.r);



            //currentColors = vec4(voxelValue.rrr, 0.01);

            if(acColor.a < 1.0) {

                acColor.rgb += currentColors.rgb * currentColors.a * acColor.a;
                acColor.a *= (1.0 - currentColors.a);

            }
            else
            {
                acColor.rgb = currentColors.rgb * currentColors.a;
                acColor.a = (1.0 - currentColors.a);

            }

            //if(emptySpaceSkippingFlag == 0){
            //if(acColor.a > 0.0){
            //       emptySpaceSkippingFlag = -1;
            // }

            // }
        }

        out_Color = acColor;
*/

}
