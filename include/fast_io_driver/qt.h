#pragma once

#if __has_include(<QIODevice>)
#include<QIODevice>
#else
#include<QtCore/QIODevice>
#endif
#if __has_include(<QFile>)
#include<QFile>
#else
#include<QtCore/QFile>
#endif
#if __has_include(<QFileDevice>)
#include<QFileDevice>
#else
#include<QtCore/QFileDevice>
#endif
#if __has_include(<QString>)
#include<QString>
#else
#include<QtCore/QString>
#endif

#include"qt_impl/qstring.h"
#include"qt_impl/qiodevice.h"
#include"qt_impl/qfile.h"
#if defined(QDEBUG_H)
#include"qt_impl/qdebug.h"
#endif
