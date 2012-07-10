/*
 *  ctkEventBus_global.h
 *  ctkEventBus
 *
 *  Created by Paolo Quadrani on 27/03/09.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#ifndef CTKEVENTBUS_GLOBAL_H
#define CTKEVENTBUS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CTKEVENTBUS_LIBRARY)
#  define CTKEVENTBUSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CTKEVENTBUSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MAFEVENTBUS_GLOBAL_H
