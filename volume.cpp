#include "volume.h"

Volume::Volume(){
            volResolution = new int[3];
            volResolution[0] = XSIZE; volResolution[1] = YSIZE; volResolution[2] = ZSIZE;

            ///Setting up the array to store the parsed dataset
            int voxelArraySize = XSIZE*YSIZE*ZSIZE*4;
            voxelArray = new GLubyte[voxelArraySize];

            ///Reading the texture file and sorting it in voxelArray
            ifstream file (FILENAME, ios::in|ios::binary);

            char buff[128];
            int i =0;

            if(file.is_open())
            {
                cout << "3D texture file opened:"<<endl;
                while (!file.eof())
                {
                    file.read(buff, 1);
                    voxelArray[i+3] = (unsigned char)buff[0];
                    voxelArray[i+2] = (unsigned char)0;
                    voxelArray[i+1] = (unsigned char)0;
                    voxelArray[i] = (unsigned char)0;
                    i+=4;
                }
                file.close();
                cout<<"Closed file"<<endl;
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
//Volume::Volume(string filePath, int* vSize, Eigen::Vector3f dimension){
    cout << "Start" << endl;
    int voxelArraySize = vSize[0]*vSize[1]*vSize[2]*4;
    volResolution = vSize;
    voxelArray = new GLubyte[voxelArraySize];

    ///Reading the texture file and sorting it in voxelArray
    //const char* c = filePath.c_str();
    ifstream file (filePath, ios::in|ios::binary);
    //ifstream file (c, ios::in|ios::binary);

    char buff[128];
    int i =0;

    if(file.is_open())
    {
        cout << "3D texture file opened:"<<endl;
        while (!file.eof())
        {
            file.read(buff, 1);
            voxelArray[i+3] = (unsigned char)buff[0];
            voxelArray[i+2] = (unsigned char)0;
            voxelArray[i+1] = (unsigned char)0;
            voxelArray[i] = (unsigned char)0;
            i+=4;
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

void Volume::resetVolume(char *filePath, int *vSize, Eigen::Vector3f dimension){

    delete mainTexture;
    //delete mesh;
    delete [] volResolution;

    int voxelArraySize = vSize[0]*vSize[1]*vSize[2]*4;
    volResolution = vSize;
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
            voxelArray[i+3] = (unsigned char)buff[0];
            voxelArray[i+2] = (unsigned char)0;
            voxelArray[i+1] = (unsigned char)0;
            voxelArray[i] = (unsigned char)0;
            i+=4;
        }
        file.close();
    }
    else cout << "Unable to open file!" << endl;

    ///Initializing the mesh
    //mesh = new Mesh();
    //cout << "Mesh created." << endl;

    realDimension = dimension;

    ///Loading the Volume
    cout << "Entering loadVolume()" << endl;
    loadVolume();
}


void Volume::loadVolume(){
    cout << "Loading volume..."<<endl;
    cout << volResolution[0] << " " << volResolution[1] << " " << volResolution[2] << endl;

    mesh -> createParallelepiped(realDimension[0], realDimension[1], realDimension[1]);

    cout<< "Parallelepiped created, now to the texture..."<<endl;

    mainTexture = new Texture();
    cout << "Texture instantiated." << endl;
    mainTexture->create(GL_TEXTURE_3D, GL_RGBA8, volResolution[0], volResolution[1],
                        GL_RGBA, GL_UNSIGNED_BYTE, voxelArray, volResolution[2]);
    cout << "Texture created." << endl;
    mainTexture->setTexParameters(GL_CLAMP, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR);
    errorCheckFunc(__FILE__, __LINE__);
    cout << "Texture parameters set." << endl;

    cout<<"Texture successfully created!"<<endl;

    delete [] voxelArray;
    voxelArray = 0;

    calculateGradient();
    errorCheckFunc(__FILE__, __LINE__);
}

int Volume::bindTexture(){
    return mainTexture->bind();
}

void Volume::unbindTexture(){
    mainTexture->unbind();
}

void Volume::calculateGradient(){
    QTime timer;
    timer.start();

    cout<<" Calculating gradient "<<endl;

    gradShader = new Shader("shaders/","gradShader",1);
    gradShader->initialize();
    gradShader->enable();

    cout<<" Shader initialized "<<endl;

    scratchTexture = new Texture();
    scratchTexture->create(GL_TEXTURE_3D, GL_RGBA16F, volResolution[0], volResolution[1],
                           GL_RGBA, GL_HALF_FLOAT, NULL, volResolution[2]);
    GLuint unit = scratchTexture->bind();
    glBindImageTexture(unit, scratchTexture->texID(), 0, GL_TRUE, 0, GL_READ_WRITE,
                       GL_RGBA16F);

    GLint baseUnit = mainTexture->bind();

    Eigen::Vector3f dimensions = getTextureResolution();

    gradShader->setUniform("baseTexture", baseUnit);
    gradShader->setUniform("gradientTexture", (GLint)unit);
    gradShader->setUniform("resolution", &dimensions[0], 3, 1);

    glDispatchCompute(volResolution[0], volResolution[1], volResolution[2]);

    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    mainTexture->unbind();
    scratchTexture->unbind();

    delete mainTexture;
    mainTexture = new Texture();
    cout << "Texture instantiated." << endl;
    mainTexture->create(GL_TEXTURE_3D, GL_RGBA16F, volResolution[0], volResolution[1],
                        GL_RGBA, GL_HALF_FLOAT, NULL, volResolution[2]);
    cout << "Texture created." << endl;
    mainTexture->setTexParameters(GL_CLAMP, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR);
    errorCheckFunc(__FILE__, __LINE__);

    gradShader->disable();

    smoothGrad = new Shader("shaders/","smoothGrad",1);
    smoothGrad->initialize();
    smoothGrad->enable();

    cout<<" Shader initialized "<<endl;

    unit = mainTexture->bind();
    glBindImageTexture(unit, mainTexture->texID(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);

    baseUnit = scratchTexture->bind();
    glBindImageTexture(baseUnit, mainTexture->texID(), 0, GL_TRUE, 0, GL_READ_WRITE,
                       GL_RGBA16F);

    smoothGrad->setUniform("baseTexture", baseUnit);
    smoothGrad->setUniform("gradientTexture", (GLint)unit);

    glDispatchCompute(volResolution[0], volResolution[1], volResolution[2]);

    smoothGrad->setUniform("baseTexture", (GLint)unit);
    smoothGrad->setUniform("gradientTexture", baseUnit);

    glDispatchCompute(volResolution[0], volResolution[1], volResolution[2]);

    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    mainTexture->unbind();
    scratchTexture->unbind();
    delete scratchTexture;
    glFinish();

    cout<<"Gradient calculation and smoothing:"<<timer.elapsed()<<" ms."<<endl;
    errorCheckFunc(__FILE__, __LINE__);
}

float Volume::getDiagonal(){
    float diagonal;
    diagonal = sqrt(pow(realDimension[0], 2) + pow(realDimension[1], 2)
               + pow(realDimension[2], 2));
    return diagonal;
}

Eigen::Vector3f Volume::getDimensions(){
    return realDimension;
}
