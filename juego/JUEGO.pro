QT       += core gui serialport\
            multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    crear.cpp \
    graficar.cpp \
    main.cpp \
    mainwindow.cpp \
    objetocircular.cpp \
    objetocuadrado.cpp

HEADERS += \
    crear.h \
    graficar.h \
    mainwindow.h \
    objetocircular.h \
    objetocuadrado.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    imagenes.qrc

DISTFILES += \
    juego/1.gif \
    juego/Giff.gif \
    juego/ata.wav \
    juego/boton.wav \
    juego/c.PNG \
    juego/contrapared.wav \
    juego/disparador.wav \
    juego/logoard.png \
    juego/mario.wav \
    juego/mundo1.jpg \
    juego/mundo2.jpg \
    juego/mundo22.wav \
    juego/mundo2sustituto.jpg \
    juego/mundo3.jpg \
    juego/oe.gif \
    juego/plays1.wav \
    juego/punto.wav \
    juego/qtaudio_windows.dll \
    juego/tetriz.wav
