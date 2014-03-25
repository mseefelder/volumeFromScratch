#ifndef __RAYCASTING__
#define __RAYCASTING__

#include "shader.hpp"
#include "trackball.hpp"
#include "effect.hpp"
#include "volume.hpp"

using namespace std;

class RayCasting : public Effect {

	public:

	///Default Constructor. Initializes the shaders, volume, textures, transfer function and the trackball.
	RayCasting(const char* name, Eigen::Vector2i currentSize) {

		effectName = name;
		viewportSize = currentSize;

		viewportWorldSpaceSize << sqrt(3.0), sqrt(3.0);

		//Initializing the depth step:
		depthStep = 0.01;

		//Sets the folder in which the shaders programs are stored.
		setShadersDir("shaders/");

	}

	virtual void initialize();

	void clearTexture2D(GLuint texID);

	void paintTexture2D(GLuint texID, Eigen::Vector4f color);

	void createTexture1D(GLuint &texID);

	void createTexture2D(GLuint &texID);

	/**
	 * Sets the step used to pass through the volume during the raycasting.
	 * @param step The step used to pass through the volume.
	**/
	void setDepthStep(float step) {
		depthStep = step;
	}

	/**
	 * Returns the depth step that is used to pass through the volume during the raycasting.
	 * @return Returns the depth step used to pass through the volume.
	**/
	float getDepthStep() {
		return depthStep;
	}

	Shader* getShader() {
		return shader;
	}

	///Renders the volume using the ray casting.
	virtual void render(Mesh* mesh = NULL, Trackball* cameraTrackball = NULL, Trackball* lightTrackball = NULL);

	~RayCasting() {

		if(volume) {
			delete volume;
		}

		if(quad) {
			delete quad;
		}

		if(textureClearColor) {
			delete [] textureClearColor;
		}

		if(computeShader) {
			delete computeShader;
		}

		if(shader) {
			delete shader;
		}


	}

	private:

	/**
	 * Computes the initial translation necessary in order not to waste computational power ray casting in the empty space, before the volume borders.
	 * @return Returns the vertex that is nearest to the screen position. That vertex will be the starting point for the ray casting.
	**/
	Eigen::Vector4f computeInitialTranslation(Trackball* cameraTrackball);

	/**
	 * Given a point that belongs to the plane, the plane's normal and another arbitrary point, computes the distance between the plane and the point.
	 * @param planePoint The point that belongs to the plane.
	 * @param planeNormal The plane's normal vector.
	 * @param point An arbitrary point to which the distance from the plane will be computed.
	**/
	float computeDistanceFromPointToPlane(Eigen::Vector3f planePoint, Eigen::Vector3f planeNormal, Eigen::Vector3f point);

	///Initializes the transfer function of the Ray Casting.
	void initializeTransferFunction();

	///Shader used for ray-casting computation.
	Shader* computeShader;

	///TexID used for the current color of the volume.
	GLuint currentColorsTexture;

	///TexID used for transfer function computations. Initially a texture1D, later a texture2D.
	GLuint transferFunction;

	///Volume object.
	Volume* volume;

	///Step iteration distance.
	float depthStep;

	///Number of iterations needed to cover the whole volume with the ray casting.
	int numberOfIterations;

	///
	Shader* shader;

	///Quad used to project the final image and visualize the volume rendering.
	Mesh* quad;

	float* textureClearColor;

	Eigen::Affine3f auxViewMatrix;

	///The Size of the viewport in world space size. It's the diagonals of a unit cube, i.e., (sqrt(3),sqrt(3)).
	Eigen::Vector2f viewportWorldSpaceSize;

	///The normal vector to the view plane.
	Eigen::Vector3f viewPlaneNormal;
	

};

#endif
