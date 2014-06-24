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

#include "ctkPluginFrameworkUtil_p.h"
#include "ctkPluginFrameworkContext_p.h"

#include <QString>
#include <QCoreApplication>

#include <ctkException.h>


/**
 * Class for tokenize an attribute string.
 */
class AttributeTokenizer {

public:

  QString s;
  int length;
  int pos;

  AttributeTokenizer(const QString& input)
    : s(input), length(input.size()), pos(0)
  {

  }

  //----------------------------------------------------------------------------
  QString getWord()
  {
    skipWhite();
    bool backslash = false;
    bool quote = false;
    QString val;
    int end = 0;
    for (; pos < length; pos++)
    {
      bool breakLoop = false;
      if (backslash)
      {
        backslash = false;
        val.append(s.at(pos));
      }
      else
      {
        QChar c = s.at(pos);
        switch (c.toLatin1())
        {
        case '"':
          quote = !quote;
          end = val.length();
          break;
        case '\\':
          backslash = true;
          break;
        case ',': case ':': case ';': case '=':
          if (!quote)
          {
            breakLoop = true;
            break;
          }
          // Fall through
        default:
          val.append(c);
          if (!c.isSpace())
          {
            end = val.length();
          }
          break;
        }
        if (breakLoop) break;
      }
    }
    if (quote || backslash || end == 0)
    {
      return QString();
    }
    return val.left(end);
  }

  //----------------------------------------------------------------------------
  QString getKey()
  {
    if (pos >= length) {
      return QString();
    }
    int save = pos;
    if (s.at(pos) == ';') {
      pos++;
    }
    QString res = getWord();
    if (!res.isNull()) {
      if (pos == length) {
        return res;
      }
      QChar c = s.at(pos);
      if (c == ';' || c == ',') {
        return res;
      }
    }
    pos = save;
    return QString();
  }

  //----------------------------------------------------------------------------
  QString getParam()
  {
    if (pos == length || s.at(pos) != ';') {
      return QString();
    }
    int save = pos++;
    QString res = getWord();
    if (!res.isNull()) {
      if (pos < length && s.at(pos) == '=') {
        return res;
      } if (pos + 1 < length && s.at(pos) == ':' && s.at(pos+1) == '=') {
        return res;
      }
    }
    pos = save;
    return QString();
  }

  //----------------------------------------------------------------------------
  bool isDirective()
  {
    if (pos + 1 < length && s.at(pos) == ':')
    {
      pos++;
      return true;
    }
    else
    {
      return false;
    }
  }

  //----------------------------------------------------------------------------
  QString getValue()
  {
    if (s.at(pos) != '=')
    {
      return QString();
    }
    int save = pos++;
    skipWhite();
    QString val = getWord();
    if (val.isNull())
    {
      pos = save;
      return QString();
    }
    return val;
  }

  //----------------------------------------------------------------------------
  bool getEntryEnd()
  {
    int save = pos;
    skipWhite();
    if (pos == length) {
      return true;
    } else if (s.at(pos) == ',') {
      pos++;
      return true;
    } else {
      pos = save;
      return false;
    }
  }

  //----------------------------------------------------------------------------
  bool getEnd()
  {
    int save = pos;
    skipWhite();
    if (pos == length) {
      return true;
    } else {
      pos = save;
      return false;
    }
  }

  //----------------------------------------------------------------------------
  QString getRest()
  {
    QString res = s.mid(pos).trimmed();
    return res.length() == 0 ? "<END OF LINE>" : res;
  }

private:

  //----------------------------------------------------------------------------
  void skipWhite()
  {
    for (; pos < length; pos++) {
      if (!s.at(pos).isSpace()) {
        break;
      }
    }
  }
};

//----------------------------------------------------------------------------
QList<QMap<QString, QStringList> > ctkPluginFrameworkUtil::parseEntries(const QString& a, const QString& s,
                                           bool single, bool unique, bool single_entry)
{
  QList<QMap<QString, QStringList> > result;
  if (!s.isNull())
  {
    AttributeTokenizer at(s);
    do {
      QList<QString> keys;
      QMap<QString, QStringList > params;
      QStringList directives;

      QString key = at.getKey();
      if (key.isNull())
      {
        QString what = QString("Definition, ") + a + ", expected key at: " + at.getRest()
                       + ". Key values are terminated by a ';' or a ',' and may not "
                       + "contain ':', '='.";
        throw ctkInvalidArgumentException(what);
      }
      if (!single)
      {
        keys.push_back(key);
        while (!(key = at.getKey()).isNull())
        {
          keys.push_back(key);
        }
      }
      QString param;
      while (!(param = at.getParam()).isNull())
      {
        QStringList& old = params[param];
        bool is_directive = at.isDirective();
        if (!old.isEmpty() && unique)
        {
          QString what = QString("Definition, ") + a + ", duplicate " +
                         (is_directive ? "directive" : "attribute") +
                         ": " + param;
          throw ctkInvalidArgumentException(what);
        }
        QString value = at.getValue();
        if (value.isNull())
        {
          QString what = QString("Definition, ") + a + ", expected value at: " + at.getRest();
          throw ctkInvalidArgumentException(what);
        }
        if (is_directive)
        {
          // NYI Handle directives and check them
          directives.push_back(param);
        }
        if (unique)
        {
          params.insert(param, QStringList(value));
        } else {
          old.push_back(value);
        }
      }

      if (at.getEntryEnd())
      {
        if (single)
        {
          params.insert("$key", QStringList(key));
        }
        else
        {
          params.insert("$keys", keys);
        }
        result.push_back(params);
      }
      else
      {
        QString what = QString("Definition, ") + a + ", expected end of entry at: " + at.getRest();
        throw ctkInvalidArgumentException(what);
      }

      if (single_entry && !at.getEnd())
      {
        QString what = QString("Definition, ") + a + ", expected end of single entry at: " + at.getRest();
        throw ctkInvalidArgumentException(what);
      }

      params.insert("$directives", directives); // $ is not allowed in
                                             // param names...
    } while (!at.getEnd());
  }
  return result;
}

//----------------------------------------------------------------------------
QString ctkPluginFrameworkUtil::getFrameworkDir(ctkPluginFrameworkContext* ctx)
{
  QString s = ctx->props[ctkPluginConstants::FRAMEWORK_STORAGE].toString();
  if (s.isEmpty())
  {
    s = QCoreApplication::applicationDirPath();
    if (s.lastIndexOf("/") != s.length() -1)
    {
      s.append("/");
    }
    QString appName = QCoreApplication::applicationName();
    appName.replace(" ", "");
    if (!appName.isEmpty())
    {
      s.append(appName + "_ctkpluginfw");
    }
    else
    {
      s.append("ctkpluginfw");
      qWarning() << "Warning: Using generic plugin framework storage directory:" << s;
      qWarning() << "You should set an application name via QCoreApplication::setApplicationName()";
    }
  }
  return s;
}

//----------------------------------------------------------------------------
QDir ctkPluginFrameworkUtil::getFileStorage(ctkPluginFrameworkContext* ctx,
                                            const QString& name)
{
  // See if we have a storage directory
  QString fwdir = getFrameworkDir(ctx);
  if (fwdir.isEmpty())
  {
    throw ctkRuntimeException("The framework storge directory is empty");
  }
  QDir dir(fwdir + "/" + name);
  if (dir.exists())
  {
    if (!QFileInfo(dir.absolutePath()).isDir())
    {
      QString msg("Not a directory: ");
      msg.append(dir.absolutePath());
      throw ctkRuntimeException(msg);
    }
  }
  else
  {
    if (!dir.mkpath(dir.absolutePath()))
    {
      QString msg("Cannot create directory: ");
      msg.append(dir.absolutePath());
      throw ctkRuntimeException(msg);
    }
  }
  return dir;
}

//----------------------------------------------------------------------------
bool ctkPluginFrameworkUtil::filterMatch(const QString& filter, const QString& s)
{
  return patSubstr(s, 0, filter, 0);
}

//----------------------------------------------------------------------------
bool ctkPluginFrameworkUtil::patSubstr(const QString& s, int si, const QString& pat, int pi)
{
  if (pat.length() - pi == 0)
  {
    return s.length() - si == 0;
  }
  if (pat[pi] == '*')
  {
    pi++;
    for (;;)
    {
      if (patSubstr(s, si, pat, pi))
        return true;
      if (s.length() - si == 0)
        return false;
      si++;
    }
  }
  else
  {
    if (s.length() - si==0)
    {
      return false;
    }
    if(s[si] != pat[pi])
    {
      return false;
    }
    return patSubstr(s, ++si, pat, ++pi);
  }
}

bool pluginIdLessThan(const QSharedPointer<ctkPlugin>& p1, const QSharedPointer<ctkPlugin>& p2)
{
  return p1->getPluginId() < p2->getPluginId();
}
