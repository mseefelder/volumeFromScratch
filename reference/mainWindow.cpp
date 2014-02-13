#include "mainWindow.hpp"

void MainWindow::initialize() {

	GLenum glewInitResult;
	cameraTrackball = new Trackball;
	lightTrackball = new Trackball;

	//GLFW Window Initialization:
	initWindow();

	//Glew Initialization:
	glewInitResult = glewInit();

	//Check Glew Initialization:
	if (GLEW_OK != glewInitResult) {
		cerr << "Error: " << glewGetErrorString(glewInitResult) << endl;
		exit(EXIT_FAILURE);
	}
	errorCheckFunc(__FILE__, __LINE__);

	//Enable Depth Testing:
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	errorCheckFunc(__FILE__, __LINE__);
 
	//Enable Culling:
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	errorCheckFunc(__FILE__, __LINE__);
 
	//Init OpenGL Matrices:
	cameraTrackball->initOpenGLMatrices();
	lightTrackball->initOpenGLMatrices();
	cameraTrackball->usePerspectiveMatrix(60.0 , (float)currentWidth/(float)currentHeight, 1.0f , 100.0f );

	//Creating VBOs:
	mesh->createBuffers();
	cameraTrackball->initializeBuffers();

	initializeAntTweakBar();

	phongShader = new Shader("shaders/","phongShader",0);
	phongShader->initialize();

	//Trackball Shader generation:
	cameraTrackball->loadShader();

	//Load model:
	mesh->loadObjFile("models/stay-puff-poisson.obj");
	Eigen::Vector3f translation = -mesh->getObjectCenter();
	cameraTrackball->applyScaleToModelMatrix(mesh->getScale());
	cameraTrackball->translateModelMatrix(translation);

	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;
}

void MainWindow::initWindow() {

	//GLFW Initialization:
	if (!glfwInit()) {
		cerr << "Error: Could not initialize GLFW Window" << endl;
		exit(EXIT_FAILURE);
	}

	//Open Window:
	if (!glfwOpenWindow(currentWidth,currentHeight,0,0,0,8,8,8,GLFW_WINDOW)) {
		cerr << "Error: Could not open GLFW Window" << endl;
		exit(EXIT_FAILURE);
	}

	//Setting window title and initial position:
	glfwSetWindowTitle(WINDOW_TITLE_PREFIX);
	glfwSetWindowPos(currentPosX,currentPosY);

	//Enable Key Repeating when a key is held down:
	glfwEnable(GLFW_KEY_REPEAT);

}

void MainWindow::initializeAntTweakBar() {

	//AntTweakBar Initialization:
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(currentWidth, currentHeight);
	menu = TwNewBar("menu");

	//Defining default values for menu:
	std::string parameters;
	parameters = "menu label='Menu' color='51 204 255' contained=true text=dark fontresizable=false valueswidth=60 "
				 "size='200 200' position='0 0'";
	TwDefine(parameters.c_str());

	//Getting Pointers to link to buttons:
	bool* drawTrackball = cameraTrackball->getDrawTrackballFlag();

	//Getting light Direction:
	float* lightDirection = lightTrackball->getQuaternionVector();
	
	//Standard Menu:
	TwAddVarRO(menu, "quaternion", TW_TYPE_QUAT4F, lightDirection, 
				" label='Light Direction' help='Light Direction Representation' opened=true arrow='0 0 -1'");
	TwAddVarRW(menu,"drawTrackball",TW_TYPE_BOOLCPP, drawTrackball, 
			   " label='Draw Position Trackball' key=t help='If checked, draw trackball representation on screen.'");
	//TwAddButton(menu, "reloadShader", reloadShaders, NULL, 
	//			" label='Reload Shaders' key=r help='Reload the current shader from the file.'");
	
}

void MainWindow::draw(void)
{
	//The matrices multiplication order is: Projection * View * Model * Vertex.

	phongShader->enable();

	//Define standard color for mesh rendering:
	Eigen::Vector4f colorVector;
	colorVector << 0.686, 0.933, 0.933, 1.0;

	//Setting Uniforms:
	phongShader->setUniform("in_Color",colorVector.data(),4,1);
	phongShader->setUniform("projectionMatrix", cameraTrackball->getProjectionMatrix().data(), 4 ,GL_FALSE, 1);
	phongShader->setUniform("modelMatrix",cameraTrackball->getModelMatrix().data(), 4, GL_FALSE, 1);
	phongShader->setUniform("viewMatrix",cameraTrackball->getViewMatrix().data(), 4, GL_FALSE, 1);
	Eigen::Matrix3f lightViewMatrix = lightTrackball->getViewMatrix().linear();
	phongShader->setUniform("lightViewMatrix",lightViewMatrix.data(),3,GL_FALSE,1);
	errorCheckFunc(__FILE__, __LINE__);
	
	//Mesh Rendering:
	mesh->render();

	phongShader->disable();

	errorCheckFunc(__FILE__, __LINE__);

	//Drawing the Trackball:
	cameraTrackball->render();

	//Draw AntTweakBar menus:
	TwDraw();

	errorCheckFunc(__FILE__, __LINE__);
}

void MainWindow::mainLoop(void) {
	double oldTime = glfwGetTime();
	while(glfwGetWindowParam(GLFW_OPENED)) {
		if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS) {
			break;
		}
		++frameCount;
		glClearColor(1.0,1.0,1.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw();
		glfwSwapBuffers();
		double currentTime = glfwGetTime();
		int fps = 1/(currentTime-oldTime);
	}
}

void MainWindow::resize(int width, int height) {

	setCurrentWidth(width);
	setCurrentHeight(height);
	glViewport(0, 0, width, height);

}

Eigen::Vector2f MainWindow::convertToNormalizedDeviceCoordinates(Eigen::Vector2i position) {
	float x = (float)position[0]/(float)(currentWidth/2.0);
	x = x-1;

	float y = (float)position[1]/(float)(currentHeight/2.0);
	y = 1-y;

	Eigen::Vector2f ret(x,y);
	return ret;
}

void MainWindow::createLightEffect() {

	lightEffect = new lightDistance("lightDistance");
	lightEffect->initialize();

}

//DO THE GET LIGHT EFFECT