######################################################################
# InventoryManager.pro - Archivo de proyecto Qt para Gestor de Inventario
# Autor: [Tu Nombre]
# Curso: Programación Orientada a Objetos
# Fecha: 2024
# Sistema: Linux con Qt5 y SQLite3
######################################################################

# Módulos de Qt requeridos
QT += core gui widgets sql

# Para compatibilidad con Qt5
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Configuración del compilador
CONFIG += c++17 warn_on

# Nombre del ejecutable
TARGET = InventoryManager

# Tipo de proyecto (aplicación)
TEMPLATE = app

# Directorios para archivos generados
DESTDIR = $$PWD/build
OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.rcc
UI_DIR = $$DESTDIR/.ui

######################################################################
# ARCHIVOS FUENTE (.cpp)
######################################################################

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/component.cpp \
    src/databasemanager.cpp \
    src/inventory_manager.cpp

######################################################################
# ARCHIVOS DE CABECERA (.h)
######################################################################

HEADERS += \
    src/mainwindow.h \
    src/component.h \
    src/databasemanager.h \
    src/inventory_manager.h

######################################################################
# ARCHIVOS DE INTERFAZ (.ui)
######################################################################

FORMS += \
    ui/mainwindow.ui

######################################################################
# CONFIGURACIÓN ESPECÍFICA POR PLATAFORMA
######################################################################

# Configuración para Linux (incluyendo Raspberry Pi)
linux-g++ {
    # Optimización para tamaño/rendimiento
    QMAKE_CXXFLAGS += -O2 -pipe -Wall -Wextra
    
    # Definir para dispositivos de bajo consumo
    DEFINES += LOW_POWER_DEVICE QT_DEPRECATED_WARNINGS
    
    # Enlazar con SQLite3
    LIBS += -lsqlite3
    
    # Configuración para depuración si se necesita
    CONFIG(debug, debug|release) {
        QMAKE_CXXFLAGS += -g
        DEFINES += DEBUG_MODE
    }
}

# Configuración para Windows (si lo compilas en Windows)
win32 {
    LIBS += -lsqlite3
    DEFINES += _WIN32_WINNT=0x0601
}

# Configuración para macOS
macx {
    LIBS += -lsqlite3
    QMAKE_MAC_SDK = macosx
}

######################################################################
# CONFIGURACIÓN DE DEPURACIÓN Y RELEASE
######################################################################

# Configuración para modo RELEASE
CONFIG(release, debug|release) {
    # Deshabilitar salida de depuración
    DEFINES += QT_NO_DEBUG_OUTPUT
    # Optimización máxima
    QMAKE_CXXFLAGS_RELEASE -= -O
    QMAKE_CXXFLAGS_RELEASE += -O3
}

# Configuración para modo DEBUG
CONFIG(debug, debug|release) {
    # Habilitar símbolos de depuración
    QMAKE_CXXFLAGS += -g
    # Advertencias adicionales
    QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
    # Definir para modo debug
    DEFINES += DEBUG_MODE
}

######################################################################
# CONFIGURACIÓN DE INSTALACIÓN (OPCIONAL)
######################################################################

# Directorios de instalación
unix {
    target.path = /usr/local/bin
    INSTALLS += target
}

######################################################################
# CONFIGURACIÓN DE RECURSOS (SI NECESITAS IMÁGENES/ICONOS)
######################################################################

# Descomentar si añades un archivo .qrc con recursos
# RESOURCES += \
#     resources.qrc

######################################################################
# VARIABLES PERSONALIZADAS
######################################################################

# Versión del proyecto
VERSION = 1.0.0

# Información para el ejecutable
QMAKE_TARGET_COMPANY = "IoT Laboratory"
QMAKE_TARGET_DESCRIPTION = "Sistema de Gestión de Inventario"
QMAKE_TARGET_COPYRIGHT = "Copyright © 2024 IoT Lab. Todos los derechos reservados."
QMAKE_TARGET_PRODUCT = "Gestor de Inventario IoT"

######################################################################
# CONFIGURACIÓN DE INCLUDES (SI NECESITAS DIRECTORIOS ADICIONALES)
######################################################################

# Incluir directorio src para headers
INCLUDEPATH += $$PWD/src

# Mensaje de compilación exitosa
message("Proyecto configurado correctamente")
message("Target: $$TARGET")
message("Qt Modules: $$QT")
message("Compilador: $$QMAKE_CXX")
