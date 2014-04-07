TEMPLATE = app
CONFIG += console
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp \
    model.cpp \
    graph.cpp \
    searcher.cpp \
    evaluator.cpp

HEADERS += \
    model.h \
    graph.h \
    searcher.h \
    evaluator.h

