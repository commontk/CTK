/*
 * ctkCPlugin.cxx
 *
 *  Created on: Mar 29, 2010
 *      Author: zelzer
 */

#include "ctkCPlugin.h"

#include <QtPlugin>

#include <iostream>

void ctkCPlugin::start(ctkPluginContext* context)
{
  Q_UNUSED(context)
  std::cout << "C Plugin started\n";
}

void ctkCPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  std::cout << "C Plugin stopped\n";
}

Q_EXPORT_PLUGIN2(org_commontk_c, ctkCPlugin)
