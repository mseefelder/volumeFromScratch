#include "volume.h"

Volume::Volume(){
            volSize = new int[3];
            volSize[0] = XSIZE; volSize[1] = YSIZE; volSize[2] = ZSIZE;

            ///Setting up the
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
    scratchTexture->create(GL_TEXTURE_3D, GL_RGBA8, volSize[0], volSize[1], GL_RGBA, GL_UNSIGNED_BYTE, voxelArray, volSize[2]); // "Id =" because...
                                                        //...this funcion returns a GLuint value that represents the texture ID.
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
    return scratchTexture->bind();
}

void Volume::unbindTexture(){
    scratchTexture->unbind();
}

void Volume::calculateGradient(){
    cout<<" Calculating gradient "<<endl;

    gradShader = new Shader("shaders/","gradShader",1);
    gradShader->initialize();
    gradShader->enable();

    cout<<" Shader initialized "<<endl;

    texture = new Texture();
    texture->create(GL_TEXTURE_3D, GL_RGBA8, volSize[0], volSize[1], GL_RGBA, GL_UNSIGNED_BYTE, NULL, volSize[2]);
    GLuint unit = texture->bind();
    glBindImageTexture(unit, texture->texID(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);

    GLint baseUnit = scratchTexture->bind();

    Eigen::Vector3f dimensions = getTextureResolution();

    gradShader->setUniform("baseTexture", baseUnit);
    gradShader->setUniform("gradientTexture", (GLint)unit);
    gradShader->setUniform("resolution", &dimensions[0], 3, 1);

    glDispatchCompute(volSize[0], volSize[1], volSize[2]);

    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    scratchTexture->unbind();
    texture->unbind();

    gradShader->disable();

    smoothGrad = new Shader("shaders/","smoothGrad",1);
    smoothGrad->initialize();
    smoothGrad->enable();

    cout<<" Shader initialized "<<endl;

    //texture = new Texture();
    //scratchTexture->create(GL_TEXTURE_3D, GL_RGBA8, volSize[0], volSize[1], GL_RGBA, GL_UNSIGNED_BYTE, NULL, volSize[2]);
    unit = scratchTexture->bind();
    glBindImageTexture(unit, scratchTexture->texID(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);

    baseUnit = texture->bind();

    smoothGrad->setUniform("baseTexture", baseUnit);
    smoothGrad->setUniform("gradientTexture", (GLint)unit);

    glDispatchCompute(volSize[0], volSize[1], volSize[2]);

    glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    scratchTexture->unbind();
    texture->unbind();
    errorCheckFunc(__FILE__, __LINE__);
}

float Volume::getDiagonal(){
    float diagonal;
    diagonal = sqrt(pow(realDimension[0], 2) + pow(realDimension[1], 2) + pow(realDimension[2], 2));
    return diagonal;
}

Eigen::Vector3f Volume::getDimensions(){
    return realDimension;
}


/*----------------------------------------------------------------------------------------------------------
void Volume::setGradient(GLubyte *gradArray, int dimensions){
    for(int i = 0; i<dimensions; i+=4){
        voxelArray[i] = (unsigned char)gradArray[i];
        voxelArray[i+1] = (unsigned char)gradArray[i+1];
        voxelArray[i+2] = (unsigned char)gradArray[i+2];
    }

    cout << "All set" << endl;

    resetTexture();
}

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
}*/
///-------------------
