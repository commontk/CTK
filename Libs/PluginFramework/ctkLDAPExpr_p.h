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

#ifndef CTKLDAPEXPR_P_H
#define CTKLDAPEXPR_P_H

#include "ctkPluginConstants.h"
#include "ctkServiceProperties_p.h"

#include <QString>
#include <QHash>
#include <QSharedDataPointer>
#include <QVector>
#include <QStringList>

class ctkLDAPExprData;

/**
\ingroup PluginFramework
\brief LDAP Expression
\date 19 May 2010
\author Xavi Planes
\ingroup ctkPluginFramework
*/
class ctkLDAPExpr {

public:

  const static int AND;     // =  0;
  const static int OR;      // =  1;
  const static int NOT;     // =  2;
  const static int EQ;      // =  4;
  const static int LE;      // =  8;
  const static int GE;      // = 16;
  const static int APPROX;  // = 32;
  const static int COMPLEX; // = AND | OR | NOT;
  const static int SIMPLE;  // = EQ | LE | GE | APPROX;

  typedef char Byte;
  typedef QVector<QStringList> LocalCache;

  /**
   * Creates an invalid ctkLDAPExpr object. Use with care.
   *
   * @see isNull()
   */
  ctkLDAPExpr();

  //!
  ctkLDAPExpr(const QString &filter);

  //!
  ctkLDAPExpr(const ctkLDAPExpr& other);

  ctkLDAPExpr& operator=(const ctkLDAPExpr& other);

  ~ctkLDAPExpr();

  /**
   * Get object class set matched by this LDAP expression. This will not work
   * with wildcards and NOT expressions. If a set can not be determined return <code>fasle</code>.
   *
   * \param objClasses The set of matched classes will be added to objClasses.
   * \return If the set cannot be determined, <code>false</code> is returned,
   *         <code>true</code> otherwise.
   */
  bool getMatchedObjectClasses(QSet<QString>& objClasses) const;

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
    const QStringList& keywords,
    LocalCache& cache,
    bool matchCase) const;

  /**
   * Returns <code>true</code> if this instance is invalid, i.e. it was
   * constructed using ctkLDAPExpr().
   *
   * @return <code>true</code> if the expression is invalid,
   *         <code>false</code> otherwise.
   */
  bool isNull() const;

  //!
  static bool query(const QString &filter, const ctkDictionary &pd);

  //! Evaluate this LDAP filter.
  bool evaluate(const ctkServiceProperties &p, bool matchCase) const;

  //!
  const QString toString() const;


private:

  class ParseState;

  //!
  ctkLDAPExpr(int op, const QList<ctkLDAPExpr> &args);

  //!
  ctkLDAPExpr(int op, const QString &attrName, const QString &attrValue);

  //!
  static ctkLDAPExpr parseExpr(ParseState &ps);

  //!
  static ctkLDAPExpr parseSimple(ParseState &ps);

  //!
  bool compare(const QVariant &obj, int op, const QString &s) const;

  //!
  static bool compareString(const QString &s1, int op, const QString &s2);

  //!
  static QString fixupString(const QString &s);

  //!
  static bool patSubstr(const QString &s, const QString &pat);

  //!
  static bool patSubstr(const QString &s, int si, const QString &pat, int pi);


  const static QChar WILDCARD; // = 65535;
  const static QString WILDCARD_QString;// = QString( WILDCARD );

  const static QString NULLQ;//      = "Null query";
  const static QString GARBAGE;//   = "Trailing garbage";
  const static QString EOS;//       = "Unexpected end of query";
  const static QString MALFORMED;// = "Malformed query";
  const static QString OPERATOR;//  = "Undefined m_operator";

  //! Shared pointer
  QSharedDataPointer<ctkLDAPExprData> d;

};


#endif // CTKLDAPEXPR_P_H
