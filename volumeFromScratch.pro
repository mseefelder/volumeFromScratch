#-------------------------------------------------
#
# Project created by QtCreator 2014-02-10T09:16:05
#
#-------------------------------------------------

#EIGEN_PATH  = ../../resources/eigen
EIGEN_PATH  = ../../resources/eigen3
SHADERLIB_PATH = ../../resources/shader-lib
#GLEW_PATH = ../../resources/glew
GLEW_PATH  = ../../../Documents/glew-1.10.0

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = volumeFromscratch
TEMPLATE = app

INCLUDEPATH += $$EIGEN_PATH $$SHADERLIB_PATH/include $$GLEW_PATH/include

LIBS  += -L$$GLEW_PATH/lib -L/usr/lib/nvidia-current/

# LIBRARIES
LIBS += -lGLEW -lGLU
QMAKE_CXXFLAGS += -fpermissive

SOURCES += main.cpp\
    mainwindow.cpp \
    $$SHADERLIB_PATH/src/shader.cpp \
    $$SHADERLIB_PATH/src/effect.cpp \
    $$SHADERLIB_PATH/src/framebuffer.cpp \
    $$SHADERLIB_PATH/src/texture.cpp \
    $$SHADERLIB_PATH/src/textureManager.cpp \
    $$SHADERLIB_PATH/src/mesh.cpp \
    ../../resources/shader-lib/src/trackball.cpp \
    volwidget.cpp \
    volume.cpp

HEADERS  += mainwindow.h \
    $$SHADERLIB_PATH/include/shader.hpp \
    $$SHADERLIB_PATH/include/effect.hpp \
    $$SHADERLIB_PATH/include/framebuffer.hpp \
    $$SHADERLIB_PATH/include/mesh.hpp \
    ../../resources/shader-lib/include/trackball.hpp \
    ../../resources/shader-lib/include/textureManager.hpp \
    ../../resources/shader-lib/include/texture.hpp \
    volwidget.h \
    volume.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    shaders/trackballShader.vert \
    shaders/trackballShader.frag \
    shaders/phongShader.vert \
    shaders/phongShader.frag
