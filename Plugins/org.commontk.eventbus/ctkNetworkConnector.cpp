/*
 *  ctkNetworkConnector.cpp
 *  mafEventBus
 *
 *  Created by Daniele Giunchi on 11/04/10.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkNetworkConnector.h"

using namespace ctkEventBus;

ctkNetworkConnector::ctkNetworkConnector() {

}

QString ctkNetworkConnector::protocol() {
    return m_Protocol;
}
