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

    /**
     * @brief Initializes the OpenGL context
     */
    virtual void initializeGL();

    /**
     * @brief Initializes the widget with a given image
     * @param Given image
     */
    void initialize();


    /**
     * @brief Repaints screen buffer.
     **/
    virtual void paintGL();

    /**
     * @brief Initializes the transfer function
     */
    void initializeTransferFunction();

    /**
     * @brief Initializes the transfer function
     */
    void initializeJitteringTexture();

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

    void reloadShader();

signals:

public slots:

private:
    ///The volume we're working with and it's Texture ID
    Volume* volume;
    GLint texIndex;

    ///Transfer Function and Jittering Texture
    Texture* transferFunction;
    Texture* jitteringTexture;

    ///Transfer Function and Jittering Texture indices
    GLuint TFuid;
    GLint TFid;
    GLuint JTuid;
    GLint JTid;

    ///The unit vectors
    Eigen::Vector4f uX, uY, uZ;

    ///About the volume container:
    ///The diagonal's size
    float volDiagonal;
    ///The dimensions in world space
    Eigen::Vector3f volDimensions;
    ///The texture's resolution
    Eigen::Vector3f volResolution;

    ///Number of steps and step size for rendering
    int numberOfSteps;
    float stepSize;

    ///Shader
    Shader* shader;

    ///Rendering plane
    Mesh* mesh;

    ///Trackball Object for the camera position.
    Trackball* cameraTrackball;
    ///Trackball object for light position.
    Trackball* lightTrackball;
    ///Contains both trackballs
    Trackball** trackballs;
    ///Index of the currently active trackball
    int tBallIndex;

    ///Current Viewport Width measured in pixels.
    int currentWidth;

    ///Current Viewport Height measured in pixels.
    int currentHeight;

    ///Current Viewport Position in X axis in pixels.
    int currentPosX;

    ///Current Viewport Position in Y axis in pixels.
    int currentPosY;

    ///Rendering plane's center. Has to be updated everytime the camera moves
    Eigen::Vector4f rendPlaneCenter;

    ///FPS counting:
    QTime *time;
    int fps;

    /**
     * @brief Calculates the Rendering plane's center
     */
    void updateRendPlane();

    /**
     * @brief The rendering
     */
    void draw(void);


protected:
    virtual void keyPressEvent(QKeyEvent *keyevent);

    virtual void keyReleaseEvent(QKeyEvent *keyevent);
};

#endif // VOLWIDGET_H
