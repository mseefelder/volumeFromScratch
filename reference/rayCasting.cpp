#include "rayCasting.hpp"

void RayCasting::initialize() {

		//Initialize Shaders:
		computeShader = new Shader(shaders_dir, "rayCasting",1);
		computeShader->initialize();
		shader = new Shader(shaders_dir, "shaderProgram",1);
		shader->initialize();

		//Initialize Volume:
		volume = new Volume();

		//Initialize Textures:
		createTexture2D(currentColorsTexture);
		createTexture1D(transferFunction);

		//Initialize transfer function:
		initializeTransferFunction();

		//Initialize quad:
		quad = new Mesh();
		quad->createQuad();

		//Initialize clear color array:
		int textureClearColorSize = viewportSize[0]*viewportSize[1]*4;
		textureClearColor = new float[textureClearColorSize];
		for (int i = 0; i<textureClearColorSize; i+=4) {
			textureClearColor[i] = 0.0;
			textureClearColor[i+1] = 0.0;
			textureClearColor[i+2] = 0.0;
			textureClearColor[i+3] = 1.0;
		}

}

void RayCasting::initializeTransferFunction() {

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
	glEnable(GL_TEXTURE_1D);
	glBindTexture(GL_TEXTURE_1D,transferFunction);//Do I need to enable GL_TEXTURE_1D?
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 256, 0, GL_RGBA, GL_FLOAT, &values[0]);
	glBindTexture(GL_TEXTURE_1D,0);
	glDisable(GL_TEXTURE_1D);
    errorCheckFunc(__FILE__, __LINE__);
}

void RayCasting::clearTexture2D(GLuint texID) {

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texID);//Do I need to enable GL_TEXTURE_2D?
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewportSize[0], viewportSize[1], 0, GL_RGBA, GL_FLOAT, &textureClearColor[0]);	
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);
    errorCheckFunc(__FILE__, __LINE__);

}

void RayCasting::paintTexture2D(GLuint texID, Eigen::Vector4f color) {

	float* testTextureArray = new float[viewportSize[0]*viewportSize[1]*4];
	for (int i = 0; i<(viewportSize[0]*viewportSize[1]) ; i+=1) {
		testTextureArray[i*4] = color[0];
		testTextureArray[i*4+1] = color[1];
		testTextureArray[i*4+2] = color[2];
		testTextureArray[i*4+3] = color[3];
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewportSize[0], viewportSize[1], 0, GL_RGBA, GL_FLOAT, &testTextureArray[0]);	
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);
    errorCheckFunc(__FILE__, __LINE__);

	delete [] testTextureArray;

}

void RayCasting::createTexture2D(GLuint &texID) {

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1,&texID);
	cout << texID << endl;
	glBindTexture(GL_TEXTURE_2D,texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewportSize[0], viewportSize[1], 0, GL_RGBA, GL_FLOAT, 0); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);
    errorCheckFunc(__FILE__, __LINE__);

}

void RayCasting::createTexture1D(GLuint &texID) {

	glGenTextures(1,&texID);
	glBindTexture(GL_TEXTURE_1D,texID);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
/* Which parameters do I use?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
*/
	glBindTexture(GL_TEXTURE_1D,0);
    errorCheckFunc(__FILE__, __LINE__);

}

float RayCasting::computeDistanceFromPointToPlane(Eigen::Vector3f planePoint, Eigen::Vector3f planeNormal, Eigen::Vector3f point) {

	return fabs( (planeNormal[0] * point[0] + planeNormal[1] * point[1] + planeNormal[2] * point[2] - 
			(planeNormal[0] * planePoint[0] + planeNormal[1] * planePoint[1] + planeNormal[2] * planePoint[2])) );

}

Eigen::Vector4f RayCasting::computeInitialTranslation(Trackball* cameraTrackball){

	//The initial translation applied to view matrix.
	float initialTranslation = 0.0;//Change this to trackball->getDefaultTranslation().

	//Compute Initial Translation:

	//Get the viewport plane equation:
	Eigen::Vector4f cubeVertex;
	Eigen::Vector3f planeNormal, planePoint, transformedPlaneNormal, transformedPlanePoint;

	Eigen::Affine3f modelViewMatrix;
	modelViewMatrix = cameraTrackball->getViewMatrix();// * cameraTrackball->getModelMatrix();

	//Transforms the plane point and normal to eye space:
	//transformedPlanePoint = (modelViewMatrix * planePoint).head(3);
	planePoint = modelViewMatrix.translation();
	transformedPlanePoint = modelViewMatrix.rotation() * planePoint;
	planeNormal << 0.0, 0.0, 1.0;
	transformedPlaneNormal = modelViewMatrix.rotation() * planeNormal;
	transformedPlaneNormal.normalize();
	//cout<< "transformedPlanePoint = " << transformedPlanePoint.transpose() << endl;
	//cout<< "transformedPlaneNormal = " << transformedPlaneNormal.transpose() << endl;

	float distance, minDistance, maxDistance;
    minDistance = 1000.0;
	maxDistance = 0.0;
	int maxVertexIndex, minVertexIndex;

	//Test all volumes' vertices to get the one nearest to the viewport plane and start ray casting at this vertex.
	for (int i=0; i<8; i++) {
		cubeVertex = volume->getMesh()->getCubeVertex(i);
		distance = computeDistanceFromPointToPlane(transformedPlanePoint, transformedPlaneNormal, cubeVertex.head(3));
		if ( distance < minDistance ) {
			minDistance = distance;
			minVertexIndex = i;
		}
		if ( distance > maxDistance ) {
			maxDistance = distance;
			maxVertexIndex = i;
		}
	}

	//cout << "mindis = " << minDistance << " , maxdist = " << maxDistance << endl;
	//cout << "Minimum vertex: " << volume->getMesh()->getCubeVertex(minVertexIndex).transpose() << endl;

	//Translation that will be applied to modelView matrix in order to start ray casting at the volume's boundary.
	Eigen::Vector3f translationVector = transformedPlaneNormal * minDistance;
//	translationVector << minDistance * transformedPlaneNormal[0], minDistance * transformedPlaneNormal[1],
//						 minDistance * transformedPlaneNormal[2] - initialTranslation;

	auxViewMatrix = cameraTrackball->getViewMatrix();

	//cout<< "first translation: "<<translationVector.transpose() <<endl;

	//Translate View Matrix:
	auxViewMatrix.translate(translationVector);

	//Computing number of Iterations:

	//Get the final vertex covered by the ray casting:
//	cubeVertex = volume->getMesh()->getCubeVertex(minVertexIndex);
//	cubeVertex[3] = 0.0;//Transforms point to vector.
//	cubeVertex *= 1; //2 or -2? Depends on the scalar signal to define the step direction.

	//Compute Initial Translation in z axis = Distance between origin and tangent vertex to the view plane.:
	//initialTranslation = transformedPlaneNormal.dot(cubeVertex.head(3));
	initialTranslation = maxDistance - minDistance;
	cout<<"initial distance: "<<initialTranslation<<endl;

	//Projects the cubeVertex vector in the plane normal to get the total length covered by the ray casting. Then divide it by the depthStep to get the number of iterations needed:
	//cout << "initial = " << initialTranslation << " depthStep = " << depthStep << endl;
	numberOfIterations = abs(initialTranslation/depthStep);
	//cout << "number of iterations: " << numberOfIterations << endl;
	viewPlaneNormal=transformedPlaneNormal;

	return volume->getMesh()->getCubeVertex(minVertexIndex);

}


//We will use ping pong structure to use the shaders alternating the reading and writing textures. Each step will be duplicated.
//First we define the start point and inside the fragment shader we will increment each step.
void RayCasting::render(Mesh* mesh, Trackball* cameraTrackball, Trackball* lightTrackball) {

	//cout << "Accumulated: " << accumulatedColorsTexture << ", Current: " << currentColorsTexture << endl << endl;

	//Do I need to clear the buffers before begin?
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//Move screen plane to nearest cube vertex:
	Eigen::Vector4f firstVertex = computeInitialTranslation(cameraTrackball);

	//Getting the volume boundaries (Max value = 0.5):
	Eigen::Vector3f volumeBoundaries;
	volumeBoundaries << fabs(firstVertex[0]), fabs(firstVertex[1]), fabs(firstVertex[2]);
	//cout << volumeBoundaries.transpose() << endl;

	//DEBUG: Print the first vertex processed by the ray casting:
	//cout << "FIRST VERTEX = " << firstVertex.transpose() << endl;

	//Creating the canonic vectors:
	Eigen::Vector4f e_x, e_y, e_z;
	e_x << auxViewMatrix.rotation() * Eigen::Vector3f(1.0,0.0,0.0) * viewportWorldSpaceSize[0]/(float)viewportSize[0],0.0;
	e_y << auxViewMatrix.rotation() * Eigen::Vector3f(0.0,1.0,0.0) * viewportWorldSpaceSize[1]/(float)viewportSize[1],0.0;
	e_z << auxViewMatrix.rotation() * Eigen::Vector3f(0.0,0.0,1.0) * depthStep,0.0; 

	//DEBUG: Print the canonic vectors*viewportSize.
	//cout<< "ex: "<<(e_x*viewportSize[0]).transpose()<<endl<< "ey: "<<(e_y*viewportSize[1]).transpose()<<endl<< "ez: "<<e_z.transpose()<<endl;

	computeShader->enable();

	Eigen::Vector4f currentVertex;
	cout<<"fisrt vertex = "<<firstVertex.transpose()<<endl;
	currentVertex << firstVertex.head(3).dot(viewPlaneNormal)*viewPlaneNormal,1.0;
	cout<<"initial distance test: "<<firstVertex.head(3).dot(viewPlaneNormal)<<endl;
	//Initialize current vertex from the left-down vertex of the viewport, instead of its center.
    currentVertex -= (e_x*viewportSize[0]+ e_y*viewportSize[1])/2.0;

	//Clear Textures:
	clearTexture2D(currentColorsTexture);
    errorCheckFunc(__FILE__, __LINE__);

	//Enabling Textures:
	glEnable(GL_TEXTURE_1D);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_3D);

	//cout << "PRIMEIRO:" << endl;
	//cout<< "currentVertex = "<< currentVertex.transpose() << endl;
	//cout<< "oposite of currentVertex = "<< (currentVertex+e_x*viewportSize[0]+e_y*viewportSize[1]).transpose() <<" m = "<<(currentVertex+e_x*viewportSize[0]+e_y*viewportSize[1]).head(3).norm() <<endl;
	paintTexture2D(currentColorsTexture,Eigen::Vector4f(0.0,0.0,0.0,1.0));

	//Bind Image Texture:
	glBindImageTexture(0, currentColorsTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F); // bound to image unit 0
	
	glActiveTexture(GL_TEXTURE2); // on texture unit 2
	glBindTexture(GL_TEXTURE_3D, volume->getTexture3D());
	
	glActiveTexture(GL_TEXTURE3); // on texture unit 3
	glBindTexture(GL_TEXTURE_1D, transferFunction);
    
    errorCheckFunc(__FILE__, __LINE__);

	int iterationsInsideShader = numberOfIterations;
	//Setting Uniforms:
	computeShader->setUniform("volumeBoundaries", &volumeBoundaries[0], 3, 1);
	computeShader->setUniform("volumeTexture", 2);
	computeShader->setUniform("transferFunction", 3);
	computeShader->setUniform("e_x", &e_x[0], 4, 1);
	computeShader->setUniform("e_y", &e_y[0], 4, 1);
	computeShader->setUniform("e_z", &e_z[0], 4, 1);
	computeShader->setUniform("accumulatedColorTexture", 0);
	computeShader->setUniform("numberOfIterations",iterationsInsideShader);
    errorCheckFunc(__FILE__, __LINE__);

	//First Pass: Compute Shader:

	//int i = 1;
	//while (i <= numberOfIterations){
	
		computeShader->setUniform("currentVertex", &currentVertex[0], 4, 1);

		//Dispatch Compute:
		glDispatchCompute(viewportSize[0],viewportSize[1],1);
	    errorCheckFunc(__FILE__, __LINE__);
	
		//currentVertex += 100*e_z;

		//i+=100;

	//}

	//Unbind Textures:
	glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);//binds empty texture
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, 0);//binds empty texture
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_1D, 0);//binds empty texture
    
    errorCheckFunc(__FILE__, __LINE__);

	computeShader->disable();

	//Second Pass: Draw Quad:

	//Call shader program:
	shader->enable();

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);//binds to texture unit 1
	glBindTexture(GL_TEXTURE_2D,currentColorsTexture);
	
	shader->setUniform("finalTexture",1);
	shader->setUniform("viewportSize", &viewportSize[0], 2, 1);
    
    errorCheckFunc(__FILE__, __LINE__);

	quad->render();

	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_3D);

	shader->disable();

}
