
TEMPLATE     = vcapp
TARGET       = MelodiUS
CONFIG       += warn_on qt debug_and_release windows console c++latest
INCLUDEPATH  += ./include ./portaudio ./source ./

LIB          += ./portaudio_x64.lib
QMAKE_LFLAGS += ./portaudio_x64.lib

SOURCES      += ./source/main.cpp
HEADERS      += ./include/recorder.h ./include/recording.h
SOURCES      += ./source/recorder.cpp ./source/recording.cpp
HEADERS      += ./include/playback.h
SOURCES      += ./source/playback.cpp
HEADERS      += ./include/readwrite_wav.h
SOURCES      += ./source/readwrite_wav.cpp
