#include "volume.h"

//Volume::Volume()
//{
//}

Volume::Volume(char* filePath, int* vSize, Eigen::Vector3f dimension){
    cout << "Start" << endl;
    int voxelArraySize = vSize[0]*vSize[1]*vSize[2]*2;
    //volSize = new int[3];
    volSize = vSize;
    voxelArray = new GLubyte[voxelArraySize];

    ///Reading the texture file and sorting it in voxelArray
    ifstream file (filePath, ios::in|ios::binary);

    char buff[128];
    int i =0;

    if(file.is_open())
    {
        cout << "3D texture file opened:"<<endl;
        while (!file.eof())
        {
            file.read(buff, 1);
            voxelArray[i] = (unsigned char)buff[0];
            voxelArray[i+1] = (unsigned char)0;
            i+=2;
        }
        file.close();
    }
    else cout << "Unable to open file!" << endl;

    ///Initializing the mesh
    mesh = new Mesh();
    cout << "Mesh created." << endl;

    realDimension = dimension;

    ///Loading the Volume
    cout << "Entering loadVolume()" << endl;
    loadVolume();
}

void Volume::loadVolume(){
    cout << "Loading volume..."<<endl;
    cout << volSize[0] << " " << volSize[1] << " " << volSize[2] << endl;

    mesh -> createParallelepiped(realDimension[0], realDimension[1], realDimension[1]);

    cout<< "Parallelepiped created, now to the texture..."<<endl;

    texture = new Texture();
    cout << "Texture instantiated." << endl;
    errorCheckFunc(__FILE__, __LINE__);
    Id = texture->create(GL_TEXTURE_3D, GL_RG8, volSize[0], volSize[1], GL_RG, GL_UNSIGNED_BYTE, voxelArray, volSize[2]); // "Id =" because...
                                                        //...this funcion returns a GLuint value that represents the texture ID.
    errorCheckFunc(__FILE__, __LINE__);
    cout << "Texture created." << endl;
    texture->setTexParameters(GL_CLAMP, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR);
    cout << "Texture parameters set." << endl;

    cout<<"Texture successfully created!"<<endl;
}

int Volume::bindTexture(){
    return texture->bind();
}

float Volume::getDiagonal(){
    float diagonal;
    diagonal = sqrt(pow(realDimension[0], 2) + pow(realDimension[1], 2) + pow(realDimension[2], 2));
    return diagonal;
}

Eigen::Vector3f Volume::getDimensions(){
    return realDimension;
}
