#-------------------------------------------------
#
# Project created by QtCreator 2013-06-08T22:34:13
#
#-------------------------------------------------

TARGET = numa_tester

SOURCES += main.cpp \
           tester.cpp \
           hwloc_cpp_helper.cpp

HEADERS += tester.hpp  \
           hwloc_cpp_helper.hpp

#QMAKE_CXX = clang

CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11

QMAKE_CXXFLAGS_DEBUG -= -g 
QMAKE_CXXFLAGS_DEBUG += -ggdb 
QMAKE_CXXFLAGS_DEBUG += -Wpedantic 
#QMAKE_CXXFLAGS_DEBUG += -Weverything #clang only


LIBS += -L/usr/lib -lpthread  -lhwloc
