# Project settings
TARGET       = MelodiUS
CONFIG       += warn_on qt debug_and_release console
INCLUDEPATH  += ./include ./source ./

# Headers
HEADERS      += ./include/globaldef.h
HEADERS      += ./include/fpga.h
HEADERS      += ./include/recorder.h
HEADERS      += ./include/recording.h
HEADERS      += ./include/playback.h
HEADERS      += ./include/readwrite_wav.h
HEADERS      += ./include/generator.h
HEADERS      += ./include/detection_rythme.h
HEADERS      += ./include/fft.h


# Sources
SOURCES      += ./source/main.cpp
SOURCES      += ./source/fpga.cpp
SOURCES      += ./source/recorder.cpp
SOURCES      += ./source/playback.cpp
SOURCES      += ./source/readwrite_wav.cpp
SOURCES      += ./source/generator.cpp
SOURCES      += ./source/detection_rythme.cpp
SOURCES      += ./source/fft.cpp

win32 {
    # QT Project Settings
    TEMPLATE     = vcapp
    CONFIG       += c++17
    CONFIG       += /external:W0
    QMAKE_CXXFLAGS_WARN_ON ~= s/-W3/-W4

    # Libraries
    INCLUDEPATH  += ./portaudio ./CommunicationFPGA ./fasttrigo
    HEADERS      += ./portaudio/portaudio.h ./CommunicationFPGA/CommunicationFPGA.h ./fasttrigo/fasttrigo.h
    SOURCES      += ./fasttrigo/fasttrigo.cpp
    QMAKE_LFLAGS += ./portaudio_x64.lib
    LIB          += ./portaudio_x64.lib

    CONFIG(debug, debug|release) {
        QMAKE_LFLAGS += ./CommunicationFPGA-debug.lib
        LIB          += ./CommunicationFPGA-debug.lib
    }
    else {
        QMAKE_LFLAGS += ./CommunicationFPGA-release.lib
        LIB          += ./CommunicationFPGA-release.lib
    }
}

linux-g++* {
	DEFINES += LINUX_
	CONFIG += c++17
}
