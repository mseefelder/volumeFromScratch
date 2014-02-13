#include "volwidget.h"

volWidget::volWidget(QWidget *parent) : QGLWidget(parent)
{
    //cameraTrackball = new Trackball;
    //lightTrackball = new Trackball;
    //mesh = new Mesh;
    //shader = new Shader("shaders/","shader",0);
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
    /*
    Volume volume;
    cout << "Volume created" << endl;
    */
    cameraTrackball = new Trackball;
    lightTrackball = new Trackball;
    mesh = new Mesh;
    shader = new Shader("shaders/","phongShader",0);

    //Initializing Matrices
    cameraTrackball->initOpenGLMatrices();
    lightTrackball->initOpenGLMatrices();
    Eigen::Matrix4f projectionMatrix = cameraTrackball->createPerspectiveMatrix(60.0 , (float)currentWidth/(float)currentHeight, 1.0f , 100.0f );
    cameraTrackball->setProjectionMatrix(projectionMatrix);

    mesh->createQuad();

    cameraTrackball->initializeBuffers();

    shader->initialize();

    //Trackball Shader generation:
    cameraTrackball->loadShader();
    errorCheckFunc(__FILE__, __LINE__);


}

void volWidget::readOutput(QImage &potentialImage, QImage &spikeImage) {
    //snn->writeOutputImages(potentialImage, spikeImage);
}

void volWidget::paintGL(void) {
    makeCurrent();
    //if (snn) {
    //    snn->render();
    //}


    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
    //glfwSwapBuffers();
    //double currentTime = glfwGetTime();
    //int fps = 1/(currentTime-oldTime);

}

void volWidget::draw(void)
{

    //The matrices multiplication order is: Projection * View * Model * Vertex.

    shader->enable();

    //Define standard color for mesh rendering:
    //Eigen::Vector4f colorVector;
    //colorVector << 1.0, 1.0, 1.0, 1.0;

    //Setting Uniforms:
    //shader->setUniform("in_Color",colorVector.data(),4,1);
    errorCheckFunc(__FILE__, __LINE__);
    //shader->setUniform("projectionMatrix", cameraTrackball->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
    errorCheckFunc(__FILE__, __LINE__);
    //shader->setUniform("modelMatrix",cameraTrackball->getModelMatrix().data(), 4, GL_FALSE, 1);
    errorCheckFunc(__FILE__, __LINE__);
    //shader->setUniform("viewMatrix",cameraTrackball->getViewMatrix().data(), 4, GL_FALSE, 1);
    errorCheckFunc(__FILE__, __LINE__);
    //Eigen::Matrix3f lightViewMatrix = lightTrackball->getViewMatrix().linear();
    errorCheckFunc(__FILE__, __LINE__);
    //shader->setUniform("lightViewMatrix",lightViewMatrix.data(),3,GL_FALSE,1);
    errorCheckFunc(__FILE__, __LINE__);

    //Mesh Rendering:
    mesh->render();

    shader->disable();

    errorCheckFunc(__FILE__, __LINE__);

    //Drawing the Trackball:
    //cameraTrackball->render();

    //Draw AntTweakBar menus:
    //TwDraw();

    errorCheckFunc(__FILE__, __LINE__);
}
