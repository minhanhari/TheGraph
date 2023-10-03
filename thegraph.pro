QT += widgets\
    core\
    svg
requires(qtConfig(filedialog))

QMAKE_CXX = gcc
QMAKE_CXXFLAGS = -c -Wall

HEADERS       = mainwindow.h \
    centralitywidget.h \
    groupwidget.h \
    include/edge.h \
    include/graph.h \
    include/vector.h \
    include/vertex.h \
    pathwidget.h \
    rbpfilter.h \
    renderarea.h \
    settingdialog.h
SOURCES       = main.cpp \
                centralitywidget.cpp \
                groupwidget.cpp \
                include/edge.c \
                include/graph.c \
                include/vector.c \
                include/vertex.c \
                mainwindow.cpp \
                pathwidget.cpp \
                rbpfilter.cpp \
                renderarea.cpp \
                settingdialog.cpp
#! [0]
RESOURCES     = thegraph.qrc
#! [0]
RC_ICONS = images/network.ico
# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/mainwindows/thegraph
INSTALLS += target

DISTFILES += \
    .gitignore
