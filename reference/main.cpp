#include "mainWindow.hpp"

MainWindow *window;	

void GLFWCALL reshapeFunction(int width, int height) {

	Trackball* cameraTrackball = window->getCameraTrackball();

	window->resize(width, height);

	//Projection Matrix definition:
	cameraTrackball->usePerspectiveMatrix(60.0 , (float)width/(float)height, 1.0f , 100.0f );
	cameraTrackball->useTrackballOrthographicMatrix(-1.0,1.0,-1.0,1.0,1.0f,100.0f);
 
}

void GLFWCALL mouseButtonFunction(int button, int action) {
	if(!TwEventMouseButtonGLFW(button,action)) {  
		Trackball* cameraTrackball = window->getCameraTrackball();
		Trackball* lightTrackball = window->getLightTrackball();

		int x, y;
		glfwGetMousePos(&x, &y);
	
		//Position vector in [0,viewportWidth] domain:
		Eigen::Vector2i pos(x,y);

		//Convert the mouse coordinates vector to normalized device coordinates system
		Eigen::Vector2f positionInTrackballSystem;
		positionInTrackballSystem = window->convertToNormalizedDeviceCoordinates(pos);

		//Camera Translating:
		if( (button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS) && (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS)) {
			cameraTrackball->setInitialTranslationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
			cameraTrackball->beginTranslation();
		}

		//Camera Rotating:
		else if( (button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {
			cameraTrackball->setInitialRotationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
			cameraTrackball->beginRotation();
		}
		if( (button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_RELEASE) ) {
			if(cameraTrackball->isRotating()) {
				cameraTrackball->endRotation();
			}
			else if(cameraTrackball->isTranslating()) {
				cameraTrackball->endTranslation();
			}
		}

		//Light Rotating:
		if( (button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_PRESS) ) {
			lightTrackball->setInitialRotationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
			lightTrackball->beginRotation();

		}
		if( (button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_RELEASE) ) {
			lightTrackball->endRotation();
		}
	}
}

void GLFWCALL mouseWheelFunction(int pos) {

	if(!TwEventMouseWheelGLFW(pos)) {  
		Trackball* cameraTrackball = window->getCameraTrackball();

		//Wheel Up:
		//Precise Zooming:
		if( (pos > 0) && (glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS) ) {
			cameraTrackball->increaseZoom(1.01);
		}
		//Fast Zooming:
		else if(pos > 0) {
			cameraTrackball->increaseZoom(1.05);
		}

		//Wheel Down:
		//Precise Zooming:	
		if( (pos < 0) && (glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS)) {
			cameraTrackball->decreaseZoom(1.01);
		}
		//Fast Zooming:
		else if(pos < 0) {
			cameraTrackball->decreaseZoom(1.05);
		}

		glfwSetMouseWheel(0);
	}
}

void GLFWCALL  mousePosFunction(int x, int y) {
	if(!TwEventMousePosGLFW(x, y)) {
		//cout << "x: " << x << " y: " << y << endl;
 
		Trackball* cameraTrackball = window->getCameraTrackball();
		Trackball* lightTrackball = window->getLightTrackball();

		//Position vector in [0,viewportWidth] domain:
		Eigen::Vector2i pos(x,y);

		//Convert to Normalized Device Coordinates System:
		Eigen::Vector2f positionInTrackballSystem;
		positionInTrackballSystem = window->convertToNormalizedDeviceCoordinates(pos);

		//It's important to remember that it only makes sense to calculate a rotation if initial and final positions vector are different.

		//Camera Rotating:
		if(cameraTrackball->isRotating()) {
			Eigen::Vector3f initialPosition = cameraTrackball->getInitialRotationPosition();
			if(positionInTrackballSystem != Eigen::Vector2f(initialPosition[0], initialPosition[1])) {
				cameraTrackball->setFinalRotationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
				cameraTrackball->rotateCamera();
				cameraTrackball->setInitialRotationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);

			}
		}

		//Light Rotating:
		else if(lightTrackball->isRotating()) {
			Eigen::Vector3f initialPosition = lightTrackball->getInitialRotationPosition();
			if(positionInTrackballSystem != Eigen::Vector2f(initialPosition[0], initialPosition[1])) {
				lightTrackball->setFinalRotationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
				lightTrackball->rotateCamera();
				lightTrackball->setInitialRotationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
			}
		}

		//Camera Translating:
		else if(cameraTrackball->isTranslating()) {
			Eigen::Vector2f initialPosition = cameraTrackball->getInitialTranslationPosition();
			if(positionInTrackballSystem != Eigen::Vector2f(initialPosition[0], initialPosition[1])) {
				cameraTrackball->setFinalTranslationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
				cameraTrackball->translateCamera();
				cameraTrackball->setInitialTranslationPosition(positionInTrackballSystem[0],positionInTrackballSystem[1]);
			}
		}
	}
}

void TW_CALL reloadShaders(void *clientData) {

	Shader* shader = window->getPhongShader();
	shader->reloadShaders();

}

int main(int argc, char * argv[]) {
	
	window = new MainWindow();
	window->initialize();

	//Setting Callback functions:
	glfwSetWindowSizeCallback(reshapeFunction);
	glfwSetMousePosCallback(mousePosFunction);
	glfwSetMouseButtonCallback(mouseButtonFunction);
	glfwSetMouseWheelCallback(mouseWheelFunction);

	window->mainLoop();
	window->shutDown();

	delete window;
}
