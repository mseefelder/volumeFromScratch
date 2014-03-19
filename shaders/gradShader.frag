in vec4 ex_Color;
in vec4 gl_FragCoord;

uniform sampler3D volumeTexture;
uniform vec3 volumeResolution;
uniform int rootOfDepth;

out vec4 out_Color;

void main(void)
{
    float PI = 3.14159265358979323846264;
    float xLayer, yLayer;
    float x, y, z;

    xLayer = gl_FragCoord.x/rootOfDepth;
    yLayer = gl_FragCoord.y/rootOfDepth;

    x = sin((xLayer)*(pi/2));
    y = sin((yLayer)*(pi/2));
    z = ((int(xLayer)+1)*(int(yLayer)+1))/volumeResolution.z;

    float voxelValue = texture(volumeTexture, vec3(x, y, z)).a;

    out_Color = vec4(voxelValue);
}

/*
For a 256x256x256 dtaset, we have a (256x16)x(256x16) buffer
This buffer works as a matrix of the dataset's 256 "layers"
______________________
|__|__|__|__|__|__|__|
|__|__|__|__|__|__|__|
|__|__|__|__|__|__|__|
|__|__|__|__|__|__|__|
|__|__|__|__|__|__|__|
|__|__|__|__|__|__|__|
|__|__|__|__|__|__|__|
|__|__|__|__|__|__|__|

xLayer scheme:
______________________
|0_|1_|2_|   ...  |15| ^
|0_|1_|2_|   ...  |15| |
|0_|1_|2_|   ...  |15| |
|0_|1_|2_|   ...  |15| |256x16 pixels
|0_|1_|2_|   ...  |15| |16 cells
|0_|1_|2_|   ...  |15| |
|0_|1_|2_|   ...  |15| |
|0_|1_|2_|   ...  |15| v

yLayer scheme:
______________________
|16|16|16|16|16|16|16|
|. |__|__|__|__|__|__|
|. |__|__|__|__|__|__|
|. |__|__|__|__|__|__|
|  |__|__|__|__|__|__|
|2_|__|__|__|__|__|__|
|1_|__|__|__|__|__|__|
|0_|0_|0_|0_|0_|0_|0_|
<-------------------->
     256x16 pixels
       16 cells

In x and y we make the values for each pixel inside a single
matrix cell vary from 0.0 to 1.0.
The z coordinate, mapped between 0.0 and 1.0 is obtained by
multiplying the integer part of xLayer and yLayer (both increased
by 1) and dividing by the number of Z layers on the dataset.

*/
