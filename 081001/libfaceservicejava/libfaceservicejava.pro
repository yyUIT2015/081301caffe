#-------------------------------------------------
#
# Project created by QtCreator 2018-08-10T20:28:23
#
#-------------------------------------------------

QT       -= gui

TARGET = libfaceservicejava
TEMPLATE = lib

DEFINES += LIBFACESERVICEJAVA_LIBRARY

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
        libfaceservicejava.cpp \
    faceservice.cpp

HEADERS += \
        libfaceservicejava.h \
        libfaceservicejava_global.h \ 
    faceservice.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


INCLUDEPATH += /home/yyprocess/Desktop/libfaceservice
INCLUDEPATH += /opt/jvm/jdk1.8.0_181/include
INCLUDEPATH += /opt/jvm/jdk1.8.0_181/include/linux
INCLUDEPATH += /usr/local/include

#LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib -lopencv_video -lopencv_ml -lopencv_legacy -lopencv_objdetect

LIBS += /home/yyprocess/Desktop/build-libfaceservice-Desktop_Qt_5_11_0_GCC_64bit-Debug/liblibfaceservice.a \
/usr/lib/x86_64-linux-gnu/libboost_system.so \
/usr/lib/x86_64-linux-gnu/libboost_filesystem.so \
/usr/lib/x86_64-linux-gnu/libboost_thread.so \
/usr/lib/x86_64-linux-gnu/libglog.so \
/usr/lib/x86_64-linux-gnu/libprotobuf.so \
/usr/local/lib/libopencv_core.so \
/usr/local/lib/libopencv_imgproc.so \
/usr/local/lib/libopencv_highgui.so \
/usr/local/lib/libopencv_contrib.so \
/usr/local/lib/libopencv_video.so \
/usr/local/lib/libopencv_ml.so \
/usr/local/lib/libopencv_legacy.so \
 /usr/local/lib/libopencv_objdetect.so
#LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib -lopencv_video -lopencv_ml -lopencv_legacy -lopencv_objdetect
#LIBS += -L/usr/lib/x86_64-linux-gnu/ -lboost_system -lboost_filesystem -lboost_thread -lglog -lprotobuf
LIBS += -L/home/yyprocess/Desktop/newcaffe/caffe/build/lib/ -lcaffe

INCLUDEPATH += /home/yyprocess/Desktop/newcaffe/caffe/include
DEPENDPATH += /home/yyprocess/Desktop/newcaffe/caffe/build

PRE_TARGETDEPS += /home/yyprocess/Desktop/newcaffe/caffe/build/lib/libcaffe.a
DEFINES +=CPU_ONLY
