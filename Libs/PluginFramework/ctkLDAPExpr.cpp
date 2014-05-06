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

#include "ctkLDAPExpr_p.h"

#include <ctkException.h>

#include <QSet>
#include <QVariant>
#include <QStringList>

#include <stdexcept>

const int ctkLDAPExpr::AND     =  0;
const int ctkLDAPExpr::OR      =  1;
const int ctkLDAPExpr::NOT     =  2;
const int ctkLDAPExpr::EQ      =  4;
const int ctkLDAPExpr::LE      =  8;
const int ctkLDAPExpr::GE      = 16;
const int ctkLDAPExpr::APPROX  = 32;
const int ctkLDAPExpr::COMPLEX = ctkLDAPExpr::AND | ctkLDAPExpr::OR | ctkLDAPExpr::NOT;
const int ctkLDAPExpr::SIMPLE  = ctkLDAPExpr::EQ | ctkLDAPExpr::LE | ctkLDAPExpr::GE | ctkLDAPExpr::APPROX;

const QChar ctkLDAPExpr::WILDCARD = 65535;
const QString ctkLDAPExpr::WILDCARD_QString = QString( WILDCARD );
const QString ctkLDAPExpr::NULLQ     = "Null query";
const QString ctkLDAPExpr::GARBAGE   = "Trailing garbage";
const QString ctkLDAPExpr::EOS       = "Unexpected end of query";
const QString ctkLDAPExpr::MALFORMED = "Malformed query";
const QString ctkLDAPExpr::OPERATOR  = "Undefined operator";

//! Contains the current parser position and parsing utility methods.
class ctkLDAPExpr::ParseState
{

private:

  int m_pos;
  QString m_str;

public:

  ParseState(const QString &str);

  //! Move m_pos to remove the prefix \a pre
  bool prefix(const QString &pre);

  /** Peek a char at m_pos
  \note If index out of bounds, throw exception
  */
  QChar peek();

  //! Increment m_pos by n
  void skip(int n);

  //! return string from m_pos until the end
  QString rest() const;

  //! Move m_pos until there's no spaces
  void skipWhite();

  //! Get string until special chars. Move m_pos
  QString getAttributeName();

  //! Get string and convert * to WILDCARD
  QString getAttributeValue();

  //! Throw InvalidSyntaxException exception
  void error(const QString &m) const;

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
    : m_operator(op), m_args(args)
  {
  }

  ctkLDAPExprData( int op, QString attrName, QString attrValue )
    : m_operator(op), m_attrName(attrName), m_attrValue(attrValue)
  {
  }

  ctkLDAPExprData( const ctkLDAPExprData& other )
    : QSharedData(other), m_operator(other.m_operator),
    m_args(other.m_args), m_attrName(other.m_attrName),
    m_attrValue(other.m_attrValue)
  {
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

//----------------------------------------------------------------------------
ctkLDAPExpr::ctkLDAPExpr()
{

}

//----------------------------------------------------------------------------
ctkLDAPExpr::ctkLDAPExpr( const QString &filter )
{
  ParseState ps(filter);

  ctkLDAPExpr expr;
  try
  {
    expr = parseExpr(ps);
  }
  catch (const std::out_of_range&)
  {
    ps.error(EOS);
  }
 
  if (!ps.rest().trimmed().isEmpty())
  {
    ps.error(GARBAGE + " '" + ps.rest() + "'");
  }

  d = expr.d;
}

//----------------------------------------------------------------------------
ctkLDAPExpr::ctkLDAPExpr( int op, const QList<ctkLDAPExpr> &args )
  : d(new ctkLDAPExprData(op, args))
{
}

//----------------------------------------------------------------------------
ctkLDAPExpr::ctkLDAPExpr( int op, const QString &attrName, const QString &attrValue )
  : d(new ctkLDAPExprData(op, attrName, attrValue))
{
}

//----------------------------------------------------------------------------
ctkLDAPExpr::ctkLDAPExpr( const ctkLDAPExpr& other )
  : d(other.d)
{
}

//----------------------------------------------------------------------------
ctkLDAPExpr& ctkLDAPExpr::operator=(const ctkLDAPExpr& other)
{
  d = other.d;
  return *this;
}

//----------------------------------------------------------------------------
ctkLDAPExpr::~ctkLDAPExpr()
{
}

//----------------------------------------------------------------------------
bool ctkLDAPExpr::getMatchedObjectClasses(QSet<QString>& objClasses) const
{
  if (d->m_operator == EQ)
  {
    if (d->m_attrName.compare(ctkPluginConstants::OBJECTCLASS, Qt::CaseInsensitive) &&
      d->m_attrValue.indexOf(WILDCARD) < 0) 
    {
      objClasses.insert( d->m_attrValue );
      return true;
    }
    return false;
  }
  else if (d->m_operator == AND) 
  {
    bool result = false;
    for (int i = 0; i < d->m_args.size( ); i++)
    {
      QSet<QString> r;
      if(d->m_args[i].getMatchedObjectClasses(r))
      {
        result = true;
        if (objClasses.empty())
        {
          objClasses = r;
        }
        else
        {
          // if AND op and classes in several operands,
          // then only the intersection is possible.
          objClasses.intersect(r);
        }
      }
    }
    return result;
  }
  else if (d->m_operator == OR)
  {
    for (int i = 0; i < d->m_args.length( ); i++)
    {
      QSet<QString> r;
      if (d->m_args[i].getMatchedObjectClasses(r))
      {
        objClasses += r;
      }
      else
      {
        objClasses.clear();
        return false;
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
bool ctkLDAPExpr::isSimple( 
  const QStringList& keywords,
  LocalCache& cache,
  bool matchCase ) const
{
  if (cache.isEmpty())
  {
    cache.resize(keywords.size());
  }

  if (d->m_operator == EQ) {
    int index;
    if ((index = keywords.indexOf(matchCase ? d->m_attrName : d->m_attrName.toLower())) >= 0 &&
      d->m_attrValue.indexOf(WILDCARD) < 0) {
        cache[index] = QStringList(d->m_attrValue);
        return true;
    }
  } else if (d->m_operator == OR) {
    for (int i = 0; i < d->m_args.size( ); i++) {
      if (!d->m_args[i].isSimple(keywords, cache, matchCase))
        return false;
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
bool ctkLDAPExpr::isNull() const
{
  return !d;
}

//----------------------------------------------------------------------------
bool ctkLDAPExpr::query( const QString &filter, const ctkDictionary &pd )
{
  return ctkLDAPExpr(filter).evaluate(pd, false);
}

//----------------------------------------------------------------------------
bool ctkLDAPExpr::evaluate( const ctkServiceProperties &p, bool matchCase ) const
{
  if ((d->m_operator & SIMPLE) != 0) {
    // try case sensitive match first
    int index = p.findCaseSensitive(d->m_attrName);
    if (index < 0 && !matchCase) index = p.find(d->m_attrName);
    return index < 0 ? false : compare(p.value(index), d->m_operator, d->m_attrValue);
  } else { // (d->m_operator & COMPLEX) != 0
    switch (d->m_operator) {
    case AND:
      for (int i = 0; i < d->m_args.length( ); i++) {
        if (!d->m_args[i].evaluate(p, matchCase))
          return false;
      }
      return true;
    case OR:
      for (int i = 0; i < d->m_args.length( ); i++) {
        if (d->m_args[i].evaluate(p, matchCase))
          return true;
      }
      return false;
    case NOT:
      return !d->m_args[0].evaluate(p, matchCase);
    default:
      return false; // Cannot happen
    }
  }
}

//----------------------------------------------------------------------------
bool ctkLDAPExpr::compare( const QVariant &obj, int op, const QString &s ) const
{
  if (obj.isNull())
    return false;
  if (op == EQ && s == WILDCARD_QString )
    return true;
  try {
    if ( obj.canConvert<QString>( ) ) {
      return compareString(obj.toString(), op, s);
    } else if (obj.canConvert<char>( ) ) {
      return compareString(obj.toString(), op, s);
    } else if (obj.canConvert<bool>( ) ) {
      if (op==LE || op==GE)
        return false;
      if ( obj.toBool() ) {
        return s.compare("true", Qt::CaseInsensitive);
      } else {
        return s.compare("false", Qt::CaseInsensitive);
      }
    } 
    else if ( obj.canConvert<Byte>( ) || obj.canConvert<int>( ) ) 
    {
      switch(op) {
      case LE:
        return obj.toInt() <= s.toInt();
      case GE:
        return obj.toInt() >= s.toInt();
      default: /*APPROX and EQ*/
        return s.toInt( ) == obj.toInt();
      }
    } else if ( obj.canConvert<float>( ) ) {
      switch(op) {
      case LE:
        return obj.toFloat() <= s.toFloat();
      case GE:
        return obj.toFloat() >= s.toFloat();
      default: /*APPROX and EQ*/
        return s.toFloat() == obj.toFloat();
      }
    } else if (obj.canConvert<double>()) {
      switch(op) {
      case LE:
        return obj.toDouble() <= s.toDouble();
      case GE:
        return obj.toDouble() >= s.toDouble();
      default: /*APPROX and EQ*/
        return s.toDouble( ) == obj.toDouble( );
      }
    } else if (obj.canConvert<qlonglong>( )) {
      switch(op) {
      case LE:
        return obj.toLongLong() <= s.toLongLong( );
      case GE:
        return obj.toLongLong() >= s.toLongLong( );
      default: /*APPROX and EQ*/
        return obj.toLongLong() == s.toLongLong( );
      }
    } 
    else if (obj.canConvert< QList<QVariant> >()) {
      QList<QVariant> list = obj.toList();
      QList<QVariant>::Iterator it;
      for (it=list.begin(); it != list.end( ); it++)
         if (compare(*it, op, s))
           return true;
    } 
  } catch (...) {
    // This might happen if a QString-to-datatype conversion fails
    // Just consider it a false match and ignore the exception
  }
  return false;
}

//----------------------------------------------------------------------------
bool ctkLDAPExpr::compareString( const QString &s1, int op, const QString &s2 )
{
  switch(op) {
  case LE:
    return s1.compare(s2) <= 0;
  case GE:
    return s1.compare(s2) >= 0;
  case EQ:
    return patSubstr(s1,s2);
  case APPROX:
    return fixupString(s2) == fixupString(s1);
  default:
    return false;
  }
}

//----------------------------------------------------------------------------
QString ctkLDAPExpr::fixupString( const QString &s )
{
  QString sb;
  int len = s.length();
  for(int i=0; i<len; i++) {
    QChar c = s.at(i);
    if (!c.isSpace()) {
      if (c.isUpper())
        c = c.toLower();
      sb.append(c);
    }
  }
  return sb;
}

//----------------------------------------------------------------------------
bool ctkLDAPExpr::patSubstr( const QString &s, int si, const QString &pat, int pi )
{
  if (pat.size( )-pi == 0)
    return s.size( )-si == 0;
  if (QChar( pat[pi] ) == WILDCARD ) {
    pi++;
    for (;;) {
      if (patSubstr( s, si, pat, pi))
        return true;
      if (s.size( )-si == 0)
        return false;
      si++;
    }
  } else {
    if (s.size( )-si==0){
      return false;
    }
    if(s[si]!=pat[pi]){
      return false;
    }
    return patSubstr( s, ++si, pat, ++pi);
  }
}

//----------------------------------------------------------------------------
bool ctkLDAPExpr::patSubstr( const QString &s, const QString &pat )
{
  return s.isNull() ? false : patSubstr(s,0,pat,0);
}

//----------------------------------------------------------------------------
ctkLDAPExpr ctkLDAPExpr::parseExpr( ParseState &ps )
{
  ps.skipWhite();
  if (!ps.prefix("("))
    ps.error(MALFORMED);

  int op;
  ps.skipWhite();
  QChar c = ps.peek();
  if ( c == '&') {
    op = AND;
  }else if ( c == '|' ){
    op = OR; 
  } else if ( c == '!' ) {
    op = NOT;
  } else {
    return parseSimple(ps);
  }
  ps.skip(1); // Ignore the d->m_operator
  QList<ctkLDAPExpr> v;
  do {
    v.append(parseExpr(ps));
    ps.skipWhite();
  } while (ps.peek() == '(');
  int n = v.size();
  if (!ps.prefix(")") || n == 0 || (op == NOT && n > 1))
    ps.error(MALFORMED);

  return ctkLDAPExpr(op, v);
}

//----------------------------------------------------------------------------
ctkLDAPExpr ctkLDAPExpr::parseSimple( ParseState &ps )
{
  QString attrName = ps.getAttributeName();
  if (attrName.isNull())
    ps.error(MALFORMED);
  int op = 0;
  if (ps.prefix("="))
    op = EQ;
  else if (ps.prefix("<="))
    op = LE;
  else if(ps.prefix(">="))
    op = GE;
  else if(ps.prefix("~="))
    op = APPROX;
  else {
    //      System.out.println("undef op='" + ps.peek() + "'");
    ps.error(OPERATOR); // Does not return
  }
  QString attrValue = ps.getAttributeValue();
  if (!ps.prefix(")"))
    ps.error(MALFORMED);
  return ctkLDAPExpr(op, attrName, attrValue);
}

//----------------------------------------------------------------------------
const QString ctkLDAPExpr::toString() const
{
  QString res;
  res.append("(");
  if ((d->m_operator & SIMPLE) != 0) {
    res.append(d->m_attrName);
    switch (d->m_operator) {
    case EQ:
      res.append("=");
      break;
    case LE:
      res.append("<=");
      break;
    case GE:
      res.append(">=");
      break;
    case APPROX:
      res.append("~=");
      break;
    }
    for (int i = 0; i < d->m_attrValue.length(); i++) {
      QChar c = d->m_attrValue.at(i);
      if (c ==  '(' || c == ')' || c == '*' || c == '\\') {
        res.append('\\');
      } else if (c == WILDCARD) {
        c = '*';
      }
      res.append(c);
    }
  } else {
    switch (d->m_operator) {
    case AND:
      res.append("&");
      break;
    case OR:
      res.append("|");
      break;
    case NOT:
      res.append("!");
      break;
    }
    for (int i = 0; i < d->m_args.length( ); i++) {
      res.append(d->m_args[i].toString());
    }
  }
  res.append(")");
  return res;
}

//----------------------------------------------------------------------------
ctkLDAPExpr::ParseState::ParseState( const QString &str )
{
  if (str.isEmpty())
  {
    error(NULLQ);
  }

  m_str = str;
  m_pos = 0;
}

//----------------------------------------------------------------------------
bool ctkLDAPExpr::ParseState::prefix( const QString &pre )
{
  if (!m_str.mid(m_pos).startsWith(pre))
    return false;
  m_pos += pre.length();
  return true;
}

//----------------------------------------------------------------------------
QChar ctkLDAPExpr::ParseState::peek()
{
  if ( m_pos >= m_str.size() )
  {
    throw std::out_of_range( "LDAPExpr" );
  }
  return m_str.at(m_pos);
}

//----------------------------------------------------------------------------
void ctkLDAPExpr::ParseState::skip( int n )
{
  m_pos += n;
}

QString ctkLDAPExpr::ParseState::rest() const
{
  return m_str.mid(m_pos);
}

//----------------------------------------------------------------------------
void ctkLDAPExpr::ParseState::skipWhite()
{
  while ( peek( ).isSpace( ) ) {
    m_pos++;
  }
}

//----------------------------------------------------------------------------
QString ctkLDAPExpr::ParseState::getAttributeName()
{
  int start = m_pos;
  int n = -1;
  for(;; m_pos++) {
    QChar c = peek( );
    if (c == '(' || c == ')' ||
      c == '<' || c == '>' ||
      c == '=' || c == '~') {
        break;
    } else if ( !c.isSpace( ) ) {
      n = m_pos - start + 1;
    }
  }
  if (n == -1) {
    return QString::Null( );
  }
  return m_str.mid(start, n);
}

//----------------------------------------------------------------------------
QString ctkLDAPExpr::ParseState::getAttributeValue()
{
  QString sb;
  bool exit = false;
  while( !exit ) {
    QChar c = peek( );
    switch(c.toLatin1()) {
    case '(':
    case ')':
    exit = true;
      break;
    case '*':
      sb.append(WILDCARD);
      break;
    case '\\':
      sb.append(m_str.at(++m_pos));
      break;
    default:
      sb.append(c);
      break;
    }
    if ( !exit )
    {
      m_pos++;
    }
  }
  return sb;
}

//----------------------------------------------------------------------------
void ctkLDAPExpr::ParseState::error( const QString &m ) const
{
  QString error = m + ": " + (m_str.isNull() ? "" : m_str.mid(m_pos) );
  throw ctkInvalidArgumentException(error);
}
