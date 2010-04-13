/*=========================================================================

  Library:   qCTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// CTK includes
#include "ctkDependencyGraph.h"

// STL includes
#include <stdlib.h>
#include <iostream>


int ctkDependencyGraphTest1(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);  

  const int numberOfVertices = 3;
  const int numberOfEdges = 2;

  ctkDependencyGraph  graph(numberOfVertices,numberOfEdges);

  graph.setVerbose(true);
  graph.setVerbose(false);
  graph.setVerbose(true);

  graph.insertEdge(0,1);
  graph.insertEdge(1,2);
  graph.insertEdge(2,3);

  graph.printAdditionalInfo();
  graph.printGraph();
  
  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();

  if( noe != numberOfEdges )
    {
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();
  
  if( cfc == true )
    {
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == true )
    {
    return EXIT_FAILURE;
    }

  int corigin = graph.cycleOrigin();

  int cend = graph.cycleEnd();

  QList<int> path;

  graph.findPath( 0, 2, path );

  QList<int> list;
  graph.setEdgeListToExclude( list );

  graph.shouldExcludeEdge(2);

  QList<int> sortedlist;
  graph.topologicalSort( sortedlist );

  return EXIT_SUCCESS;
}
