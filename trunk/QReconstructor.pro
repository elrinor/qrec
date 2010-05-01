TEMPLATE  = app
CONFIG   += qt warn_on
QT       += opengl

SOURCES = \
  src/main.cpp \
  src/dxflib/dl_dxf.cpp \
  src/dxflib/dl_writer_ascii.cpp \
  src/qr/MainWindow.cpp \
  src/qr/DxfReader.cpp \
  src/qr/Preprocessor.cpp \
  src/qr/RelationConstructor.cpp \
  src/qr/RelationFilter.cpp \
  src/qr/EdgeClassifier.cpp \
  src/qr/LoopConstructor.cpp \
  src/qr/LoopMerger.cpp \
  src/qr/LoopExtruder.cpp \
  src/qr/LoopFormationExtruder.cpp \
  src/qr/LoopFormationConstructor.cpp \
  src/qr/ObjectConstructor.cpp \
  src/qr/VertexClassifier.cpp \
  src/qr/ViewConstructor.cpp \
  src/qr/ViewGlView.cpp \
  src/qr/ViewBoxGlItem.cpp \
  src/qr/PolyhedronGlItem.cpp \
  src/qr/PlaneFolder.cpp \
  src/qr/Debug.cpp \

HEADERS = \
  src/qr/MainWindow.h \
  src/qr/ViewGlView.h \

FORMS = \

RESOURCES = \

INCLUDEPATH += src

UI_DIR    = src/ui
MOC_DIR   = bin/temp/moc
RCC_DIR   = bin/temp/rcc
TARGET    = QReconstructor

CONFIG(debug, debug|relase) {
  CONFIG           += console
  win32 {
    DESTDIR         = bin/debug
    OBJECTS_DIR     = bin/debug
  }
}

CONFIG(release, debug|release) {
  CONFIG           -= console
  win32 {
    DESTDIR         = bin/release
    OBJECTS_DIR     = bin/release
  }
}

win32 {
  DEFINES += WNT _CRT_SECURE_NO_WARNINGS
}