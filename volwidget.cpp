#include "volwidget.h"

volWidget::volWidget(QWidget *parent) : QGLWidget(parent)
{

    initializeGL();

}

void volWidget::initializeGL() {

    makeCurrent();

    QGLFormat glCurrentFormat = this->format();
    cout << "set version : " << glCurrentFormat.majorVersion() << " , " << glCurrentFormat.minorVersion() << endl;
    cout << "set profile : " << glCurrentFormat.profile() << endl;
    // Glew Initialization:
    cout << "initializing glew ..." << endl;

    glewExperimental=GL_TRUE;
    GLenum glewInitResult = glewInit();

    errorCheckFunc(__FILE__, __LINE__);
    if (GLEW_OK != glewInitResult) {
      cerr << "Error: " << glewGetErrorString(glewInitResult) << endl;
      exit(EXIT_FAILURE);
    }
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl << endl;

    initialize();


}

void volWidget::initialize() {

    cameraTrackball = new Trackball;
    lightTrackball = new Trackball;
    mesh = new Mesh;
    shader = new Shader("shaders/","phongShader",1);

    //gradShader = new Shader("shaders/","gradShader",1);

/// VOLUME CONFIGURATION---------
///Other datasets:
//    int* size = new int[3];
//    size[0] = 64; size[1] = 64; size[2] = 64;
//    Eigen::Vector3f  dimension;
//    dimension << 1.0, 1.0, 1.0;
//    volume = new Volume("datasets/neghip64x64x64x1.raw",size, dimension);

//    int* size = new int[3];
//    size[0] = 256; size[1] = 256; size[2] = 256;
//    Eigen::Vector3f  dimension;
//    dimension << 1.0, 1.0, 1.0;
//    volume = new Volume("datasets/skull256x1.raw",size, dimension);


//    int* size = new int[3];
//    size[0] = 301; size[1] = 324; size[2] = 56;
//    Eigen::Vector3f  dimension;
//    dimension << 1.0, 1.0, 1.4;
//    volume = new Volume("datasets/lobster301x324x56x1x1x1_4.raw",size, dimension);
///

    volume = new Volume;
    cout << "Volume created" << endl;

    ///The maximum parallellepiped diagonal and volume container dimensions
    volDiagonal = volume->getDiagonal();
    volDimensions = volume->getDimensions();
    volResolution = volume->getTextureResolution();

    //Set number of steps and calculate step size:
    Eigen::Vector3f vRes = volume->getTextureResolution();
    numberOfSteps = max(max(vRes[0], vRes[1]), vRes[2]);
    stepSize = volDiagonal/numberOfSteps;



    //Initialize transfer function:
    initializeTransferFunction();

    //Initializing Matrices
    cameraTrackball->initOpenGLMatrices();
    lightTrackball->initOpenGLMatrices();

    cameraTrackball->initializeBuffers();

    mesh->createQuad();

    //gradShader->initialize();
    shader->initialize();


    //Perspective configuration. Not used yet, because I'm doing it ortogrphically.
    Eigen::Matrix4f projectionMatrix = cameraTrackball->createPerspectiveMatrix(60.0 , (float)currentWidth/(float)currentHeight, 1.0f , 100.0f );
    cameraTrackball->setProjectionMatrix(projectionMatrix);

    ///Trackball Shader generation, not used yet:
    //cameraTrackball->loadShader();

    ///The unit vectors:
    uX << 1.0, 0.0, 0.0, 0.0;
    uY << 0.0, 1.0, 0.0, 0.0;
    uZ << 0.0, 0.0, 1.0, 0.0;

    ///The initial rendPlaneCenter
    updateRendPlane();

    ///Texture binding
    texIndex = volume->bindTexture();
    TFid = transferFunction->bind();

    //For fps counting
    time = new QTime();
    time->start();

    /*//------------------------------

        rootOfDepth = sqrt(volume->getTextureResolution()[2]);
        currentWidth = volume->getTextureResolution()[0]*rootOfDepth;
        currentHeight = volume->getTextureResolution()[1]*rootOfDepth;
        glViewport(0, 0, currentWidth, currentHeight);
        gradArray = new GLubyte[currentWidth*currentHeight];

        for (int i = 0; i < currentWidth*currentHeight; i++){
            gradArray[i] = (unsigned char)1;
        }

        glClearColor(0.0,0.0,0.0,1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        calculateGradient();
        glReadPixels(1, 1, currentWidth, currentHeight, GL_RGBA, GL_UNSIGNED_BYTE, gradArray);

        errorCheckFunc(__FILE__, __LINE__);

        volume->setGradient(gradArray, currentWidth*currentHeight);


    /// ----------------------------- */

        ///Texture binding
        //texIndex = volume->bindTexture();
        //TFid = transferFunction->bind();

    ///Adjust the viewport size
        currentWidth = this->width();
        currentHeight = this->height();
        glViewport(0, 0, currentWidth, currentHeight);

        errorCheckFunc(__FILE__, __LINE__);

    //this->setMouseTracking(true);


}

///NEED TO CREATE THIS GRADSHADER
void volWidget::calculateGradient(){

    gradShader -> enable();
    gradShader->setUniform("volumeTexture", texIndex);
    gradShader->setUniform("volumeResolution", &volResolution[0], 3, 1);
    gradShader->setUniform("rootOfDepth", rootOfDepth);

    mesh->render();
    glReadPixels(1, 1, currentWidth, currentHeight, GL_RGBA, GL_UNSIGNED_BYTE, gradArray);

    gradShader->disable();
    errorCheckFunc(__FILE__, __LINE__);
}
///-----------------

void volWidget::initializeTransferFunction(){

    float values[256*4];

    values[0] = 0.0;
    values[1] = 0.0;
    values[2] = 0.0;
    values[3] = 0.0;

    for(int i = 1; i<256; i++) {//*4 because of 256 RGBA values
            values[i*4] = 0.0;
            values[i*4+1] = 1.0;
            values[i*4+2] = 1.0;
            values[i*4+3] = 0.01;
    }

    transferFunction = new Texture();
    cout << "TF initializing" << endl;
    //errorCheckFunc(__FILE__, __LINE__);
    TFuid = transferFunction->create(GL_TEXTURE_1D, GL_RGBA32F, 256, 256, GL_RGBA, GL_FLOAT, values, 256); // "Id =" because...
                                                        //...this funcion returns a GLuint value that represents the texture ID.
    //errorCheckFunc(__FILE__, __LINE__);
    cout << "TF created." << endl;
    transferFunction->setTexParameters(GL_CLAMP, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR);
    cout << "TF parameters set." << endl;

    cout<<"TF successfully created!"<<endl;

    errorCheckFunc(__FILE__, __LINE__);

}

void volWidget::resetTransferFunction(int a, int b, int c, int d){

    float values[256*4];

    values[0] = 0.0;
    values[1] = 0.0;
    values[2] = 0.0;
    values[3] = 0.0;

    if (a > 0){
        for(int i = 0; i < a; i++){
            values[i*4] = 0.0;
            values[i*4+1] = 0.0;
            values[i*4+2] = 0.0;
            values[i*4+3] = 0.0;
        }
    }

    float dividerAB = 1.0*(b-a);
    for(int i = a; i<b; i++) {
            values[i*4] = 1.0;
            values[i*4+1] = 1.0*((i-a)/dividerAB);
            values[i*4+2] = 1.0*((i-a)/dividerAB);
            values[i*4+3] = 0.01*((i-a)/dividerAB);
    }

    for(int i = b; i < c; i++){
        values[i*4] = 1.0;
        values[i*4+1] = 1.0;
        values[i*4+2] = 1.0;
        values[i*4+3] = 0.05;
    }

    float dividerCD = 1.0*(d-c);
    for(int i = c; i < d; i++){
        values[i*4] = 1.0*((i-c)/dividerCD);
        values[i*4+1] = 1.0;
        values[i*4+2] = 1.0;
        values[i*4+3] = 0.01*((i-c)/dividerCD);
    }

    if (d < 256) {
        for(int i = d; i < 256; i++){
            values[i*4] = 0.0;
            values[i*4+1] = 0.0;
            values[i*4+2] = 0.0;
            values[i*4+3] = 0.0;
        }
    }

    transferFunction->unbind();
    delete transferFunction;


    transferFunction = new Texture();
    cout << "TF initializing" << endl;
    errorCheckFunc(__FILE__, __LINE__);
    TFuid = transferFunction->create(GL_TEXTURE_1D, GL_RGBA32F, 256, 256, GL_RGBA, GL_FLOAT, values, 256); // "Id =" because...
                                                        //...this funcion returns a GLuint value that represents the texture ID.
    errorCheckFunc(__FILE__, __LINE__);
    cout << "TF created." << endl;
    transferFunction->setTexParameters(GL_CLAMP, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR);
    cout << "TF parameters set." << endl;

    cout<<"TF successfully created!"<<endl;
    TFid = transferFunction->bind();

    errorCheckFunc(__FILE__, __LINE__);

}

void volWidget::paintGL(void) {

    makeCurrent();

    fps = 1000/(time->restart()+1);
    cout<<fps<<endl;

    GLint* dims;
    //glGetIntegerv(GL_MAX_VIEWPORT_DIMS,dims);
    errorCheckFunc(__FILE__, __LINE__);

    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //cout << this->width() << "w | " << this->height() << "h" << endl;
    //cout << dims[0] << " " << dims[1] << endl;
    errorCheckFunc(__FILE__, __LINE__);
    draw();

}

void volWidget::draw(void)
{
    //Enable the shader (pretty obvious)
    shader->enable();
    updateRendPlane();
    updateUnitVectors();

    //SUPER COUT
    //cout<<"rPC: "<<rendPlaneCenter<< endl << "diag: "<<volDiagonal<<endl<<uX<<endl << uY<<endl<<uZ<< endl<< volDimensions <<endl;

    //Set the uniforms
    shader->setUniform("volumeTexture", texIndex);
    shader->setUniform("transferFunction", TFid);
    shader->setUniform("textureDepth", volume->getTextureDepth());
    shader->setUniform("screenWidth", this->width());
    shader->setUniform("screenHeight",this->height());
    shader->setUniform("numberOfSteps", numberOfSteps);
    shader->setUniform("stepSize", stepSize);
    shader->setUniform("diagonal", volDiagonal);
    shader->setUniform("uX", &uX[0], 4, 1);
    shader->setUniform("uY", &uY[0], 4, 1);
    shader->setUniform("uZ", &uZ[0], 4, 1);
    shader->setUniform("rendPlaneCenter", &rendPlaneCenter[0], 4, 1);
    shader->setUniform("volDimensions", &volDimensions[0], 3, 1);

    shader->setUniform("layer", curLayer);

    //Mesh Rendering:
    mesh->render();
    //cout << "Mesh rendered" << endl;

    shader->disable();

    //cout << rendPlaneCenter << endl;

    errorCheckFunc(__FILE__, __LINE__);
}

void volWidget::mousePressEvent(QMouseEvent *event){
    Eigen::Vector2f screenPos;
    ///screenPos << event->pos().x()/256.0, (256-(event->pos().y())/256.0);
    screenPos << ((event->pos().x()/(float)this->width())*2.0)-1.0, (2.0*(((float)this->height()-(event->pos().y()))/(float)this->height()))-1.0;
    cameraTrackball->setInitialRotationPosition(screenPos);
    cameraTrackball->beginRotation();
    cout<<"rotation began"<<endl;
}

void volWidget::mouseReleaseEvent(QMouseEvent *event){
    Eigen::Vector2f screenPos;
    screenPos << ((event->pos().x()/(float)this->width())*2.0)-1.0, (2.0*(((float)this->height()-(event->pos().y()))/(float)this->height()))-1.0;
    if(cameraTrackball->isRotating()) {
        cameraTrackball->endRotation();
        cout<<"rotation ended"<<endl;
        cameraTrackball->setInitialRotationPosition(screenPos);
    }
}

void volWidget::mouseMoveEvent(QMouseEvent *event){

    //cout<<"inside"<< ins << endl;
    //qDebug() << event->pos();
    //ins += 1;

    setFocus();

    Eigen::Vector2f screenPos;
    screenPos << ((event->pos().x()/(float)this->width())*2.0)-1.0, (2.0*(((float)this->height()-(event->pos().y()))/(float)this->height()))-1.0;
    //cout<<screenPos<<endl;
    setLayer(screenPos[0]);

    if (cameraTrackball->isRotating()){
        cameraTrackball->setFinalRotationPosition(screenPos);
        cameraTrackball->rotateCamera();
        cameraTrackball->setInitialRotationPosition(screenPos);

        //updateUnitVectors();
        //updateRendPlane();
        this->update();

        //cout<<"Rotated"<<endl<<rendPlaneCenter<<endl;
    }

        //cameraTrackball->setFinalRotationPosition(this->mouseReleaseEvent());
        //cameraTrackball->calculateRotation();


}

void volWidget::updateRendPlane(){
    //Eigen::Vector3f cameraPos;
    //cameraPos = cameraTrackball->getCenter();
    rendPlaneCenter << cameraTrackball->getCenter(), 0.0;
    rendPlaneCenter.normalize();
    rendPlaneCenter << (volDiagonal/2.0)*rendPlaneCenter;

}

void volWidget::setLayer(float layer){
    curLayer = layer;
}

void volWidget::updateUnitVectors(){
    Eigen::Affine3f projection, model, view, final;
    //projection = cameraTrackball->getProjectionMatrix();
    //model = cameraTrackball->getModelMatrix();
    view = cameraTrackball->getViewMatrix();
    final = view.inverse();
    uX << final.rotation() * Eigen::Vector3f(1.0,0.0,0.0) ,0.0;
    uY << final.rotation() * Eigen::Vector3f(0.0,1.0,0.0) ,0.0;
    uZ << final.rotation() * Eigen::Vector3f(0.0,0.0,1.0) ,0.0;
}

void volWidget::keyPressEvent(QKeyEvent *keyevent){
    if (keyevent->key() == Qt::Key_F5){
        shader->reloadShaders();
        cout << "F5 pressed, reloading shader...";
    }

    if (keyevent->key() == Qt::Key_F6){
        resetTransferFunction(8, 64, 240, 256);
        cout << "F6 pressed, reseting transfer function...";
    }
}
