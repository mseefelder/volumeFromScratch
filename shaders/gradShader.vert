#version 420
	 
layout(location=0) in vec4 in_Position;
layout(location=1) in vec3 in_Normal;

out vec4 ex_Color;

out float depth;

	 
void main(void)
{

        gl_Position = in_Position;
        ex_Color = in_Position;

}
