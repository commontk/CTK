/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// CTK includes
#include "ctkLDAPExpr_p.h"

#include <iostream>
#include <cstdlib>

#include <QVariant>


int TestParsing( );
int TestEvaluate( );

//-----------------------------------------------------------------------------
int ctkLDAPExpreTest(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);  

  if ( TestParsing( ) != EXIT_SUCCESS )
  {
    return EXIT_FAILURE;
  }

  if ( TestEvaluate( ) != EXIT_SUCCESS )
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int TestParsing( ) 
{

  // WELL FORMED Expr
  try
  {
    ctkLDAPExpr ldap( "(cn=Babs Jensen)" );
    ldap = ctkLDAPExpr( "(!(cn=Tim Howes))" );
    ldap = ctkLDAPExpr( "(&(" + ctkPluginConstants::OBJECTCLASS + "=Person)(|(sn=Jensen)(cn=Babs J*)))" );
    ldap = ctkLDAPExpr( "(o=univ*of*mich*)" );
    ldap = ctkLDAPExpr( "(cn=Babs Jensen)" );
  }
  catch ( std::invalid_argument &e )
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }


  // MALFORMED Expre
  try
  {
    ctkLDAPExpr ldap( "cn=Babs Jensen)" );
    return EXIT_FAILURE;
  }
  catch ( std::invalid_argument &e )
  {
    // Nothing to do
    int i = 0;
  }

  return EXIT_SUCCESS;
}


int TestEvaluate( )
{
  // EVALUATE
  try
  {
    ctkLDAPExpr ldap( "(cn=Babs Jensen)" );
    ctkDictionary dict;
    bool eval = false;

    // Several values
    dict.insert( "cn", "Babs Jensen" );
    dict.insert( "unused", "Jansen" );
    eval = ldap.evaluate( dict, true );
    if ( !eval )
    {
      return EXIT_FAILURE;
    }

    // WILDCARD
    ldap = ctkLDAPExpr( "(cn=Babs *)" );
    dict.clear();
    dict.insert( "cn", "Babs Jensen" );
    eval = ldap.evaluate( dict, true );
    if ( !eval )
    {
      return EXIT_FAILURE;
    }

    // NOT FOUND
    ldap = ctkLDAPExpr( "(cn=Babs *)" );
    dict.clear();
    dict.insert( "unused", "New" );
    eval = ldap.evaluate( dict, true );
    if ( eval )
    {
      return EXIT_FAILURE;
    }

    // QList with integer values
    ldap = ctkLDAPExpr( "  ( |(cn=Babs *)(sn=1) )" );
    dict.clear();
    QList<QVariant> list;
    list.append( "Babs Jensen" );
    list.append( "1" );
    dict.insert( "sn", list );
    eval = ldap.evaluate( dict, true );
    if ( !eval )
    {
      return EXIT_FAILURE;
    }
  }
  catch ( std::invalid_argument &e )
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
