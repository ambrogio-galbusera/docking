QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = docking
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += SHOW_STEPS
DEFINES += RPI

SOURCES += \
        detectchars.cpp \
        detectplates.cpp \
        main.cpp \
        mainwindow.cpp \
        possiblechar.cpp \
        possibleplate.cpp \
        preprocess.cpp \
        servocontroller.cpp \
        sonar.cpp \
        videothread.cpp

HEADERS += \
        detectchars.h \
        detectplates.h \
        globals.h \
        mainwindow.h \
        possiblechar.h \
        possibleplate.h \
        preprocess.h \
        servocontroller.h \
        sonar.h \
        videothread.h

FORMS += \
        mainwindow.ui

DEPENDPATH += /usr/local/lib
INCLUDEPATH += /usr/local/include/opencv4 /usr/local/include/opencv4/opencv2 ../Navio2/C++/Navio
LIBS += -L/usr/local/lib \
    -lopencv_calib3d \
    -lopencv_core \
    -lopencv_dnn \
    -lopencv_features2d \
    -lopencv_flann \
    -lopencv_gapi \
    -lopencv_highgui \
    -lopencv_imgcodecs \
    -lopencv_imgproc \
    -lopencv_ml \
    -lopencv_objdetect \
    -lopencv_photo \
    -lopencv_stitching \
    -lopencv_video \
    -lopencv_videoio

LIBS += -L. -lnavio
