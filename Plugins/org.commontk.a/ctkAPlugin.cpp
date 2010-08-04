/*
 * ctkAPlugin.cxx
 *
 *  Created on: Mar 29, 2010
 *      Author: zelzer
 */

#include "ctkAPlugin.h"

#include <QtPlugin>

#include <iostream>

void ctkAPlugin::start(ctkPluginContext* context)
{
  Q_UNUSED(context)
  std::cout << "A Plugin started\n";
}

void ctkAPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  std::cout << "A Plugin stopped\n";
}

Q_EXPORT_PLUGIN2(org_commontk_a, ctkAPlugin)
