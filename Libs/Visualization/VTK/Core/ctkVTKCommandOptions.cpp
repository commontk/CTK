/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/
/*=========================================================================
  
  Program:   ParaView
  Module:    $RCSfile: vtkCommandOptions.h,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QString>
#include <QLatin1String>
#include <QSettings>
#include <QHash>
#include <QList>
#include <QPointer>
#include <QStringList>

// CTK includes
#include <ctkUtils.h>
#include "ctkVTKCommandOptions.h"

// VTKSYS includes
#include <vtksys/CommandLineArguments.hxx>

// --------------------------------------------------------------------------
class ctkVTKCommandOptionsPrivate: public ctkPrivate<ctkVTKCommandOptions>
{
public:
  typedef ctkVTKCommandOptionsPrivate Self;
  ctkVTKCommandOptionsPrivate();
  ~ctkVTKCommandOptionsPrivate();

  void cleanArgcArgv();

  /// Callback executed when an unknown arguments is parsed
  static int unknownArgumentHandler(const char* argument, void* call_data);

  /// If case the --ignore-rest flag has been specified, this method will
  /// catch the first argument parsed and add it to the ignoredArguments list.
  /// If not, it will call the virtual method 'wrongArgument(const char* argument)'
  bool checkForIgnoreRestFlag(const char* argument);

  /// Callback executed when a deprecated arguments is parsed
  static int deprecatedArgumentHandler(const char* argument, const char* value, void* call_data);
                                                       
  /// Since vtksys::CommandLineArguments will only update char*, returns
  /// a valid char* pointer that it could use.
  char** mapQStringPtrToCharPtr(QString* qStringPtr);

  /// Since vtksys::CommandLineArguments will only update std::vector<std::string>*, returns
  /// a valid std::vector<std::string>* pointer that it could use.
  std::vector<std::string>* mapQStringListPtrToStringVectorPtr(QStringList* qStringListPtr);

  /// If required, sync the updated char* with the corresponding QString*
  void syncQStringPtrWithCharPtr();

  /// If required, sync the updated std::vector<std::string> with the corresponding QStringList*
  void syncQStringListPtrWithStringVectorPtr();

  vtksys::CommandLineArguments   CMD;
  QString                        UnknownArgument;
  QString                        ErrorMessage;
  bool                           HelpSelected;
  bool                           DisableSettings;
  bool                           IgnoreRest;
  QStringList                    IgnoredArguments;
  int                            Argc;
  char**                         Argv;
  QPointer<QSettings>            Settings;
  int                            ProcessType; // GUI, Batch, Daemon, ...
  
  QHash<QString*, char**>                         QStringPointerToCharPointerMap;
  QHash<QStringList*, std::vector<std::string>*>  QStringListPointerToStringVectorPointerMap;
};

//-----------------------------------------------------------------------------
// ctkVTKCommandOptionsPrivate methods

// --------------------------------------------------------------------------
ctkVTKCommandOptionsPrivate::ctkVTKCommandOptionsPrivate()
{
  this->CMD.SetUnknownArgumentCallback(ctkVTKCommandOptionsPrivate::unknownArgumentHandler);
  this->CMD.SetClientData(this);

  this->Argc = 0;
  this->Argv = 0; 
  this->HelpSelected = false;
  this->DisableSettings = false;
  this->IgnoreRest = false;
}

// --------------------------------------------------------------------------
ctkVTKCommandOptionsPrivate::~ctkVTKCommandOptionsPrivate()
{
  this->cleanArgcArgv();
  foreach (QString* qStringPtr, this->QStringPointerToCharPointerMap.keys())
    {
    delete this->QStringPointerToCharPointerMap[qStringPtr];
    }
  foreach (QStringList* qStringListPtr, this->QStringListPointerToStringVectorPointerMap.keys())
    {
    delete this->QStringListPointerToStringVectorPointerMap[qStringListPtr];
    }
}

// --------------------------------------------------------------------------
void ctkVTKCommandOptionsPrivate::cleanArgcArgv()
{
  int cc;
  if (this->Argv)
    {
    for (cc = 0; cc < this->Argc; cc++)
      {
      delete [] this->Argv[cc];
      }
    delete [] this->Argv;
    this->Argv = 0;
    }
}

//----------------------------------------------------------------------------
int ctkVTKCommandOptionsPrivate::unknownArgumentHandler(const char* argument,
                                                      void* call_data)
{
  ctkVTKCommandOptionsPrivate* self =
    static_cast<ctkVTKCommandOptionsPrivate*>(call_data);
  if (self)
    {
    self->UnknownArgument = QString::fromLatin1(argument);
    return self->checkForIgnoreRestFlag(argument);
    }
  return 0;
}

//----------------------------------------------------------------------------
bool ctkVTKCommandOptionsPrivate::checkForIgnoreRestFlag(const char* argument)
{
  CTK_P(ctkVTKCommandOptions);
  if (this->IgnoreRest)
    {
    this->IgnoredArguments << QLatin1String(argument);
    return true;
    }
  else
    {
    return p->wrongArgument(argument);
    }
}

//----------------------------------------------------------------------------
int ctkVTKCommandOptionsPrivate::deprecatedArgumentHandler(const char* argument,
                                                         const char* , void* call_data)
{
  //qDebug() << "UnknownArgumentHandler: " << argument;
  ctkVTKCommandOptionsPrivate* self = static_cast<ctkVTKCommandOptionsPrivate*>(call_data);
  if (self)
    {
    return self->ctk_p()->deprecatedArgument(argument);
    }
  return 0;
}

// --------------------------------------------------------------------------
char** ctkVTKCommandOptionsPrivate::mapQStringPtrToCharPtr(QString* qStringPtr)
{
  Q_ASSERT(!this->QStringPointerToCharPointerMap.contains(qStringPtr));
  char** charPtr = new char*; // Create a new pointer
  *charPtr = 0; // Initialize to 0
  this->QStringPointerToCharPointerMap[qStringPtr] = charPtr;
  return charPtr;
}

// --------------------------------------------------------------------------
std::vector<std::string>*
ctkVTKCommandOptionsPrivate::mapQStringListPtrToStringVectorPtr(QStringList* qStringListPtr)
{
  Q_ASSERT(!this->QStringListPointerToStringVectorPointerMap.contains(qStringListPtr));
  std::vector<std::string>* vectorPtr = new std::vector<std::string>(); // Create a new vector
  this->QStringListPointerToStringVectorPointerMap[qStringListPtr] = vectorPtr;
  return vectorPtr;
}

// --------------------------------------------------------------------------
void ctkVTKCommandOptionsPrivate::syncQStringPtrWithCharPtr()
{
  foreach(QString* qStringPtr, this->QStringPointerToCharPointerMap.keys())
    {
    char** charPtr = this->QStringPointerToCharPointerMap[qStringPtr];
    Q_ASSERT(charPtr);
    // Update QString only if the content pointed by charPtr is valid
    if (*charPtr)
      {
      qStringPtr->clear();
      qStringPtr->append(QLatin1String(*charPtr));
      }
    }
}

// --------------------------------------------------------------------------
void ctkVTKCommandOptionsPrivate::syncQStringListPtrWithStringVectorPtr()
{
  foreach(QStringList* qStringListPtr, this->QStringListPointerToStringVectorPointerMap.keys())
    {
    std::vector<std::string>* vectorPtr =
      this->QStringListPointerToStringVectorPointerMap[qStringListPtr];
    Q_ASSERT(vectorPtr);
    // Update QString only if vectorPtr is not empty
    if (vectorPtr->size() > 0)
      {
      qStringListPtr->clear();
      QStringList convertedVector;
      ctkUtils::stlVectorToQList(*vectorPtr, convertedVector);
      qStringListPtr->append(convertedVector);
      }
    }
}

//-----------------------------------------------------------------------------
// ctkVTKCommandOptions methods

// --------------------------------------------------------------------------
ctkVTKCommandOptions::ctkVTKCommandOptions(QSettings* _settings)
{
  CTK_INIT_PRIVATE(ctkVTKCommandOptions);
  Q_ASSERT(_settings);
  CTK_D(ctkVTKCommandOptions);
  d->Settings = _settings;
}

//----------------------------------------------------------------------------
ctkVTKCommandOptions::~ctkVTKCommandOptions()
{
}

//----------------------------------------------------------------------------
void ctkVTKCommandOptions::printAdditionalInfo()
{
  CTK_D(ctkVTKCommandOptions);
  qDebug() << "ctkVTKCommandOptions:" << this << endl
           << " HelpSelected:" << this->helpSelected() << endl
           << " DisableSettings:" << d->DisableSettings << endl
           << " IgnoreRest:" << d->IgnoreRest << endl
           << " IgnoredArguments:" << d->IgnoredArguments;
}

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKCommandOptions, QString, errorMessage, ErrorMessage);
CTK_GET_CXX(ctkVTKCommandOptions, QString, unknownArgument, UnknownArgument);
CTK_GET_CXX(ctkVTKCommandOptions, bool, helpSelected, HelpSelected);
CTK_GET_CXX(ctkVTKCommandOptions, bool, disableSettings, DisableSettings);
CTK_GET_CXX(ctkVTKCommandOptions, QSettings*, settings, Settings);
CTK_GET_CXX(ctkVTKCommandOptions, bool, ignoreRest, IgnoreRest);
CTK_GET_CXX(ctkVTKCommandOptions, QStringList, ignoredArguments, IgnoredArguments);

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKCommandOptions, int, processType, ProcessType);
CTK_SET_CXX(ctkVTKCommandOptions, int, setProcessType, ProcessType);

//----------------------------------------------------------------------------
void ctkVTKCommandOptions::initialize()
{
}

//----------------------------------------------------------------------------
QString ctkVTKCommandOptions::help()
{
  CTK_D(ctkVTKCommandOptions);
  d->CMD.SetLineLength(300);
  return QLatin1String(d->CMD.GetHelp());
}

//----------------------------------------------------------------------------
bool ctkVTKCommandOptions::postProcess(int, const char* const*)
{
  return true;
}

//----------------------------------------------------------------------------
bool ctkVTKCommandOptions::parse(int argc, const char* const argv[])
{
  CTK_D(ctkVTKCommandOptions);
  d->CMD.Initialize(argc, argv);
  this->initialize();
  this->addBooleanArgument("--help", "/?", &d->HelpSelected,
                           "Displays available command line arguments.");

  this->addBooleanArgument("--disable-settings", 0, &d->DisableSettings,
                           "Start application ignoring user settings.");


  this->addBooleanArgument("--ignore-rest", "--", &d->IgnoreRest,
                           "Ignores the rest of the labeled arguments following this flag.");

  // Get options from the command line
  bool res1 = d->CMD.Parse();
  bool res2 = this->postProcess(argc, argv);
  //qDebug() << "Res1:" << res1 << ", Res2:" << res2;
  d->cleanArgcArgv();
  d->CMD.GetRemainingArguments(&d->Argc, &d->Argv);

  if (d->DisableSettings)
    {
    this->disableCurrentSettings();
    }

  d->syncQStringPtrWithCharPtr();
  d->syncQStringListPtrWithStringVectorPtr();

  // Since CommandLineArguments include arg0 in the list
  // of remaining arguments, let's create a temporary list and remove it.
  QStringList _remaingingArguments = this->remainingArguments();
  _remaingingArguments.removeFirst();

  // Update ignored arguments list
  d->IgnoredArguments << _remaingingArguments;
  
  return res1 && res2;
}

//----------------------------------------------------------------------------
QStringList ctkVTKCommandOptions::remainingArguments()
{
  CTK_D(ctkVTKCommandOptions);
  QStringList tmp; 
  for(int i=0; i < d->Argc; ++i)
    {
    tmp << d->Argv[i]; 
    }
  return tmp;
}

//----------------------------------------------------------------------------
void ctkVTKCommandOptions::remainingArguments(int* argc, char*** argv)
{
  CTK_D(ctkVTKCommandOptions);
  *argc = d->Argc;
  *argv = d->Argv;
}

//----------------------------------------------------------------------------
void ctkVTKCommandOptions::addDeprecatedArgument(const char* longarg, const char* shortarg,
                                               const char* arghelp, int type)
{
  CTK_D(ctkVTKCommandOptions);

  // If it is for settings or not for the current process do nothing
  if((type & ctkVTKCommandOptions::QSETTINGS_ONLY) ||
     !(type & d->ProcessType || type == ctkVTKCommandOptions::ALL))
    {
    return;
    }
  
  // Add a callback for the deprecated argument handling
  d->CMD.AddCallback(longarg, vtksys::CommandLineArguments::NO_ARGUMENT,
                     ctkVTKCommandOptionsPrivate::deprecatedArgumentHandler, this, arghelp);
  if(shortarg)
    {
    d->CMD.AddCallback(shortarg, vtksys::CommandLineArguments::NO_ARGUMENT,
                       ctkVTKCommandOptionsPrivate::deprecatedArgumentHandler, this, arghelp);
    }
}

//----------------------------------------------------------------------------
bool ctkVTKCommandOptions::deprecatedArgument(const char* argument)
{
  CTK_D(ctkVTKCommandOptions);
  d->ErrorMessage = QString("  %1").arg(d->CMD.GetHelp(argument));
  return false;
}

//----------------------------------------------------------------------------
bool ctkVTKCommandOptions::wrongArgument(const char* argument)
{
  Q_UNUSED(argument);
  return false;
}

//----------------------------------------------------------------------------
void ctkVTKCommandOptions::addBooleanArgument(const char* longarg, const char* shortarg,
                                            bool* var, const char* arghelp,
                                            bool defaultValue, int type)
{
  CTK_D(ctkVTKCommandOptions);

  // Attempt to read from settings only if longarg is different from '--disable-settings'.
  if (QLatin1String(longarg) != "--disable-settings")
    {
    *var = d->Settings->value(QLatin1String(longarg+2), defaultValue).toBool();
    
    if(type & ctkVTKCommandOptions::QSETTINGS_ONLY)
      {
      return;
      }
    }

  // If the process type matches then add the argument to the command line
  if(type & d->ProcessType || type == ctkVTKCommandOptions::ALL)
    {
    d->CMD.AddBooleanArgument(longarg, var, arghelp);
    if (shortarg)
      {
      d->CMD.AddBooleanArgument(shortarg, var, longarg);
      }
    }
}

//----------------------------------------------------------------------------
void ctkVTKCommandOptions::addArgument(const char* longarg, const char* shortarg, QString* var,
                                    const char* arghelp, const QString& defaultValue, int type)
{
  CTK_D(ctkVTKCommandOptions);
  *var = d->Settings->value(QLatin1String(longarg+2), defaultValue).toString();
  
  if(type & ctkVTKCommandOptions::QSETTINGS_ONLY)
    {
    return;
    }
  
  if(type & d->ProcessType || type == ctkVTKCommandOptions::ALL)
    {
    char ** charstar = d->mapQStringPtrToCharPtr(var);
    typedef vtksys::CommandLineArguments argT;
    d->CMD.AddArgument(longarg, argT::EQUAL_ARGUMENT, charstar, arghelp);
    if ( shortarg )
      {
      d->CMD.AddArgument(shortarg, argT::EQUAL_ARGUMENT, charstar, longarg);
      }
    }
}

//----------------------------------------------------------------------------
void ctkVTKCommandOptions::addArgument(const char* longarg, const char* shortarg,
                                     QStringList* var, const char* arghelp,
                                     const QStringList& defaultValue, int type)
{
  CTK_D(ctkVTKCommandOptions);
  *var = d->Settings->value(QLatin1String(longarg+2), defaultValue).toStringList();
  
  if(type & ctkVTKCommandOptions::QSETTINGS_ONLY)
    {
    return;
    }
    
  if(type & d->ProcessType || type == ctkVTKCommandOptions::ALL)
    {
    std::vector<std::string>* vectorPtr = d->mapQStringListPtrToStringVectorPtr(var);
    typedef vtksys::CommandLineArguments argT;
    d->CMD.AddArgument(longarg, argT::MULTI_ARGUMENT, vectorPtr, arghelp);
    if (shortarg)
      {
      d->CMD.AddArgument(shortarg, argT::MULTI_ARGUMENT, vectorPtr, longarg);
      }
    }
}

//----------------------------------------------------------------------------
void ctkVTKCommandOptions::addArgument(const char* longarg, const char* shortarg, int* var,
                                     const char* arghelp, int defaultValue, int type)
{
  CTK_D(ctkVTKCommandOptions);
  *var = d->Settings->value(QLatin1String(longarg+2), defaultValue).toInt();
  
  if(type & ctkVTKCommandOptions::QSETTINGS_ONLY)
    {
    return;
    }
    
  if(type & d->ProcessType || type == ctkVTKCommandOptions::ALL)
    {
    typedef vtksys::CommandLineArguments argT;
    d->CMD.AddArgument(longarg, argT::EQUAL_ARGUMENT, var, arghelp);
    if (shortarg)
      {
      d->CMD.AddArgument(shortarg, argT::EQUAL_ARGUMENT, var, longarg);
      }
    }
}

//----------------------------------------------------------------------------
int ctkVTKCommandOptions::indexOfLastParsedArgument()
{
  CTK_D(ctkVTKCommandOptions);
  return d->CMD.GetLastArgument();
}
