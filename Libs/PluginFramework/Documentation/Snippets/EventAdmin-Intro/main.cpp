
#include <ctkPluginConstants.h>
#include <service/event/ctkEventConstants.h>

#include <QCoreApplication>
#include <QDir>

#include "ctkSnippetReportManager.h"

// dummy main
int main(int argc, char** argv)
{
  QCoreApplication myApp(argc, argv);

  QString tmpPath = QDir::tempPath() + "/snippet-eventadmin-intro";
  ctkProperties fwProps;
  fwProps[ctkPluginConstants::FRAMEWORK_STORAGE] = tmpPath;
  fwProps[ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN] = ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN_ONFIRSTINIT;
  fwProps["org.commontk.pluginfw.debug.resolve"] = true;
  fwProps["org.commontk.pluginfw.debug.service_reference"] = true;
  fwProps["org.commontk.pluginfw.debug.errors"] = true;
  fwProps["org.commontk.pluginfw.debug.pluginfw"] = true;
  fwProps["org.commontk.pluginfw.debug.lazy_activation"] = true;
  ctkPluginFrameworkLauncher::setFrameworkProperties(fwProps);
  ctkPluginFrameworkLauncher::start("org.commontk.eventadmin");

  ctkPluginContext* pluginContext = ctkPluginFrameworkLauncher::getPluginContext();

  ReportManager reportManager(pluginContext);

  //! [Event Handler service registration]
  ReportEventHandler eventHandler;
  ctkDictionary props;
  props[ctkEventConstants::EVENT_TOPIC] = "com/acme/reportgenerator/GENERATED";
  pluginContext->registerService<ctkEventHandler>(&eventHandler, props);
  //! [Event Handler service registration]

  // You can also use a wildcard in the final character of the EVENT_TOPIC
  //! [Event Handler service registration wildcard]
  props[ctkEventConstants::EVENT_TOPIC] = "com/acme/reportgenerator/*";
  pluginContext->registerService<ctkEventHandler>(&eventHandler, props);
  //! [Event Handler service registration wildcard]

  // Or you could use a filter expression (using LDAP syntax)
  //! [Event Handler service registration filter]
  props[ctkEventConstants::EVENT_TOPIC] = "com/acme/reportgenerator/GENERATED";
  props[ctkEventConstants::EVENT_FILTER] = "(title=samplereport)";
  pluginContext->registerService<ctkEventHandler>(&eventHandler, props);
  //! [Event Handler service registration filter]

  //! [Event Handler service registration slot]
  ReportEventHandlerUsingSlots eventHandlerUsingSlots;
  ctkDictionary propsForSlot;
  propsForSlot[ctkEventConstants::EVENT_TOPIC] = "com/acme/reportgenerator/*";
  ctkServiceReference ref = pluginContext->getServiceReference<ctkEventAdmin>();
  if (ref)
  {
    ctkEventAdmin* eventAdmin = pluginContext->getService<ctkEventAdmin>(ref);
    eventAdmin->subscribeSlot(&eventHandlerUsingSlots, SLOT(handleEvent(ctkEvent)), propsForSlot);
  }
  //! [Event Handler service registration slot]

  reportManager.reportGenerated(Report());
}
