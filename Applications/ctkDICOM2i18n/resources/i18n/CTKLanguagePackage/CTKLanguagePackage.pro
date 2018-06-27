QT += core
QT -= gui

CONFIG += c++11

TARGET = CTKLanguagePackage
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

TRANSLATIONS = ctk_zh_cn.ts

RESOURCES += \
    ctkdicom.qrc

DISTFILES += \
    ctk_zh_cn.ts

