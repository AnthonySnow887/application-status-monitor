INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..

QT   += core

HEADERS +=  $$PWD/CpuLoad.h \
	    $$PWD/MemoryLoad.h \
	    $$PWD/NetworkLoad.h \
	    $$PWD/DriveLoad.h \
	    $$PWD/ProcessLoad.h

SOURCES +=  $$PWD/CpuLoad.cpp \
	    $$PWD/MemoryLoad.cpp \
	    $$PWD/NetworkLoad.cpp \
	    $$PWD/DriveLoad.cpp \
	    $$PWD/ProcessLoad.cpp
 
