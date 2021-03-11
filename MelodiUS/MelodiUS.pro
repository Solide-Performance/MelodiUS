
TEMPLATE     = vcapp
TARGET       = MelodiUS
CONFIG       += warn_on qt debug_and_release windows console c++latest
QT           += core gui widgets
INCLUDEPATH  += ./include ./portaudio ./source ./
INCLUDEPATH  += ./include ./source ./portaudio ./CommunicationFPGA ./

LIB          += ./portaudio_x64.lib ./CommunicationFPGA.lib
QMAKE_LFLAGS += ./portaudio_x64.lib ./CommunicationFPGA.lib
QMAKE_CXXFLAGS_WARN_ON ~= s/-W3/-W4

HEADERS      += ./include/globaldef.h
HEADERS      += ./portaudio/portaudio.h ./CommunicationFPGA/CommunicationFPGA.h
HEADERS      += ./include/fpga.h
HEADERS      += ./include/recorder.h ./include/recording.h
HEADERS      += ./include/playback.h
HEADERS      += ./include/readwrite_wav.h
HEADERS      += ./include/generator.h
HEADERS      += ./include/mainwindow.h ./include/mainwindow_ui.h


SOURCES      += ./source/main.cpp \
    source/gui.cpp
SOURCES      += ./source/recorder.cpp ./source/recording.cpp
SOURCES      += ./source/fpga.cpp
SOURCES      += ./source/playback.cpp
SOURCES      += ./source/readwrite_wav.cpp
SOURCES      += ./source/generator.cpp
SOURCES      += ./source/mainwindow.cpp
SOURCES      += ./source/gui.cpp
