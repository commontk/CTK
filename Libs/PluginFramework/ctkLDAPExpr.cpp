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

#include "ctkLDAPExpr.h"
#include <QSet>
#include <QVariant>

const QChar ctkLDAPExpr::WILDCARD = 65535;
const QString ctkLDAPExpr::WILDCARD_QString = QString( WILDCARD );
const QString ctkLDAPExpr::NULLQ      = "Null query";
const QString ctkLDAPExpr::GARBAGE   = "Trailing garbage";
const QString ctkLDAPExpr::EOS       = "Unexpected end of query";
const QString ctkLDAPExpr::MALFORMED = "Malformed query";
const QString ctkLDAPExpr::OPERATOR  = "Undefined operator";

ctkLDAPExpr::ctkLDAPExpr( const QString &filter ) throw (ctkInvalidSyntaxException)
{
  ParseState ps(filter);
  try {

    ctkLDAPExpr expr = parseExpr(ps);
 
    if (ps.rest().trimmed ().length() != 0)
      ps.error(GARBAGE + " '" + ps.rest() + "'");

    d = new ctkLDAPExprData( *expr.d.data() );

  } catch ( std::out_of_range e) {
    ps.error(EOS);
  }
}

ctkLDAPExpr::ctkLDAPExpr( int op, const QList<ctkLDAPExpr> &args )
{
  d = new ctkLDAPExprData( op, args );
}

ctkLDAPExpr::ctkLDAPExpr( int op, const QString &attrName, const QString &attrValue )
{
  d = new ctkLDAPExprData( op, attrName, attrValue );
}

ctkLDAPExpr::ctkLDAPExpr( const ctkLDAPExpr& other )
{
  d = new ctkLDAPExprData( *other.d.data() );
}

QSet<QString> ctkLDAPExpr::getMatchedObjectClasses() const
{
  QSet<QString> objClasses;
  if (d->m_operator == EQ) 
  {
    if (d->m_attrName.compare(ctkPluginConstants::OBJECTCLASS, Qt::CaseInsensitive) &&
      d->m_attrValue.indexOf(WILDCARD) < 0) 
    {
      objClasses.insert( d->m_attrValue );
    }
  }
  else if (d->m_operator == AND) 
  {
    for (int i = 0; i < d->m_args.size( ); i++) {
      QSet<QString> r = d->m_args[i].getMatchedObjectClasses();
      if ( !r.empty() ) {
        if (objClasses.empty()) {
          objClasses = r;
        } else {
          // if AND op and classes in several operands,
          // then only the intersection is possible.
          objClasses = r;
        }
      }
    }
  } else if (d->m_operator == OR) {
    for (int i = 0; i < d->m_args.length( ); i++) {
      QSet<QString> r = d->m_args[i].getMatchedObjectClasses();
      if ( !r.empty() ) {
        objClasses += r;
      } else {
        objClasses.clear();
        break;
      }
    }
  }
  return objClasses;
}

bool ctkLDAPExpr::isSimple( 
  const QList<QString> &keywords, 
  QHash<int, QList<QString> > &cache, 
  bool matchCase ) const
{
  if (d->m_operator == EQ) {
    int index;
    if ((index = keywords.indexOf(matchCase ? d->m_attrName : d->m_attrName.toLower())) >= 0 &&
      d->m_attrValue.indexOf(WILDCARD) < 0) {
        cache[index] += d->m_attrValue;
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

bool ctkLDAPExpr::query( const QString &filter, const ctkDictionary &pd ) throw (ctkInvalidSyntaxException)
{
  return ctkLDAPExpr(filter).evaluate(pd, false);
}

bool ctkLDAPExpr::evaluate( const ctkDictionary &p, bool matchCase ) const
{
  if ((d->m_operator & SIMPLE) != 0) {
    return compare(p[ matchCase ? d->m_attrName : d->m_attrName.toLower() ],
      d->m_operator, d->m_attrValue);
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

bool ctkLDAPExpr::compare( const QVariant &obj, int op, const QString &s ) const
{
  if (obj.isNull())
    return false;
  if (op == EQ && s == WILDCARD_QString )
    return true;
  try {
    if ( obj.canConvert<QString>( ) ) {
      return compareQString(obj.toString(), op, s);
    } else if (obj.canConvert<char>( ) ) {
      return compareQString(obj.toString(), op, s);
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

bool ctkLDAPExpr::compareQString( const QString &s1, int op, const QString &s2 )
{
  switch(op) {
  case LE:
    return s1.compare(s2) <= 0;
  case GE:
    return s1.compare(s2) >= 0;
  case EQ:
    return patSubstr(s1,s2);
  case APPROX:
    return fixupQString(s2) == fixupQString(s1);
  default:
    return false;
  }
}

const QString ctkLDAPExpr::fixupQString( const QString &s )
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

bool ctkLDAPExpr::patSubstr( const QString &s, const QString &pat )
{
  return s.isNull() ? false : patSubstr(s,0,pat,0);
}

ctkLDAPExpr ctkLDAPExpr::parseExpr( ParseState &ps ) throw (ctkInvalidSyntaxException)
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

ctkLDAPExpr ctkLDAPExpr::parseSimple( ParseState &ps ) throw (ctkInvalidSyntaxException)
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

const QString ctkLDAPExpr::toQString() const
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
      res.append(d->m_args[i].toQString());
    }
  }
  res.append(")");
  return res;
}

ctkLDAPExpr::ParseState::ParseState( const QString &str ) throw (ctkInvalidSyntaxException)
{
  m_str = str;
  if (m_str.length() == 0)
    error(NULLQ);
  m_pos = 0;
}

bool ctkLDAPExpr::ParseState::prefix( const QString &pre )
{
  if (!m_str.startsWith(pre.mid(m_pos)))
    return false;
  m_pos += pre.length();
  return true;
}

QChar ctkLDAPExpr::ParseState::peek()
{
  if ( m_pos >= m_str.size() )
  {
    throw std::out_of_range( "LDAPExpr" );
  }
  return m_str.at(m_pos);
}

void ctkLDAPExpr::ParseState::skip( int n )
{
  m_pos += n;
}

const QString ctkLDAPExpr::ParseState::rest()
{
  return m_str.mid(m_pos);
}

void ctkLDAPExpr::ParseState::skipWhite()
{
  while ( peek( ).isSpace( ) ) {
    m_pos++;
  }
}

const QString ctkLDAPExpr::ParseState::getAttributeName()
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

const QString ctkLDAPExpr::ParseState::getAttributeValue()
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

void ctkLDAPExpr::ParseState::error( const QString &m ) throw (ctkInvalidSyntaxException)
{
  QString error = m + ": " + (m_str.isNull() ? "" : m_str.mid(m_pos) );
  throw ctkInvalidSyntaxException( error.toStdString() );
}
