#version 430
	 
in vec4 ex_Color;
out vec4 out_Color;
in float depth;
	 
void main(void)
{
	out_Color = ex_Color;
	gl_FragDepth = depth;
}
