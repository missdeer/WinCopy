QT -= gui

CONFIG += c++17 console cmdline precompile_header
CONFIG -= app_bundle

PRECOMPILED_HEADER = StdAfx.h
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
DEFINES += USING_PCH
}

SOURCES += \
        IoPool.cpp \
        OvlFile.cpp \
        ProcessFiles.cpp \
        StdAfx.cpp \
        main.cpp \
        winUtil.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    CopyFile.h \
    DigestFile.h \
    Exception.h \
    FileProcessor.h \
    IoPool.h \
    OvlFile.h \
    ProcessFiles.h \
    StdAfx.h \
    Util.h \
    VerifyFile.h \
    winUtil.h

LIBS += -lShlwapi
