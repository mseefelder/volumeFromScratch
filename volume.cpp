#include "volume.h"

Volume::Volume(){

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
            int voxelArraySize = XSIZE*YSIZE*ZSIZE;
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
                    //voxelArray[i+2] = (unsigned char)0;
                    //voxelArray[i+1] = (unsigned char)0;
                    //voxelArray[i+4] = (unsigned char)0;
                    i+=1;
                    //i+=1;
                    //iterations+=1;
                }
                file.close();
                cout<<"Closed file"<<endl; // << iterations << " voxels read." << endl;
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

Volume::Volume(char* filePath, int* vSize, Eigen::Vector3f dimension){
    cout << "Start" << endl;
    int voxelArraySize = vSize[0]*vSize[1]*vSize[2];
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
            //voxelArray[i+1] = (unsigned char)0;
            i+=1;
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

    scratchTexture = new Texture();
    cout << "Texture instantiated." << endl;
    errorCheckFunc(__FILE__, __LINE__);
    scratchId = scratchTexture->create(GL_TEXTURE_3D, GL_R8, volSize[0], volSize[1], GL_RED, GL_UNSIGNED_BYTE, voxelArray, volSize[2]); // "Id =" because...
                                                        //...this funcion returns a GLuint value that represents the texture ID.
    errorCheckFunc(__FILE__, __LINE__);
    cout << "Texture created." << endl;
    scratchTexture->setTexParameters(GL_CLAMP, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR);
    errorCheckFunc(__FILE__, __LINE__);
    cout << "Texture parameters set." << endl;

    cout<<"Texture successfully created!"<<endl;

    delete voxelArray;

    calculateGradient();
    errorCheckFunc(__FILE__, __LINE__);
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

///THIS IS NEW----------------
void Volume::resetTexture(){
    //texture = new Texture();

    //cout << "Texture instantiated." << endl;
    //errorCheckFunc(__FILE__, __LINE__);

    //texture->unbind();

    //Id = texture->create(GL_TEXTURE_3D, GL_RGBA8, volSize[0], volSize[1], GL_RGBA, GL_UNSIGNED_BYTE, voxelArray, volSize[2]); // "Id =" because...
                                                        //...this funcion returns a GLuint value that represents the texture ID.
    //errorCheckFunc(__FILE__, __LINE__);
    //texture->setTexParameters(GL_CLAMP, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR);

    cout<<"Reseting volume texture"<<endl;

    texture->update(voxelArray);

    errorCheckFunc(__FILE__, __LINE__);

    cout<<"Volume texture reset"<<endl;
}

void Volume::calculateGradient(){
    cout<<" Calculating gradient "<<endl;

    gradShader = new Shader("shaders/","gradShader",1);
    gradShader->initialize();
    gradShader->enable();

    cout<<" Shader initialized "<<endl;

    texture = new Texture();
    //Id = texture->create(GL_TEXTURE_3D, GL_RGBA8, volSize[0], volSize[1], GL_RGBA, GL_UNSIGNED_BYTE, NULL, volSize[2]);
    Id = texture->create(GL_TEXTURE_3D, GL_RGBA8, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, NULL, 256);
    texture->bindImageRW(Id);

    scratchTexture->bind(scratchId);

    Eigen::Vector3f dimensions = getTextureResolution();

    gradShader->setUniform("baseTexture", scratchId);
    gradShader->setUniform("gradientTexture", Id);
    gradShader->setUniform("resolution", &dimensions[0], 3, 1);

    //glDispatchCompute(volSize[0], volSize[1], volSize[2]);
    glDispatchCompute(256, 256, 256);

    scratchTexture->unbind();
    texture->unbind();
    errorCheckFunc(__FILE__, __LINE__);
}

void Volume::setGradient(GLubyte *gradArray, int dimensions){
    for(int i = 0; i<dimensions; i+=4){
        voxelArray[i] = (unsigned char)gradArray[i];
        voxelArray[i+1] = (unsigned char)gradArray[i+1];
        voxelArray[i+2] = (unsigned char)gradArray[i+2];
    }

    cout << "All set" << endl;

    resetTexture();
}
///-------------------
