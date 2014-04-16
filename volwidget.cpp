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

//    int* size = new int[3];
//   size[0] = 150; size[1] = 150; size[2] = 276;
//   Eigen::Vector3f  dimension;
//    dimension << 1.5, 1.5, 2.76;
//    volume = new Volume("datasets/mouse150x150x276_16u.raw",size, dimension); //16bits unsigned


    cout << "volWidget initialize"<<endl;
    volume = new Volume;
    //volume = new Volume(true);
    cout << "Volume created" << endl;

    mesh = new Mesh;
    mesh->createQuad();

    shader = new Shader("shaders/","phongShader",1);
    shader->initialize();

    ///The maximum parallellepiped diagonal and volume container dimensions
    volDiagonal = volume->getDiagonal();
    volDimensions = volume->getDimensions();
    volResolution = volume->getTextureResolution();

    ///Set number of steps and calculate step size:
    Eigen::Vector3f vRes = volume->getTextureResolution();
    numberOfSteps = max(max(vRes[0], vRes[1]), vRes[2]);
    stepSize = volDiagonal/numberOfSteps;

    ///Initialize transfer function and the jittering texture:
    initializeTransferFunction();
    initializeJitteringTexture();

    ///Texture binding
    texIndex = volume->bindTexture();
    TFid = transferFunction->bind();
    JTid = jitteringTexture->bind();

    cout << "Textures bound" << endl;

    ///Trackballs
    cameraTrackball = new Trackball;
    lightTrackball = new Trackball;
    trackballs = new Trackball*[2];
    trackballs[0] = cameraTrackball;
    trackballs[1] = lightTrackball;
    tBallIndex = 0; //currently 'active' trackball, used for rotation

    cout << "Trackballs set" << endl;

    ///Initializing Matrices
    cameraTrackball->initOpenGLMatrices();
    lightTrackball->initOpenGLMatrices();

    cout << "Matrices initialized" << endl;

    ///Trackball Shader an buffer generation, not used yet:
    //cameraTrackball->initializeBuffers();
    //cameraTrackball->loadShader();

    ///Perspective configuration. Not used yet, because I'm doing it ortogrphically.
    Eigen::Matrix4f projectionMatrix = cameraTrackball->createPerspectiveMatrix(60.0 , (float)currentWidth/(float)currentHeight, 1.0f , 100.0f );
    cameraTrackball->setProjectionMatrix(projectionMatrix);

    ///The unit vectors:
    uX << 1.0, 0.0, 0.0, 0.0;
    uY << 0.0, 1.0, 0.0, 0.0;
    uZ << 0.0, 0.0, 1.0, 0.0;

    ///The initial rendPlaneCenter
    updateRendPlane();

    ///For fps counting
    time = new QTime();
    time->start();

    ///Adjust the viewport size
    currentWidth = this->width();
    currentHeight = this->width(); //this->height();
    glViewport(0, 0, currentWidth, currentHeight);

    errorCheckFunc(__FILE__, __LINE__);

    cout << "Initialized!" << endl;
}

void volWidget::initializeTransferFunction(){

    float values[256*4];

    ///Make the Zeroes transparent
    values[0] = 0.0;
    values[1] = 0.0;
    values[2] = 0.0;
    values[3] = 0.0;

    for(int i = 1; i<256; i++) { //*4 because of 256 RGBA values
        values[i*4] = 1.0;
        values[i*4+1] = 0.75;
        values[i*4+2] = 0.0;
        values[i*4+3] = 0.01;
    }

    transferFunction = new Texture();
    TFuid = transferFunction->create(GL_TEXTURE_1D, GL_RGBA32F, 256, 256, GL_RGBA, GL_FLOAT, values, 256);
    cout << "TF created." << endl;
    transferFunction->setTexParameters(GL_CLAMP, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR);
    cout<<"TF successfully created!"<<endl;

    //delete values;

    errorCheckFunc(__FILE__, __LINE__);
}

void volWidget::initializeJitteringTexture(){
    int size = this->width();
    unsigned char* values = new unsigned char[size*size];
    for (int i = 0; i<(size*size); i++)
        values[i] = 255.0*rand()/(float)RAND_MAX;

    jitteringTexture = new Texture();
    JTuid = jitteringTexture->create(GL_TEXTURE_2D, GL_R8, size, size, GL_RED, GL_UNSIGNED_BYTE, values, 0);
    jitteringTexture->setTexParameters(GL_REPEAT, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);

    delete values;
}

void volWidget::paintGL(void) {

    makeCurrent();
    fps = 1000/(time->restart()+1);
    //cout<<fps<<" - ";
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    errorCheckFunc(__FILE__, __LINE__);

    draw();

}

void volWidget::draw(void)
{
    ///Enable the shader (pretty obvious)
    shader->enable();
    updateRendPlane();
    updateUnitVectors();

    ///Texture binding: NOT HERE, because the texture manager keeps binding them
    ///to different units until there are no more units
    //texIndex = volume->bindTexture();
    //TFid = transferFunction->bind();

    //Set the uniforms
    Eigen::Matrix3f lightViewMatrix = lightTrackball->getViewMatrix().linear();
    shader->setUniform("lightViewMatrix",lightViewMatrix.data(),3,GL_FALSE,1);
    shader->setUniform("volumeTexture", texIndex);
    shader->setUniform("transferFunction", TFid);
    shader->setUniform("jitteringTexture", JTid);
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

    ///Mesh Rendering:
    mesh->render();

    shader->disable();

    errorCheckFunc(__FILE__, __LINE__);
}

void volWidget::mousePressEvent(QMouseEvent *event){
    Eigen::Vector2f screenPos;
    screenPos << ((event->pos().x()/(float)this->width())*2.0)-1.0, (2.0*(((float)this->height()-(event->pos().y()))/(float)this->height()))-1.0;
    trackballs[tBallIndex]->setInitialRotationPosition(screenPos);
    trackballs[tBallIndex]->beginRotation();
    //cout<<"rotation began"<<endl;
}

void volWidget::mouseReleaseEvent(QMouseEvent *event){
    Eigen::Vector2f screenPos;

    screenPos << ((event->pos().x()/(float)this->width())*2.0)-1.0, (2.0*(((float)this->height()-(event->pos().y()))/(float)this->height()))-1.0;
    if(trackballs[tBallIndex]->isRotating()) {
        trackballs[tBallIndex]->endRotation();
        //cout<<"rotation ended"<<endl;
        trackballs[tBallIndex]->setInitialRotationPosition(screenPos);

        if (tBallIndex == 1)
            cout<<endl<< lightTrackball->getViewMatrix().linear()*Eigen::Vector3f(0.0,0.0,1.0) <<endl;
    }
}

void volWidget::mouseMoveEvent(QMouseEvent *event){

    setFocus();

    Eigen::Vector2f screenPos;

    screenPos << ((event->pos().x()/(float)this->width())*2.0)-1.0, (2.0*(((float)this->height()-(event->pos().y()))/(float)this->height()))-1.0;
    //setLayer(screenPos[0]);

    if (trackballs[tBallIndex]->isRotating()){
        trackballs[tBallIndex]->setFinalRotationPosition(screenPos);
        trackballs[tBallIndex]->rotateCamera();
        trackballs[tBallIndex]->setInitialRotationPosition(screenPos);

        this->update();
    }
}

void volWidget::updateRendPlane(){
    rendPlaneCenter << cameraTrackball->getCenter(), 0.0;
    rendPlaneCenter.normalize();
    rendPlaneCenter << (volDiagonal/2.0)*rendPlaneCenter;
}

void volWidget::updateUnitVectors(){
    Eigen::Affine3f projection, model, view, final;
    view = cameraTrackball->getViewMatrix();
    final = view.inverse();
    uX << final.rotation() * Eigen::Vector3f(1.0,0.0,0.0) ,0.0;
    uY << final.rotation() * Eigen::Vector3f(0.0,1.0,0.0) ,0.0;
    uZ << final.rotation() * Eigen::Vector3f(0.0,0.0,1.0) ,0.0;
}

void volWidget::keyPressEvent(QKeyEvent *keyevent){
    ///Reload Shader
    if (keyevent->key() == Qt::Key_F5){
        shader->reloadShaders();
        cout << "F5 pressed, reloading shader...";
    }
    ///Reset Transfer Function
    if (keyevent->key() == Qt::Key_F6){
        resetTransferFunction(8, 64, 240, 256);
        cout << "F6 pressed, reseting transfer function...";
    }
    ///Enable light rotation
    if (keyevent->key() == Qt::Key_Control){
        tBallIndex = 1;
    }
}

void volWidget::keyReleaseEvent(QKeyEvent *keyevent){
    if (keyevent->key() == Qt::Key_Control){
        tBallIndex = 0;
    }
}

void volWidget::reloadShader(){
    shader->reloadShaders();
}

void volWidget::resetTransferFunction(int a, int b, int c, int d){

    ///To understand what's happenning check "initializeTransferFunction()"

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
        values[i*4+3] = 0.05*10.0;
    }

    float dividerCD = 1.0*(d-c);
    for(int i = c; i < d; i++){
        values[i*4] = 1.0*((i-c)/dividerCD);
        values[i*4+1] = 1.0;
        values[i*4+2] = 1.0;
        values[i*4+3] = 0.01*((i-c)/dividerCD)*5.0;
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
    cout << "TF resetting" << endl;
    errorCheckFunc(__FILE__, __LINE__);
    TFuid = transferFunction->create(GL_TEXTURE_1D, GL_RGBA32F, 256, 256, GL_RGBA, GL_FLOAT, values, 256);
    transferFunction->setTexParameters(GL_CLAMP, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR);
    cout<<"TF successfully reset!"<<endl;
    TFid = transferFunction->bind();

    errorCheckFunc(__FILE__, __LINE__);

}

/*
void volWidget::setLayer(float layer){
    curLayer = layer;
}

*/
