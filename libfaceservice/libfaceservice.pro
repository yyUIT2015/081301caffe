#-------------------------------------------------
#
# Project created by QtCreator 2018-07-22T17:44:41
#
#-------------------------------------------------

QT       -= gui

TARGET = libfaceservice
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        libfaceservice.cpp \
    define.cpp \
    facedatabase.cpp \
    facedetect.cpp \
    facerecognize.cpp \
    resource.cpp \
    watchstop.cpp

HEADERS += \
        libfaceservice.h \
    libfaceservice_global.h \
    define.h \
    facedatabase.h \
    facedetect.h \
    facerecognize.h \
    resource.h \
    watchstop.h \
    network.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}



LIBS += -/home/yyprocess/Desktop/newcaffe/caffe/build/lib/ -lcaffe

INCLUDEPATH += /home/yyprocess/Desktop/newcaffe/caffe/include
DEPENDPATH += /home/yyprocess/Desktop/newcaffe/caffe/build

DEFINES +=CPU_ONLY

LIBS += -L/usr/lib/x86_64-linux-gnu -lboost_thread -lboost_system -lboost_filesystem -lglog -lprotobuf

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib -lopencv_video -lopencv_ml -lopencv_legacy -lopencv_objdetect

LIBS += -L/usr/local/lib

INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/include


