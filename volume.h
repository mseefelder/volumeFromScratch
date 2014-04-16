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

    void resetVolume();

    /**
       @brief Configures the compute shader, calculates the gradient based on the dataset,
              set result as the new texture
     **/
    void calculateGradient();

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
      @brief Calls the texture's unbind() function.
    **/
    void unbindTexture();

    /**
      @brief Returns the GL_TEXTURE_3D index
    **/
    GLuint getTexture3D() {
        //return Id;
        return mainTexture->texID();
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
        return volResolution[2];
    }

    /**
      @brief Returns the dataset's XYZ resolution
    **/
    Eigen::Vector3f getTextureResolution() {
        Eigen::Vector3f dimensions;
        dimensions << volResolution[0], volResolution[1], volResolution[2];
        return dimensions;
    }

    virtual ~Volume() {}

private:
    ///Array to store the bytes that describe the voxels:
    GLubyte* voxelArray;

    ///Vector with x,y,z resolution of the 3D texture:
    int* volResolution;

    ///The mesh:
    Mesh* mesh;

    ///Shaders:
    Shader* gradShader;
    Shader* smoothGrad;

    ///The scratch texture
    Texture* scratchTexture;

    ///The main texture3D:
    Texture* mainTexture;

    ///Dimension of the volume in world space
    Eigen::Vector3f realDimension;
};

#endif // VOLUME_H
