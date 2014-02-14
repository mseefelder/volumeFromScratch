#version 420
	 
in vec4 ex_Color;
in vec3 normalDirection;
in float depth;
in vec4 gl_FragCoord ;

uniform sampler3D volumeTexture;

out vec4 out_Color;
	 
void main(void)
{

        out_Color = vec4(1.0, 1.0, 1.0 ,1.0);
        vec4 acColor = vec4(0.0, 0.0, 0.0, 1.0);

        vec3 coord = gl_FragCoord.xyz/500;

        for (int i; i<255; i++){
            float voxelValue;
            voxelValue = texture(volumeTexture, coord).r;
            acColor.x = acColor.x + voxelValue/256;
        }


        out_Color = acColor;

        //out_Color = vec4(texture(volumeTexture, coord).r, 0.0, 0.0, 1.0);
}
