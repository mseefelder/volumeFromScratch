#ifndef VOLWIDGET_H
#define VOLWIDGET_H

//#include "effect.hpp"

#include "effect.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "trackball.hpp"
#include "volume.h"


#include <QGLWidget>


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

    /**
     * @brief Initializes the transfer function
     */
    void volWidget::initializeTransferFunction();

    /**
     * @brief Is called when the mouse is pressed and moved over the Viewport
     */
    void volWidget::mouseMoveEvent(QMouseEvent *);

signals:

public slots:

private:
    //The volume we're working with
    Volume* volume;

    //Transfer Function
    Texture* transferFunction;

    //Index of transfer Function
    GLuint TFuid;
    GLint TFid;

    //The unit vectors
    Eigen::Vector4f uX, uY, uZ;

    //The volume container diagonal size
    float volDiagonal;

    //The volume container dimensions
    Eigen::Vector3f volDimensions;

    ///Shader
    Shader* shader;

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

    void updateRendPlane();

    void draw(void);

    int ins;

     //SNN *snn;
};

#endif // VOLWIDGET_H
