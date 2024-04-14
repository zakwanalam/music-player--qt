QT       += core gui
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    music.cpp \
    playlistprompt.cpp \
    queue.cpp

HEADERS += \
    mainwindow.h \
    music.h \
    playlistprompt.h \
    queue.h

FORMS += \
    mainwindow.ui \
    playlistprompt.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$VCPKG_ROOT/installed/x64-windows/include
LIBS += -L$$VCPKG_ROOT/installed/x64-windows/lib
LIBS += -lwinmm

unix|win32: LIBS += -L$$PWD/../../vcpkg/packages/mpg123_x64-windows/lib/ -lmpg123

INCLUDEPATH += $$PWD/../../vcpkg/packages/mpg123_x64-windows/include
DEPENDPATH += $$PWD/../../vcpkg/packages/mpg123_x64-windows/include

unix|win32: LIBS += -L$$PWD/../../vcpkg/packages/portaudio_x64-windows/lib/ -lportaudio

INCLUDEPATH += $$PWD/../../vcpkg/packages/portaudio_x64-windows/include
DEPENDPATH += $$PWD/../../vcpkg/packages/portaudio_x64-windows/include


unix|win32: LIBS += -L$$PWD/../../vcpkg/packages/ffmpeg_x64-windows/lib/ -lavcodec

INCLUDEPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include
DEPENDPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include

unix|win32: LIBS += -L$$PWD/../../vcpkg/packages/ffmpeg_x64-windows/lib/ -lavdevice

INCLUDEPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include
DEPENDPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include

unix|win32: LIBS += -L$$PWD/../../vcpkg/packages/ffmpeg_x64-windows/lib/ -lavfilter

INCLUDEPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include
DEPENDPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include

unix|win32: LIBS += -L$$PWD/../../vcpkg/packages/ffmpeg_x64-windows/lib/ -lavformat

INCLUDEPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include
DEPENDPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include

unix|win32: LIBS += -L$$PWD/../../vcpkg/packages/ffmpeg_x64-windows/lib/ -lavutil

INCLUDEPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include
DEPENDPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include

unix|win32: LIBS += -L$$PWD/../../vcpkg/packages/ffmpeg_x64-windows/lib/ -lswresample

INCLUDEPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include
DEPENDPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include

unix|win32: LIBS += -L$$PWD/../../vcpkg/packages/ffmpeg_x64-windows/lib/ -lswscale

INCLUDEPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include
DEPENDPATH += $$PWD/../../vcpkg/packages/ffmpeg_x64-windows/include

DISTFILES += \
    app.qml
