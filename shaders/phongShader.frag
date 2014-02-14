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
        //vec3 fronTexCoord = vec3(FragCoord.x)

        //out_Color = vec4(gl_FragCoord.x, gl_FragCoord.y, 0.0 ,1.0);
/*
        for (int i; i<255; i++){
            if (acColor.x < 1.0){
            acColor += texture(Texture, gl_FragCoord.xy/viewportSize).x * vec4(1.0, 0.0, 0.0, 0.0);
            }
        }

        out_Color = acColor;
*/
}
