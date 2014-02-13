#ifndef VOLWIDGET_H
#define VOLWIDGET_H

//#include "effect.hpp"
//#include "volume.hpp"

#include "effect.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "trackball.hpp"

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
     * @brief Reads the iteration result and writes to a QImage
     * @param potentialImage
     * @param spikeImage
     */
    void readOutput(QImage &potentialImage, QImage &spikeImage);

    /**
     * @brief Returns a pointer to the SNN Effect
     * @return ptr to effect
     */
    //SNN* getSNNEffect (void) {
    //    return snn;
    //}

signals:

public slots:

private:
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

    void draw(void);

     //SNN *snn;
};

#endif // VOLWIDGET_H
