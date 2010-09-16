/*=============================================================================

Library: CTK

Copyright (c) 2010 German Cancer Research Center,
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

#include "ctkPluginConstants.h"
#include "ctkPluginFramework_global.h"

#include <exception>

#include <QString>
#include <QHash>
#include <QSharedDataPointer>

#include <stdexcept>

class ctkLDAPExprData;

/**
\brief LDAP Expression
\date 19 May 2010
\author Xavi Planes
\ingroup ctkPluginFramework
*/
class CTK_PLUGINFW_EXPORT ctkLDAPExpr {

public:
  const static int AND     =  0;
  const static int OR      =  1;
  const static int NOT     =  2;
  const static int EQ      =  4;
  const static int LE      =  8;
  const static int GE      = 16;
  const static int APPROX  = 32;
  const static int COMPLEX = AND | OR | NOT;
  const static int SIMPLE  = EQ | LE | GE | APPROX;
  typedef char Byte;

public:
  //!
  ctkLDAPExpr(const QString &filter) throw ( std::invalid_argument );

  //!
  ctkLDAPExpr(const ctkLDAPExpr& other);

  /**
   * Get object class set matched by this LDAP expression. This will not work
   * with wildcards and NOT expressions. If a set can not be determined return null.
   *
   * @return A set of classes matched, otherwise an empty set.
   */
  QSet<QString> getMatchedObjectClasses() const;


  /**
   * Checks if this LDAP expression is "simple". The definition of
   * a simple filter is:
   * <ul>
   *  <li><code>(<it>name</it>=<it>value</it>)</code> is simple if
   *      <it>name</it> is a member of the provided <code>keywords</code>,
   *      and <it>value</it> does not contain a wildcard character;</li>
   *  <li><code>(| EXPR+ )</code> is simple if all <code>EXPR</code>
   *      expressions are simple;</li>
   *  <li>No other expressions are simple.</li>
   * </ul>
   * If the filter is found to be simple, the <code>cache</code> is
   * filled with mappings from the provided keywords to lists
   * of attribute values. The keyword-value-pairs are the ones that
   * satisfy this expression, for the given keywords.
   *
   * @param keywords The keywords to look for.
   * @param cache An array (indexed by the keyword indexes) of lists to
   * fill in with values saturating this expression.
   * @return <code>true</code> if this expression is simple,
   * <code>false</code> otherwise.
   */
  bool isSimple(
    const QList<QString> &keywords, 
    QHash<int, QList<QString> > &cache, 
    bool matchCase) const;

  //! 
  static bool query(const QString &filter, const ctkDictionary &pd)
      throw (std::invalid_argument);

  //! Evaluate this LDAP filter.
  bool evaluate(const ctkDictionary &p, bool matchCase) const;

  //! 
  const QString toQString() const;


private:
  //!
  bool compare(const QVariant &obj, int op, const QString &s) const;

  //! 
  static bool compareQString(const QString &s1, int op, const QString &s2);

  //! 
  const static QString fixupQString(const QString &s);

  //! 
  static bool patSubstr(const QString &s, const QString &pat);

  //!
  static bool patSubstr(const QString &s, int si, const QString &pat, int pi);

  //! Contains the current parser position and parsing utility methods.
  class ParseState {
    int m_pos;
    QString m_str;

  public:
    ParseState(const QString &str) throw (std::invalid_argument);

    //! Move m_pos to remove the prefix \a pre
    bool prefix(const QString &pre);

    /** Peek a char at m_pos
    \note If index out of bounds, throw exception
    */
    QChar peek();

    //! Increment m_pos by n
    void skip(int n);

    //! return string from m_pos until the end
    const QString rest();

    //! Move m_pos until there's no spaces
    void skipWhite();

    //! Get string until special chars. Move m_pos
    const QString getAttributeName();

    //! Get string and convert * to WILDCARD
    const QString getAttributeValue();

    //! Throw InvalidSyntaxException exception
    void error(const QString &m) throw (std::invalid_argument);

  };

  //!
  static ctkLDAPExpr parseExpr(ParseState &ps)
    throw (std::invalid_argument);

  //!
  static ctkLDAPExpr parseSimple(ParseState &ps)
    throw (std::invalid_argument);

private:
  //!
  ctkLDAPExpr(int op, const QList<ctkLDAPExpr> &args);

  //!
  ctkLDAPExpr(int op, const QString &attrName, const QString &attrValue);


private:
  const static QChar WILDCARD; // = 65535;
  const static QString WILDCARD_QString;// = QString( WILDCARD );

  const static QString NULLQ;//      = "Null query";
  const static QString GARBAGE;//   = "Trailing garbage";
  const static QString EOS;//       = "Unexpected end of query";
  const static QString MALFORMED;// = "Malformed query";
  const static QString OPERATOR;//  = "Undefined m_operator";

private:

  //! Shared pointer
  QSharedDataPointer<ctkLDAPExprData> d;

};


/**
\brief LDAP Expression Data
\date 19 May 2010
\author Xavi Planes
\ingroup ctkPluginFramework
*/
class ctkLDAPExprData : public QSharedData
{
public:

  ctkLDAPExprData( int op, QList<ctkLDAPExpr> args )
  {
    m_operator = op;
    m_args = args;
    m_attrName = QString::Null( );
    m_attrValue = QString::Null( );
  }

  ctkLDAPExprData( int op, QString attrName, QString attrValue )
  {
    m_operator = op;
    m_args.clear();
    m_attrName = attrName;
    m_attrValue = attrValue;
  }

  ctkLDAPExprData( const ctkLDAPExprData& other ) : QSharedData(other)
  {
    m_operator = other.m_operator;
    m_args = other.m_args;
    m_attrName = other.m_attrName;
    m_attrValue = other.m_attrValue;
  }

  //!
  int m_operator;
  //!
  QList<ctkLDAPExpr> m_args;
  //!
  QString m_attrName;
  //!
  QString m_attrValue;
};


