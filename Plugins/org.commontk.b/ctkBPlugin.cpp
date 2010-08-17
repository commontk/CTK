/*
 * ctkBPlugin.cxx
 *
 *  Created on: Mar 29, 2010
 *      Author: zelzer
 */

#include "ctkBPlugin.h"

#include <QtPlugin>

#include <iostream>

#include <ctkClassA.h>
#include <ctkPlugin.h>

void ctkBPlugin::start(ctkPluginContext* context)
{
  Q_UNUSED(context)

  ctkClassA a;

  std::cout << "B Plugin started\n";
}

void ctkBPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  std::cout << "B Plugin stopped\n";
}

Q_EXPORT_PLUGIN2(org_commontk_b, ctkBPlugin)
