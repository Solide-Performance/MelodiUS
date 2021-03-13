# Project settings
TARGET       = MelodiUS
CONFIG       += warn_on qt debug_and_release console
INCLUDEPATH  += ./include ./source ./

# Headers
HEADERS      += ./include/globaldef.h
HEADERS      += ./include/fpga.h
HEADERS      += ./include/recorder.h ./include/recording.h
HEADERS      += ./include/playback.h
HEADERS      += ./include/readwrite_wav.h
HEADERS      += ./include/generator.h
HEADERS      += ./include/detection_rythme.h
HEADERS      += ./include/fft.h

# Sources
SOURCES      += ./source/main.cpp
SOURCES      += ./source/fpga.cpp
SOURCES      += ./source/recorder.cpp ./source/recording.cpp
SOURCES      += ./source/playback.cpp
SOURCES      += ./source/readwrite_wav.cpp
SOURCES      += ./source/generator.cpp
SOURCES      += ./source/detection_rythme.cpp
SOURCES      += ./source/fft.cpp

win32 {
    # QT Project Settings
    TEMPLATE     = vcapp
    CONFIG       += windows c++17 console
    QMAKE_CXXFLAGS_WARN_ON ~= s/-W3/-W4

    # Libraries
    LIB          += ./portaudio_x64.lib ./CommunicationFPGA.lib
    QMAKE_LFLAGS += ./portaudio_x64.lib ./CommunicationFPGA.lib
    INCLUDEPATH  += ./portaudio ./CommunicationFPGA
    HEADERS      += ./portaudio/portaudio.h ./CommunicationFPGA/CommunicationFPGA.h
}

linux-g++* {
	DEFINES += LINUX_
	CONFIG += c++17
}
