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


#ifndef CTKPLUGINTRACKER_H
#define CTKPLUGINTRACKER_H

#include <QScopedPointer>

#include "ctkPluginFrameworkExport.h"

#include "ctkPlugin.h"
#include "ctkPluginTrackerCustomizer.h"

template<class T> class ctkTrackedPlugin;
template<class T> class ctkPluginTrackerPrivate;

/**
 * \ingroup PluginFramework
 *
 * The <code>ctkPluginTracker</code> class simplifies tracking plugins much like
 * the <code>ctkServiceTracker</code> simplifies tracking services.
 * <p>
 * A <code>ctkPluginTracker</code> is constructed with state criteria and a
 * <code>ctkPluginTrackerCustomizer</code> object. A <code>ctkPluginTracker</code> can
 * use the <code>ctkPluginTrackerCustomizer</code> to select which plugins are
 * tracked and to create a customized object to be tracked with the plugin. The
 * <code>ctkPluginTracker</code> can then be opened to begin tracking all plugins
 * whose state matches the specified state criteria.
 * <p>
 * The <code>getPlugins</code> method can be called to get the
 * <code>ctkPlugin</code> objects of the plugins being tracked. The
 * <code>getObject</code> method can be called to get the customized object for
 * a tracked plugin.
 * <p>
 * The <code>ctkPluginTracker</code> class is thread-safe. It does not call a
 * <code>ctkPluginTrackerCustomizer</code> while holding any locks.
 * <code>ctkPluginTrackerCustomizer</code> implementations must also be
 * thread-safe.
 *
 * \tparam T The type of the tracked object. The type must be an assignable
 *         datatype, provide a boolean conversion function, and provide
 *         a constructor and an assignment operator which can handle 0 as an argument.
 * \remarks This class is thread safe.
 */
template<class T = QSharedPointer<ctkPlugin> >
class ctkPluginTracker : protected ctkPluginTrackerCustomizer<T>
{
public:

  ~ctkPluginTracker();

  /**
   * Create a <code>ctkPluginTracker</code> for plugins whose state is present in
   * the specified state mask.
   *
   * <p>
   * Plugins whose state is present on the specified state mask will be
   * tracked by this <code>ctkPluginTracker</code>.
   *
   * @param context The <code>ctkPluginContext</code> against which the tracking
   *        is done.
   * @param stateMask The bit mask of the <code>OR</code>ing of the plugin
   *        states to be tracked.
   * @param customizer The customizer object to call when plugins are added,
   *        modified, or removed in this <code>ctkPluginTracker</code>. If
   *        customizer is <code>null</code>, then this
   *        <code>ctkPluginTracker</code> will be used as the
   *        <code>ctkPluginTrackerCustomizer</code> and this
   *        <code>ctkPluginTracker</code> will call the
   *        <code>ctkPluginTrackerCustomizer</code> methods on itself. If the
   *        customizer is not <code>null</code>, this <code>ctkPluginTracker</code>
   *        takes ownership of the customizer.
   * @see ctkPlugin#getState()
   */
  ctkPluginTracker(ctkPluginContext* context, ctkPlugin::States stateMask,
                   ctkPluginTrackerCustomizer<T>* customizer = 0);

  /**
   * Open this <code>ctkPluginTracker</code> and begin tracking plugins.
   *
   * <p>
   * ctkPlugin's which match the state criteria specified when this
   * <code>ctkPluginTracker</code> was created are now tracked by this
   * <code>ctkPluginTracker</code>.
   *
   * @throws ctkIllegalStateException If the <code>ctkPluginContext</code>
   *         with which this <code>ctkPluginTracker</code> was created is no
   *         longer valid.
   */
  virtual void open();

  /**
   * Close this <code>ctkPluginTracker</code>.
   *
   * <p>
   * This method should be called when this <code>ctkPluginTracker</code> should
   * end the tracking of plugins.
   *
   * <p>
   * This implementation calls getPlugins() to get the list of
   * tracked plugins to remove.
   */
  virtual void close();

  /**
   * Return a list of <code>ctkPlugin</code>s for all plugins being tracked by
   * this <code>ctkPluginTracker</code>.
   *
   * @return A list of <code>ctkPlugin</code>s.
   */
  virtual QList<QSharedPointer<ctkPlugin> > getPlugins() const;

  /**
   * Returns the customized object for the specified <code>ctkPlugin</code> if
   * the specified plugin is being tracked by this <code>ctkPluginTracker</code>.
   *
   * @param plugin The <code>ctkPlugin</code> being tracked.
   * @return The customized object for the specified <code>ctkPlugin</code> or
   *         <code>null</code> if the specified <code>ctkPlugin</code> is not
   *         being tracked.
   */
  virtual T getObject(QSharedPointer<ctkPlugin> plugin) const;

  /**
   * Remove a plugin from this <code>ctkPluginTracker</code>.
   *
   * The specified plugin will be removed from this <code>ctkPluginTracker</code>.
   * If the specified plugin was being tracked then the
   * <code>ctkPluginTrackerCustomizer::removedPlugin</code> method will be called
   * for that plugin.
   *
   * @param plugin The <code>ctkPlugin</code> to be removed.
   */
  virtual void remove(QSharedPointer<ctkPlugin> plugin);

  /**
   * Return the number of plugins being tracked by this
   * <code>ctkPluginTracker</code>.
   *
   * @return The number of plugins being tracked.
   */
  virtual int size() const;

  /**
   * Returns the tracking count for this <code>ctkPluginTracker</code>.
   *
   * The tracking count is initialized to 0 when this
   * <code>ctkPluginTracker</code> is opened. Every time a plugin is added,
   * modified or removed from this <code>ctkPluginTracker</code> the tracking
   * count is incremented.
   *
   * <p>
   * The tracking count can be used to determine if this
   * <code>ctkPluginTracker</code> has added, modified or removed a plugin by
   * comparing a tracking count value previously collected with the current
   * tracking count value. If the value has not changed, then no plugin has
   * been added, modified or removed from this <code>ctkPluginTracker</code>
   * since the previous tracking count was collected.
   *
   * @return The tracking count for this <code>ctkPluginTracker</code> or -1 if
   *         this <code>ctkPluginTracker</code> is not open.
   */
  virtual int getTrackingCount() const;

  /**
   * Return a <code>QMap</code> with the <code>ctkPlugin</code>s and customized
   * objects for all plugins being tracked by this <code>ctkPluginTracker</code>.
   *
   * @return A <code>QMap</code> with the <code>ctkPlugin</code>s and customized
   *         objects for all services being tracked by this
   *         <code>ctkPluginTracker</code>. If no plugins are being tracked, then
   *         the returned map is empty.
   */
  virtual QMap<QSharedPointer<ctkPlugin>, T> getTracked() const;

  /**
   * Return if this <code>ctkPluginTracker</code> is empty.
   *
   * @return <code>true</code> if this <code>ctkPluginTracker</code> is not tracking any
   *         plugins.
   */
  virtual bool isEmpty() const;

protected:

  /**
   * Default implementation of the
   * <code>ctkPluginTrackerCustomizer::addingPlugin</code> method.
   *
   * <p>
   * This method is only called when this <code>ctkPluginTracker</code> has been
   * constructed with a <code>null</code> ctkPluginTrackerCustomizer argument.
   *
   * <p>
   * This implementation simply returns the specified <code>ctkPlugin*</code> in
   * a QVariant.
   *
   * <p>
   * This method can be overridden in a subclass to customize the object to be
   * tracked for the plugin being added.
   *
   * @param plugin The <code>ctkPlugin</code> being added to this
   *        <code>ctkPluginTracker</code> object.
   * @param event The plugin event which caused this customizer method to be
   *        called or an invalid event if there is no plugin event associated
   *        with the call to this method.
   * @return The specified plugin.
   * @see ctkPluginTrackerCustomizer::addingPlugin(ctkPlugin*, const ctkPluginEvent&)
   */
  T addingPlugin(QSharedPointer<ctkPlugin> plugin, const ctkPluginEvent& event);

  /**
   * Default implementation of the
   * <code>ctkPluginTrackerCustomizer::modifiedPlugin</code> method.
   *
   * <p>
   * This method is only called when this <code>ctkPluginTracker</code> has been
   * constructed with a <code>null</code> ctkPluginTrackerCustomizer argument.
   *
   * <p>
   * This implementation does nothing.
   *
   * @param plugin The <code>ctkPlugin</code> whose state has been modified.
   * @param event The plugin event which caused this customizer method to be
   *        called or an invalid event if there is no plugin event associated
   *        with the call to this method.
   * @param object The customized object for the specified ctkPlugin.
   * @see ctkPluginTrackerCustomizer::modifiedPlugin(ctkPlugin*, const ctkPluginEvent&, QVariant)
   */
  void modifiedPlugin(QSharedPointer<ctkPlugin> plugin, const ctkPluginEvent& event, T object);

  /**
   * Default implementation of the
   * <code>ctkPluginTrackerCustomizer::removedPlugin</code> method.
   *
   * <p>
   * This method is only called when this <code>ctkPluginTracker</code> has been
   * constructed with a <code>null</code> ctkPluginTrackerCustomizer argument.
   *
   * <p>
   * This implementation does nothing.
   *
   * @param plugin The <code>ctkPlugin</code> being removed.
   * @param event The plugin event which caused this customizer method to be
   *        called or an invalid event if there is no plugin event associated
   *        with the call to this method.
   * @param object The customized object for the specified plugin.
   * @see ctkPluginTrackerCustomizer::removedPlugin(ctkPlugin*, const ctkPluginEvent&, QVariant)
   */
  void removedPlugin(QSharedPointer<ctkPlugin> plugin, const ctkPluginEvent& event, T object);

private:

  typedef ctkPluginTracker<T> PluginTracker;
  typedef ctkTrackedPlugin<T> TrackedPlugin;
  typedef ctkPluginTrackerPrivate<T> PluginTrackerPrivate;
  typedef ctkPluginTrackerCustomizer<T> PluginTrackerCustomizer;

  friend class ctkTrackedPlugin<T>;
  friend class ctkPluginTrackerPrivate<T>;

  inline PluginTrackerPrivate* d_func()
  {
    return reinterpret_cast<PluginTrackerPrivate*>(qGetPtrHelper(d_ptr));
  }

  inline const PluginTrackerPrivate* d_func() const
  {
    return reinterpret_cast<const PluginTrackerPrivate*>(qGetPtrHelper(d_ptr));
  }

  const QScopedPointer<PluginTrackerPrivate> d_ptr;
};


#include "ctkPluginTracker.tpp"

#endif // CTKPLUGINTRACKER_H
