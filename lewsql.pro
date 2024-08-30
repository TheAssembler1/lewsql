# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = lewsql

QT = core gui widgets

HEADERS = \
   $$PWD/buffer_manager/src/buffer_manager.h \
   $$PWD/buffer_manager/src/buffer_manager_error.h \
   $$PWD/buffer_manager/src/buffer_page.h \
   $$PWD/buffer_manager/src/replacement/replacement_alg.h \
   $$PWD/buffer_manager/src/replacement/lru.h \
   $$PWD/disk_manager/src/impl/posix_disk_manager.h \
   $$PWD/disk_manager/src/disk_manager.h \
   $$PWD/disk_manager/src/disk_manager_error.h \
   buffer_manager/src/buffer_manager_types.h \
   disk_manager/src/disk_manager_types.h

SOURCES = \
   $$PWD/buffer_manager/src/buffer_manager.cpp \
   $$PWD/buffer_manager/src/replacement/lru.cpp \
   $$PWD/disk_manager/src/impl/posix_disk_manager.cpp \
   $$PWD/driver/src/run.cpp

INCLUDEPATH = \
    $$PWD/buffer_manager/src \
    $$PWD/disk_manager/src \
    $$PWD/disk_manager/src/impl

#DEFINES = 

DISTFILES += \
    buffer_manager/src/replacment

