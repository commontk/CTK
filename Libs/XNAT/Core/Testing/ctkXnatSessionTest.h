/*=========================================================================

  Library:   CTK

  Copyright (c) 2013 University College London, Centre for Medical Image Computing

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __CTKXNATSESSIONTEST_H
#define __CTKXNATSESSIONTEST_H

#include <QObject>

class ctkXnatSessionTestCasePrivate;

class ctkXnatSessionTestCase: public QObject
{
  Q_OBJECT

  void wait(int msec);

public:

  explicit ctkXnatSessionTestCase();
  virtual ~ctkXnatSessionTestCase();

private slots:

  void initTestCase();

  void init();

  void cleanupTestCase();

  void cleanup();

  void testParentChild();

  void testSession();

  void testAuthenticationError();

  void testProjectList();

  void testResourceUri();

  void testCreateProject();

  void testCreateSubject();

  void testAddResourceFolder();

  void testUploadAndDownloadFile();

private:
  QScopedPointer<ctkXnatSessionTestCasePrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatSessionTestCase)
  Q_DISABLE_COPY(ctkXnatSessionTestCase)
};

// --------------------------------------------------------------------------
int ctkXnatSessionTest(int argc, char* argv[]);

#endif
