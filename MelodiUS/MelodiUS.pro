# Project settings
TARGET      = MelodiUS
CONFIG      += warn_on qt debug_and_release console c++latest # c++20
CONFIG      -= flat
INCLUDEPATH += ./include ./source ./


# Headers
HEADERS += ./include/fpga_phoneme.h
HEADERS += ./include/recorder.h
HEADERS += ./include/recording.h
HEADERS += ./include/playback.h
HEADERS += ./include/readwrite_wav.h
HEADERS += ./include/generator.h
HEADERS += ./include/detection_rythme.h
HEADERS += ./include/fft.h
HEADERS += ./include/tuning.h
HEADERS += ./include/note.h
HEADERS += ./include/Partition.h
HEADERS += ./include/Portee.h


# Sources
SOURCES += ./source/main.cpp
SOURCES += ./source/fpga_phoneme.cpp
SOURCES += ./source/recorder.cpp
SOURCES += ./source/playback.cpp
SOURCES += ./source/readwrite_wav.cpp
SOURCES += ./source/generator.cpp
SOURCES += ./source/detection_rythme.cpp
SOURCES += ./source/fft.cpp
SOURCES += ./source/Portee.cpp
SOURCES += ./source/Partition.cpp



# Platform-specific project settings
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

    SOURCES += ./source/mainwindow.cpp

    # Precompiled Header
    DEFINES += USING_PC   
    PRECOMPILED_HEADER = ./include/globaldef.h
}

linux-g++* {
    DEFINES += LINUX_
}
