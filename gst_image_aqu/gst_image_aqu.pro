#-------------------------------------------------
#
# Project created by QtCreator 2018-03-15T15:38:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gst_image_aqu
TEMPLATE = app

#Step 1: add libs to the .pro file
unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gtk+-2.0 gstreamer-0.10 gstreamer-video-0.10 gstreamer-interfaces-0.10 gstreamer-app-0.10 opencv libv4l2
}

SOURCES += main.cpp\
        mainwindow.cpp \
    cam_thread.cpp

HEADERS  += mainwindow.h \
    cam_thread.h

FORMS    += mainwindow.ui
