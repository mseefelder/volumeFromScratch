#version 430
	 
layout(location=0) in vec4 in_Position;

out vec4 ex_Color;
out float depth;
	 
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec4 in_Color;

uniform float nearPlane;
uniform float farPlane;
	 
void main(void)
{
    gl_Position = (viewMatrix * modelMatrix) * in_Position;
	depth = (farPlane+nearPlane)/(farPlane-nearPlane) + ( (2*nearPlane*farPlane)/(farPlane-nearPlane) ) * (1/gl_Position[2]);
	depth = (depth+1)/2.0;
	gl_Position = projectionMatrix*gl_Position;
	ex_Color = in_Color;
}
