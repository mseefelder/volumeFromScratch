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

    cameraTrackball = new Trackball;
    lightTrackball = new Trackball;
    mesh = new Mesh;
    shader = new Shader("shaders/","phongShader",0);
    volume = new Volume;
    cout << "Volume created" << endl;

    //Initializing Matrices
    cameraTrackball->initOpenGLMatrices();
    lightTrackball->initOpenGLMatrices();
    Eigen::Matrix4f projectionMatrix = cameraTrackball->createPerspectiveMatrix(60.0 , (float)currentWidth/(float)currentHeight, 1.0f , 100.0f );
    cameraTrackball->setProjectionMatrix(projectionMatrix);

    mesh->createQuad();

    cameraTrackball->initializeBuffers();

    shader->initialize();

    glViewport(0, 0, this->width(), this->height());

    //Trackball Shader generation:
    cameraTrackball->loadShader();
    errorCheckFunc(__FILE__, __LINE__);


}

void volWidget::paintGL(void) {

    makeCurrent();
    GLint* dims;
    //glGetIntegerv(GL_MAX_VIEWPORT_DIMS,dims);
    errorCheckFunc(__FILE__, __LINE__);

    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cout << this->width() << "w | " << this->height() << "h" << endl;
    //cout << dims[0] << " " << dims[1] << endl;
    errorCheckFunc(__FILE__, __LINE__);
    draw();

}

void volWidget::draw(void)
{

    //The matrices multiplication order is: Projection * View * Model * Vertex.

    //Enable the shader (pretty obvious)
    shader->enable();

    GLint texIndex;
    texIndex = volume->bindTexture();

    shader->setUniform("volumeTexture", texIndex);

    //Mesh Rendering:
    mesh->render();
    cout << "Mesh rendered" << endl;

    shader->disable();

    errorCheckFunc(__FILE__, __LINE__);
}
