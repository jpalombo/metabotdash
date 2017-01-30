#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T15:48:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = metabotdash
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp \
    camera.cpp \
    cameracontrol.cpp

HEADERS  += mainwindow.h \
    glwidget.h \
    camera.h \
    cameracontrol.h \
    mmalincludes.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

win32{
    message("Using win32 configuration")
    exists(C:/Users/jp/Downloads/opencv) {
        OPENCV_PATH = C:/Users/jp/Downloads/opencv
        message("Work configuration")
    } else {
        OPENCV_PATH = C:/Users/john/Downloads/opencv
        message("Home configuration")
    }

    LIBS_PATH = "$$OPENCV_PATH/build/x64/vc14/lib"

    CONFIG(debug, debug|release) {
    LIBS     += -L$$LIBS_PATH \
                -lopencv_world320d
    }

    CONFIG(release, debug|release) {
    LIBS     += -L$$LIBS_PATH \
                -lopencv_world320
    }

    INCLUDEPATH += \
        $$OPENCV_PATH/build/include/
}

unix{

message("Using unix configuration")

INCLUDEPATH += /usr/include

LIBS_PATH = =/opt/vc/lib

LIBS += \
    -L$$LIBS_PATH \
    -lopencv_core \
    -lopencv_imgproc \
    -lmmal_core \
    -lmmal_util \
    -lmmal_vc_client \
    -lvcos \
    -lbcm_host\
    -lwiringPi
}

message("OpenCV path: $$OPENCV_PATH")
message("Includes path: $$INCLUDEPATH")
message("Libraries: $$LIBS")

INSTALLS        = target
target.files    = metabotdash
target.path     = /home/pi

