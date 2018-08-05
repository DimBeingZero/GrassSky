TARGET = GrassSky
TEMPLATE = app

HEADERS += \
    enterthreedwidget.h \
    loadobj.h \
    meshmodel.h

SOURCES += \
    enterthreedwidget.cpp \
    main.cpp \
    loadobj.cpp \
    meshmodel.cpp

QT += core gui opengl

LIBS += -lopengl32 -lglu32

greaterThan(QT_MAJOR_VERSION,4): QT += widgets




