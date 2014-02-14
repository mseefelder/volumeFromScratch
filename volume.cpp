#include "volume.h"

//Volume::Volume()
//{
//}

Volume::Volume(char* filePath, int* volSize, Eigen::Vector3f dimension){
    int voxelArraySize = volSize[0]*volSize[1]*volSize[2]*2;
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

    mesh -> createParallelepiped(realDimension[0], realDimension[1], realDimension[1]);

    cout<< "Parallelepiped created, now to the texture..."<<endl;

    texture = new Texture();
    cout << "Texture instantiated." << endl;
    Id = texture->create(GL_TEXTURE_3D, GL_R8, XSIZE, YSIZE, GL_RED, GL_UNSIGNED_BYTE, voxelArray, ZSIZE); // "Id =" because...
                                                        //...this funcion returns a GLuint value that represents the texture ID.
    cout << "Texture created." << endl;
    texture->setTexParameters(GL_CLAMP, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR);
    cout << "Texture parameters set." << endl;

    cout<<"Texture successfully created!"<<endl;
}

int Volume::bindTexture(){
    return texture->bind();
}
