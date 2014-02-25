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

    glewExperimental=TRUE;
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

    //ins = 0;

    cameraTrackball = new Trackball;
    lightTrackball = new Trackball;
    mesh = new Mesh;
    shader = new Shader("shaders/","phongShader",0);

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
//    volume = new Volume("datasets/bonsai256x1.raw",size, dimension);
///

    volume = new Volume;
    cout << "Volume created" << endl;

    //Initialize transfer function:
    initializeTransferFunction();

    //Initializing Matrices
    cameraTrackball->initOpenGLMatrices();
    lightTrackball->initOpenGLMatrices();
    ///Not using this part because I'm doing it Ortographic style:
    //cameraTrackball->setProjectionMatrix(projectionMatrix);
    //Eigen::Matrix4f projectionMatrix = cameraTrackball->createPerspectiveMatrix(60.0 , (float)currentWidth/(float)currentHeight, 1.0f , 100.0f );

    cameraTrackball->initializeBuffers();

    mesh->createQuad();

    shader->initialize();

    ///Adjust the viewport size
    glViewport(0, 0, this->width(), this->height());

    ///Trackball Shader generation, not used yet:
    //cameraTrackball->loadShader();

    ///The maximum parallellepiped diagonal and volume container dimensions
    volDiagonal = volume->getDiagonal();
    volDimensions = volume->getDimensions();

    ///The unit vectors:
    uX << 1.0, 0.0, 0.0, 0.0;
    uY << 0.0, 1.0, 0.0, 0.0;
    uZ << 0.0, 0.0, 1.0, 0.0;

    ///The initial rendPlaneCenter
    updateRendPlane();

    ///Texture stuff
    texIndex = volume->bindTexture();
    TFid = transferFunction->bind();

    errorCheckFunc(__FILE__, __LINE__);

    this->setMouseTracking(true);


}

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
    errorCheckFunc(__FILE__, __LINE__);
    TFuid = transferFunction->create(GL_TEXTURE_1D, GL_RGBA32F, 256, 256, GL_RGBA, GL_FLOAT, values, 256); // "Id =" because...
                                                        //...this funcion returns a GLuint value that represents the texture ID.
    errorCheckFunc(__FILE__, __LINE__);
    cout << "TF created." << endl;
    transferFunction->setTexParameters(GL_CLAMP, GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR);
    cout << "TF parameters set." << endl;

    cout<<"TF successfully created!"<<endl;

//    glEnable(GL_TEXTURE_1D);
//    glBindTexture(GL_TEXTURE_1D,transferFunction);//Do I need to enable GL_TEXTURE_1D?
//    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 256, 0, GL_RGBA, GL_FLOAT, &values[0]);
//    glBindTexture(GL_TEXTURE_1D,0);
//    glDisable(GL_TEXTURE_1D);
    errorCheckFunc(__FILE__, __LINE__);

}

void volWidget::paintGL(void) {

    makeCurrent();
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

    //The matrices multiplication order is: Projection * View * Model * Vertex.

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

    shader->setUniform("diagonal", volDiagonal);
    shader->setUniform("uX", &uX[0], 4, 1);
    shader->setUniform("uY", &uY[0], 4, 1);
    shader->setUniform("uZ", &uZ[0], 4, 1);
    shader->setUniform("rendPlaneCenter", &rendPlaneCenter[0], 4, 1);
    shader->setUniform("volDimensions", &volDimensions[0], 3, 1);

    shader->setUniform("layer", curLayer);

    //Mesh Rendering:
    mesh->render();
    cout << "Mesh rendered" << endl;

    shader->disable();

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

    Eigen::Vector2f screenPos;
    screenPos << ((event->pos().x()/(float)this->width())*2.0)-1.0, (2.0*(((float)this->height()-(event->pos().y()))/(float)this->height()))-1.0;
    cout<<screenPos<<endl;
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
    rendPlaneCenter = (volDiagonal/2.0)*rendPlaneCenter;

}

void volWidget::setLayer(float layer){
    curLayer = layer;
}

void volWidget::updateUnitVectors(){
    Eigen::Affine3f projection, model, view, final;
    projection = cameraTrackball->getProjectionMatrix();
    model = cameraTrackball->getModelMatrix();
    view = cameraTrackball->getViewMatrix();
    final = view.inverse();
    uX << final.rotation() * Eigen::Vector3f(1.0,0.0,0.0) ,0.0;
    uY << final.rotation() * Eigen::Vector3f(0.0,1.0,0.0) ,0.0;
    uZ << final.rotation() * Eigen::Vector3f(0.0,0.0,1.0) ,0.0;
}
