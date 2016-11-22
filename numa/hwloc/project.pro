#-------------------------------------------------
#
# Project created by QtCreator 2013-06-08T22:34:13
#
#-------------------------------------------------

TARGET = numa_test 

SOURCES += main.cpp \
           numa_test.cpp \
           numa_topo.cpp \
           hwloc_bitmap_wrapper.cpp \
           numa_tutorial.cpp
HEADERS += numa_test.hpp \
           numa_tutorial.hpp \
           hwloc_bitmap_wrapper.hpp \
           numa_topo.hpp \
           generic_allocator.hpp

#QMAKE_CXX = clang

CONFIG -= qt
CONFIG += debug
QMAKE_CXXFLAGS += -std=c++11

QMAKE_CXXFLAGS_DEBUG -= -g 
QMAKE_CXXFLAGS_DEBUG += -ggdb 
QMAKE_CXXFLAGS_DEBUG += -Wpedantic 
#QMAKE_CXXFLAGS_DEBUG += -Weverything #clang only


#LIBS += -lnuma
LIBS += -lhwloc





#LIBS += -L/home/woelke/spideroak/working_dir/caf/actor-framework/build/lib -lcaf_core
#LIBS += -L/usr/lib -lpthread 

#ON PROBLEMS with "error while loading shared libraries" do:
#run ldconfig or ...
#LD_LIBRARY_PATH=/usr/local/lib ./project_name 
#oder /etc/ld.so.conf.d 
#sudo ldconfig


#export QT_SELECT=qt5
