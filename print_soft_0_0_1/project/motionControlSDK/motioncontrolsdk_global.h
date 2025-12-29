#pragma once

#include <QtCore/qglobal.h>
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QtGlobal>


#ifndef BUILD_STATIC
# if defined(MOTIONCONTROLSDK_LIB)
#  define MOTIONCONTROLSDK_EXPORT Q_DECL_EXPORT
# else
#  define MOTIONCONTROLSDK_EXPORT Q_DECL_IMPORT
# endif
#else
# define MOTIONCONTROLSDK_EXPORT
#endif



