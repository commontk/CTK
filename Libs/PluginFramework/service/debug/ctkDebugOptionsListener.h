/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#ifndef CTKDEBUGOPTIONSLISTENER_H
#define CTKDEBUGOPTIONSLISTENER_H

#include <QtPlugin>

struct ctkDebugOptions;

/**
 * A debug options listener is notified whenever one of its plug-in option-path entries is
 * changed.  A listener is registered as a service using the ctkDebugOptions#LISTENER_SYMBOLICNAME
 * service property to specify the symbolic name of the debug options listener.
 * <p>
 * The ctkDebugOptionsListener#optionsChanged(const ctkDebugOptions&) method will automatically
 * be called upon registration of the debug options listener service. This allows the
 * listener to obtain the initial debug options. This initial call to the listener
 * will happen even if debug is not enabled at the time of registration
 * ctkDebugOptions#isDebugEnabled() will return false in this case).
 * </p>
 * A debug options listener allows a plug-in to cache trace option values in boolean fields for performance
 * and code cleanliness. For example:
 * <pre>
 * class Activator : ctkPluginActivator, ctkDebugOptionsListener
 * {
 *   static bool DEBUG_FLAG = false;
 *
 *   void start(ctkPluginContext* context)
 *   {
 *     ctkDictionary props;
 *     props.insert(ctkDebugOptions::LISTENER_SYMBOLICNAME, "com.mycompany.mybundle");
 *     context->registerService<ctkDebugOptionsListener>(this, props);
 *   }
 *
 *   void optionsChanged(const ctkDebugOptions& options)
 *   {
 *     DEBUG_FLAG = options->getBooleanOption("com.mycompany.mybundle/debug", false);
 *   }
 *
 *   void doSomeWork()
 *   {
 *     if (DEBUG_FLAG) BERRY_INFO << "foo";
 *   }
 *   ...
 * }
 * </pre>
 */
struct ctkDebugOptionsListener
{
  virtual ~ctkDebugOptionsListener() {}

  /**
   * Notifies this listener that an option-path for its plug-in has changed.
   * This method is also called initially by the ctkDebugOptions implementation
   * when the listener is registered as a service.  This allows the listener
   * to obtain the initial set of debug options without the need to
   * acquire the debug options service.
   * @param options a reference to the ctkDebugOptions
   */
  virtual void optionsChanged(const ctkDebugOptions& options) = 0;
};

Q_DECLARE_INTERFACE(ctkDebugOptionsListener, "org.commontk.service.debug.DebugOptionsListener")

#endif // CTKDEBUGOPTIONSLISTENER_H
