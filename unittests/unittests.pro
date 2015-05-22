include(../coverage.pri)

CONFIG += testcase
QMAKE_CXXFLAGS += -std=c++11
DEFINES += QT_NO_KEYWORDS
TARGET = tst_mediaplayerplugin

QT += multimedia opengl multimedia-private quick testlib

INCLUDEPATH += ../src/aal \
    /usr/include/qt5/QtMultimedia \
    /usr/include/hybris \
    /usr/include/libqtubuntu-media-signals \
    /usr/include/mediascanner-2.0
LIBS += \
    -lqtubuntu-media-signals \
    -lmediascanner-2.0

HEADERS += \
    ../src/aal/aalmediaplayercontrol.h \
    ../src/aal/aalmediaplayerservice.h \
    ../src/aal/aalmediaplayerserviceplugin.h \
    ../src/aal/aalvideorenderercontrol.h \
    ../src/aal/aalmetadatareadercontrol.h \
    ../src/aal/aalmediaplaylistcontrol.h \
    tst_mediaplayerplugin.h \
    tst_metadatareadercontrol.h \
    tst_mediaplaylistcontrol.h \
    player.h \
    service.h

SOURCES += \
    tst_mediaplayerplugin.cpp \
    tst_metadatareadercontrol.cpp \
    tst_mediaplaylistcontrol.cpp \
    player.cpp \
    service.cpp \
    ../src/aal/aalmediaplayercontrol.cpp \
    ../src/aal/aalmetadatareadercontrol.cpp \
    ../src/aal/aalmediaplaylistcontrol.cpp \
    ../src/aal/aalmediaplayerservice.cpp \
    ../src/aal/aalmediaplayerserviceplugin.cpp \
    ../src/aal/aalvideorenderercontrol.cpp

# Installs required test media into place where the mediascanner will scan them
system(cd $$PWD; echo $$PWD; ./setup_mediascanner.sh)
