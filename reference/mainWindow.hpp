#ifndef __MAINWINDOW__
#define __MAINWINDOW__
#define WINDOW_TITLE_PREFIX "Shaders Library Visualization"

#include "trackball.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include <GL/glfw.h>
#include "AntTweakBar.h"
#include <string>


class MainWindow {

	public:

		/**Constructor that takes the window width and height as parameters. Initial window position is (100,100).
		 * @param width The width to be set as viewport Width.
		 * @param height The height to be set as viewport Height.
		**/
		MainWindow(int width = 800, int height = 800): currentWidth(width), currentHeight(height), currentPosX(100), currentPosY(100), windowHandle(0), frameCount(0) {

			mesh = new Mesh;

		}

		///Initializes GLEW, calls window initialization function, enables depth testing and culling, initializes OpenGL Matrices and loads the mesh file.
		void initialize();

		void createLightEffect();

		lightDistance getLightEffect(){
			return lightEffect;
		}

		Shader* getPhongShader() {
			return phongShader;
		}

		/**
		 * Sets the current Viewport Width.
		 * @param width The value to be set as current viewport width.
		**/
		void setCurrentWidth(int width) {
			currentWidth = width;
		}

		/**
		 * Sets the current Viewport Height.
		 * @param height The value to be set as current viewport height.
		**/
		void setCurrentHeight(int height) {
			currentHeight = height;
		}

		/**
		 * Returns a pointer to the Trackball object that represents the Camera Trackball.
		 * @return Returns the pointer to the camera trackball.
		**/
		Trackball* getCameraTrackball() {
			return cameraTrackball;
		}

		/**
		 * Returns a pointer to the Trackball object that represents the Light Trackball.
		 * @return Return the pointer to the light trackball.
		**/
		Trackball* getLightTrackball() {
			return lightTrackball;
		}

		///GLFW Render Function. Increments framecounter, clear color and depth buffers and calls draw function.
		void mainLoop();

		///Terminates GLFW Window.
		void shutDown() {
			glfwTerminate();
			exit(EXIT_SUCCESS);
		}

		///Default Destructor. Detaches current shaders from the shader program, deletes the shaders, mesh buffers, trackballs, windows, effects, terminates antTweakBar and closes GLFW window.
		~MainWindow() {

			//IMPORTANT: THIS SHOULD BE DONE INSIDE SHADERS CLASS, NOT HERE!
			phongShader->deleteShaders();
 
			mesh->deleteBuffers();

			if(cameraTrackball) {
				delete cameraTrackball;
			}

			if(lightTrackball) {
				delete lightTrackball;
			}
	
			if(phongShader) {
				delete phongShader;
			}

			//Delete Mesh:
			if(mesh) {
				delete mesh;
			}

			cout << TwTerminate() << endl << endl;

			glfwCloseWindow();

		}

		/**
		 * Convert a given 2D int vector to Normalized Device Coordinates System.
		 * @param position The position vector to be converted.
		 * @return Returns the position in the normalized device coordinates system.
		**/
		Eigen::Vector2f convertToNormalizedDeviceCoordinates(Eigen::Vector2i position);

		///Resizes the current window.
		void resize(int width, int height);

	private:

		///Initializes all menus and buttons from antTweakBar.
		void initializeAntTweakBar();

		///Trackball Object for the camera position.
		Trackball* cameraTrackball;

		///Trackball object for lights position.
		Trackball* lightTrackball;

		///The Current Shader object to be used in mesh rendering.
		Shader* phongShader;

		///Mesh Object.
		Mesh* mesh;

		///Current Viewport Width measured in pixels.
		int currentWidth;
		
		///Current Viewport Height measured in pixels.
		int currentHeight;

		///Current Viewport Position in X axis in pixels.
		int currentPosX;

		///Current Viewport Position in Y axis in pixels.
		int currentPosY;

		///GLFW Window identification handle.
		int windowHandle;

		///Frame Counter used for FPS measuring.
		unsigned frameCount;

		///Window Title:
		string windowTitle;

		///Initializes GLFW window and sets all the callback functions.
		void initWindow();

		///Handles uniform setting for the mesh, as well as mesh and trackball's rendering.		
		void draw(void);

		///AntTweakBar menu:
		TwBar* menu;

		lightDistance* lightEffect;

};


#endif
