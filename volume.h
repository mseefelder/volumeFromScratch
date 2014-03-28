#ifndef VOLUME_H
#define VOLUME_H

///default volume characteristics
#define FILENAME ("datasets/foot256x256x256x1.raw")
#define XSIZE (256)
#define YSIZE (256)
#define ZSIZE (256)

//#include <Eigen/Dense>
#include "effect.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "trackball.hpp"

class Volume
{
public:

    /**
      @brief Default contructor, uses the "foot256x256x256x1.raw" 3D texture
    **/
    Volume();

    /**
      @brief Custom Volume constructor
      @param Complete path to the 3d texture
      @param Integer vector with x,y,z resolution of the 3D texture
      @param Eigen::Vector3f with volume dimensions in "real world"
    **/
    Volume(char* filePath, int* vSize, Eigen::Vector3f dimension);

    ///NEWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    /**
       @brief Receives a 8byte unsigned byte array and sets it as the gradient of the volume data
       @param unsigned char array
     **/
    void setGradient(GLubyte* gradArray, int dimensions);

    /**
       @brief Configures the compute shader, calculates the gradient based on the dataset, set result as the new texture
     **/
    void calculateGradient();

    /**
       @brief Resets the 3d texture with the latest voxelArray update
     **/
    void resetTexture();
    ///-------------------------------------------------

    /**
      @brief Creates the volume container (parallelepiped) and the GL_TEXTURE_3D
    **/
    void loadVolume();

    /**
      @brief Calls the texture's bind() function, binding it to the first free index
      @return Index to wich texture was binded
    **/
    int bindTexture();

    /**
      @brief Returns the GL_TEXTURE_3D index
    **/
    GLuint getTexture3D() {
        return Id;
    }

    /**
      @brief Returns the maximum daigonal of the parallellepiped containing the volume
    **/
    float getDiagonal();

    /**
      @brief Returns the maximum daigonal of the parallellepiped containing the volume
    **/
    Eigen::Vector3f getDimensions();

    /**
      @brief Returns number of layers of the 3D texture
    **/
    int getTextureDepth() {
        return volSize[2];
    }

    Eigen::Vector3f getTextureResolution() {
        Eigen::Vector3f dimensions;
        dimensions << volSize[0], volSize[1], volSize[2];
        return dimensions;
    }

    //Volume(string fileName){}

    virtual ~Volume() {}

private:
    ///Array to store the bytes that describe the voxels:
    GLubyte* voxelArray;

    ///Vector with x,y,z resolution of the 3D texture:
    int* volSize;

    ///The mesh:
    Mesh* mesh;

    ///Shader:
    Shader* gradShader;

    ///The main texture3D:
    Texture* texture;
    ///The main  3D texture id:
    GLint Id;

    ///The texture3D+gradient
    Texture* scratchTexture;
    ///The 3D texture + Gradient id:
    GLint scratchId;

    ///Dimension of the volume in world space
    Eigen::Vector3f realDimension;
};

#endif // VOLUME_H
