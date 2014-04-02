#ifndef VOLWIDGET_H
#define VOLWIDGET_H

//#include "effect.hpp"

#include "effect.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "trackball.hpp"
#include "volume.h"


#include <QGLWidget>
#include <QMouseEvent>
#include <QDebug>
#include <QTime>


class volWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit volWidget(QWidget *parent);

    virtual void initializeGL();

    /**
     * @brief Initializes the widget with a given image
     * @param Given image
     */
    void initialize();


    /**
     * Repaints screen buffer.
     **/
    virtual void paintGL();

    void setLayer(float layer);

    /**
     * @brief Initializes the transfer function
     */
    void initializeTransferFunction();

    /**
     * @brief Is called when the mouse is pressed and moved over the Viewport
     */
    void mouseMoveEvent(QMouseEvent *event);

    /**
     * @brief Is called when the mouse is pressed
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * @brief Is called when the mouse is released
     */
    void mouseReleaseEvent(QMouseEvent *event);

    /**
     * @brief Recalculate the unit vectors
     */
    void updateUnitVectors();

    /**
     * @brief Resets the transfer function
     */
    void resetTransferFunction(int a, int b, int c, int d);

    void calculateGradient();

signals:

public slots:

private:
    //The volume we're working with
    Volume* volume;
    GLint texIndex;

    //Transfer Function
    Texture* transferFunction;

    //Index of transfer Function
    GLuint TFuid;
    GLint TFid;

    //The unit vectors
    Eigen::Vector4f uX, uY, uZ;

    //The volume container diagonal size and dimensions
    float volDiagonal;
    Eigen::Vector3f volDimensions;
    Eigen::Vector3f volResolution;
    int rootOfDepth;
    GLubyte* gradArray;

    //Number of steps and step size for rendering
    int numberOfSteps;
    float stepSize;

    ///Shader
    Shader* shader;
    Shader* gradShader;

    ///Testing Mesh
    Mesh* mesh;

    ///Trackball Object for the camera position.
    Trackball* cameraTrackball;

    ///Trackball object for lights position.
    Trackball* lightTrackball;

    ///Current Viewport Width measured in pixels.
    int currentWidth;

    ///Current Viewport Height measured in pixels.
    int currentHeight;

    ///Current Viewport Position in X axis in pixels.
    int currentPosX;

    ///Current Viewport Position in Y axis in pixels.
    int currentPosY;

    ///Render plane center. Has to be updated everytime the camera moves
    Eigen::Vector4f rendPlaneCenter;

    //FPS counting:
    QTime *time;
    int fps;

    void updateRendPlane();

    void draw(void);

    //int ins;

    float curLayer;

protected:
    virtual void keyPressEvent(QKeyEvent *keyevent);
};

#endif // VOLWIDGET_H
