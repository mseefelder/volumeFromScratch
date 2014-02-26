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
    Volume() {

//        GLenum glewInitResult;

//        //Glew Initialization:
//        glewInitResult = glewInit();

//        //Check Glew Initialization:
//        if (GLEW_OK != glewInitResult) {
//            cerr << "Error: " << glewGetErrorString(glewInitResult) << endl;
//            exit(EXIT_FAILURE);
//        }
//        errorCheckFunc(__FILE__, __LINE__);

        volSize = new int[3];
        volSize[0] = XSIZE; volSize[1] = YSIZE; volSize[2] = ZSIZE;

        ///Setting up the
        int voxelArraySize = XSIZE*YSIZE*ZSIZE*2;
        voxelArray = new GLubyte[voxelArraySize];

        ///Reading the texture file and sorting it in voxelArray
        ifstream file (FILENAME, ios::in|ios::binary);

        char buff[128];
        int i =0;

        if(file.is_open())
        {
            cout << "3D texture file opened:"<<endl;
            int iterations = 0; //for debug purpose
            //while (!file.eof() && iterations<voxelArraySize)
            while (!file.eof()) //hardcoded number of bytes to read
            {
                file.read(buff, 1);
                voxelArray[i] = (unsigned char)buff[0];
                voxelArray[i+1] = (unsigned char)0;
                i+=2;
                //i+=1;
                iterations+=1;
            }
            file.close();
            cout<<"Closed file: " << iterations << " voxels read." << endl;
        }
        else cout << "Unable to open file!" << endl;

        ///Initializing the mesh
        mesh = new Mesh();
        cout << "Mesh created." << endl;

        realDimension << 1.0, 1.0, 1.0;

        ///Loading the Volume
        cout << "Entering loadVolume()" << endl;
        loadVolume();
    }

    /**
      @brief Custom Volume constructor
      @param Complete path to the 3d texture
      @param Integer vector with x,y,z resolution of the 3D texture
      @param Eigen::Vector3f with volume dimensions in "real world"
    **/
    Volume(char* filePath, int* vSize, Eigen::Vector3f dimension);

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

    //Volume(string fileName){}

    virtual ~Volume() {}

private:
    ///Array to store the bytes that describe the voxels:
    GLubyte* voxelArray;

    ///Vector with x,y,z resolution of the 3D texture:
    int* volSize;

    ///The mesh:
    Mesh* mesh;

    ///The texture:
    Texture* texture;

    ///The 3D texture id:
    GLuint Id;

    ///Dimension of the volume in world space
    Eigen::Vector3f realDimension;
};

#endif // VOLUME_H
