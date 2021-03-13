TARGET       = MelodiUS
CONFIG       += warn_on qt debug_and_release console
INCLUDEPATH  += ./include ./source ./

HEADERS      += ./include/globaldef.h
HEADERS      += ./include/fpga.h
HEADERS      += ./include/recorder.h ./include/recording.h
HEADERS      += ./include/playback.h
HEADERS      += ./include/readwrite_wav.h
HEADERS      += ./include/generator.h
HEADERS      += ./include/detection_rythme.h

SOURCES      += ./source/main.cpp
SOURCES      += ./source/recorder.cpp ./source/recording.cpp
SOURCES      += ./source/fpga.cpp
SOURCES      += ./source/playback.cpp
SOURCES      += ./source/readwrite_wav.cpp
SOURCES      += ./source/generator.cpp
SOURCES      += ./source/detection_rythme.cpp

win32 {
    TEMPLATE     = vcapp
    QMAKE_CXXFLAGS_WARN_ON ~= s/-W3/-W4
    LIB          += ./portaudio_x64.lib ./CommunicationFPGA.lib
    QMAKE_LFLAGS += ./portaudio_x64.lib ./CommunicationFPGA.lib
    CONFIG       += windows c++17 console

    INCLUDEPATH  += ./portaudio ./CommunicationFPGA
    HEADERS      += ./portaudio/portaudio.h ./CommunicationFPGA/CommunicationFPGA.h
}

linux-g++* {
	DEFINES += LINUX_
	CONFIG += c++17
}
