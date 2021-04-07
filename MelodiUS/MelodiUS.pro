# Project settings
TARGET      = MelodiUS
CONFIG      += warn_on qt debug_and_release console c++17
CONFIG      -= flat
INCLUDEPATH += ./include ./source ./

# Headers
HEADERS += ./include/globaldef.h
HEADERS += ./include/fpga.h
HEADERS += ./include/recorder.h
HEADERS += ./include/recording.h
HEADERS += ./include/playback.h
HEADERS += ./include/readwrite_wav.h
HEADERS += ./include/generator.h
HEADERS += ./include/detection_rythme.h
HEADERS += ./include/fft.h
HEADERS += ./include/tuning.h
HEADERS += ./include/note.h

SOURCES += ./source/main.cpp
SOURCES += ./source/fpga.cpp
SOURCES += ./source/recorder.cpp
SOURCES += ./source/playback.cpp
SOURCES += ./source/readwrite_wav.cpp
SOURCES += ./source/generator.cpp
SOURCES += ./source/detection_rythme.cpp
SOURCES += ./source/fft.cpp

win32 {
    # QT Project Settings
    TEMPLATE  = vcapp
    CONFIG    += /external:W0
    QT        += core gui widgets
    QMAKE_CXXFLAGS_WARN_ON ~= s/-W3/-W4

    # Libraries
    HEADERS      += ./portaudio/portaudio.h ./CommunicationFPGA/CommunicationFPGA.h ./fasttrigo/fasttrigo.h
    SOURCES      += ./fasttrigo/fasttrigo.cpp
    QMAKE_LFLAGS += ./portaudio_x64.lib
    LIB          += ./portaudio_x64.lib

    CONFIG(debug, debug|release) {
        QMAKE_LFLAGS += ./CommunicationFPGA/CommunicationFPGA-debug.lib
        LIB          += ./CommunicationFPGA/CommunicationFPGA-debug.lib
    }
    else {
        QMAKE_LFLAGS += ./CommunicationFPGA/CommunicationFPGA-release.lib
        LIB          += ./CommunicationFPGA/CommunicationFPGA-release.lib
    }

    HEADERS += ./include/widgets/widget_note.h
    HEADERS += ./include/mainwindow.h ./include/mainwindow_ui.h
    HEADERS += ./include/gui.h

    SOURCES += ./source/mainwindow.cpp
    SOURCES += ./source/gui.cpp
}

linux-g++* {
	DEFINES += LINUX_
}
