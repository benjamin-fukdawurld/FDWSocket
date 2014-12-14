TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Socket/ClientSocket.cpp \
    Socket/portable_socket.cpp \
    Socket/ServeurSocket.cpp \
    Socket/Socket.cpp \
    conversion/byteConversion.cpp

HEADERS += \
    Socket/ClientSocket.h \
    Socket/portable_socket.h \
    Socket/ServeurSocket.h \
    Socket/Socket.h \
    conversion/byteConversion.h \
    Socket/FDWSocket.h

