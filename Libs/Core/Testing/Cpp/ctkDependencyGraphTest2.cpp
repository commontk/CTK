/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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
#include "ctkDependencyGraph.h"

// STL includes
#include <cstdlib>
#include <iostream>

// Qt includes
#include <QDebug>


int ctkDependencyGraphTest2(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);  

  // check that cycle detection works
  {
  const int numberOfVertices = 2;

  ctkDependencyGraph graph(numberOfVertices);

  //
  //  1  ->  2
  //   \    /
  //     <-
  //
  graph.insertEdge(1,2);
  graph.insertEdge(2,1);

  int expectedNumberOfEdge = 2;
  
  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    qCritical() << "Number of vertices does not match (expected" << numberOfVertices << "got" << nov << ")";
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    qCritical() << "Number of edges does not match (expected" << expectedNumberOfEdge << "got" << noe << ")";
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();
  
  if( cfc == false )
    {
    qCritical() << "Cycle detection failed";
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == false )
    {
    qCritical() << "Cycle detected flag wrong";
    return EXIT_FAILURE;
    }

  int co = graph.cycleOrigin();
  int ce = graph.cycleEnd();

  if (co != 2)
    {
    qCritical() << "Wrong cycle origin (expected" << 2 << "got" << co << ")";
    return EXIT_FAILURE;
    }

  if (ce != 1)
    {
    qCritical() << "Wrong cycle end (expected" << 1 << "got" << ce << ")";
    return EXIT_FAILURE;
    }

  }

  {
  const int numberOfVertices = 4;

  ctkDependencyGraph graph(numberOfVertices);

  //         ->  3
  //       /
  // 1 -> 2  ->  4
  //       \    /
  //         <-
  //
  graph.insertEdge(1,2);
  graph.insertEdge(2,3);
  graph.insertEdge(2,4);
  graph.insertEdge(4,2);

  int expectedNumberOfEdge = 4;

  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    qCritical() << "Number of vertices does not match (expected" << numberOfVertices << "got" << nov << ")";
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    qCritical() << "Number of edges does not match (expected" << expectedNumberOfEdge << "got" << noe << ")";
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();

  if( cfc == false )
    {
    qCritical() << "Cycle detection failed";
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == false )
    {
    qCritical() << "Cycle detected flag wrong";
    return EXIT_FAILURE;
    }

  int co = graph.cycleOrigin();
  int ce = graph.cycleEnd();

  if (co != 2)
    {
    qCritical() << "Wrong cycle origin (expected" << 2 << "got" << co << ")";
    return EXIT_FAILURE;
    }

  if (ce != 4)
    {
    qCritical() << "Wrong cycle end (expected" << 4 << "got" << ce << ")";
    return EXIT_FAILURE;
    }

  }

  // check that cycle detection works on disconnected graphs
  {
  const int numberOfVertices = 8;

  ctkDependencyGraph graph(numberOfVertices);

  /* 1 -> 2  -> 3
   *       \
   *         -> 4
   *
   *         ->  7
   *       /
   * 5 -> 6  ->  8
   *  \         /
   *    ---<---
   */
  graph.insertEdge(1,2);
  graph.insertEdge(2,3);
  graph.insertEdge(2,4);
  graph.insertEdge(5,6);
  graph.insertEdge(6,7);
  graph.insertEdge(6,8);
  graph.insertEdge(8,5);

  int expectedNumberOfEdge = 7;

  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    qCritical() << "Number of vertices does not match (expected" << numberOfVertices << "got" << nov << ")";
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    qCritical() << "Number of edges does not match (expected" << expectedNumberOfEdge << "got" << noe << ")";
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();

  if( cfc == false )
    {
    qCritical() << "Cycle detection failed";
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == false )
    {
    qCritical() << "Cycle detected flag wrong";
    return EXIT_FAILURE;
    }

  }

  // check that topological ordering and paths
  // work on disconnected graphs
  {
  const int numberOfVertices = 11;

  ctkDependencyGraph graph(numberOfVertices);

  /* 1 -> 2  -> 3
   *       \
   *         -> 4
   *
   *         ->  7 ->
   *       /          \
   * 5 -> 6  ->  8 ->  9
   *             ^
   *             |
   *            10 -> 11
   */
  graph.insertEdge(1,2);
  graph.insertEdge(2,3);
  graph.insertEdge(2,4);
  graph.insertEdge(5,6);
  graph.insertEdge(6,7);
  graph.insertEdge(6,8);
  graph.insertEdge(7,9);
  graph.insertEdge(8,9);
  graph.insertEdge(10,8);
  graph.insertEdge(10,11);

  int expectedNumberOfEdge = 10;

  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    qCritical() << "Number of vertices does not match (expected" << numberOfVertices << "got" << nov << ")";
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    qCritical() << "Number of edges does not match (expected" << expectedNumberOfEdge << "got" << noe << ")";
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();

  if( cfc == true )
    {
    qCritical() << "Cycle detection failed";
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == true )
    {
    qCritical() << "Cycle detected flag wrong";
    return EXIT_FAILURE;
    }

  QList<int> sources;
  graph.sourceVertices(sources);

  QList<int> expectedSources;
  expectedSources << 1 << 5 << 10;

  if (sources != expectedSources)
    {
    qCritical() << "Source vertices do not match (expected" << expectedSources << "got" << sources << ")";
    return EXIT_FAILURE;
    }

  QList<int> globalSort;
  graph.topologicalSort(globalSort);

  QList<int> expectedGlobalSort;
  expectedGlobalSort << 1 << 5 << 10 << 2 << 6 << 11 << 3 << 4 << 7 << 8 << 9;
  if (globalSort != expectedGlobalSort)
  {
    qCritical() << "Topological sort error (expected" << expectedGlobalSort << "got" << globalSort << ")";
    return EXIT_FAILURE;
  }

  QList<int> subSort10;
  graph.topologicalSort(subSort10, 10);

  QList<int> expectedSubSort10;
  expectedSubSort10 << 10 << 8 << 11 << 9;
  if (subSort10 != expectedSubSort10)
  {
    qCritical() << "Topological subgraph sort error (expected" << expectedSubSort10 << "got" << subSort10 << ")";
    return EXIT_FAILURE;
  }

  }

  // check that topological ordering and paths
  // work on disconnected graphs and isolated vertices
  {
  const int numberOfVertices = 13;

  ctkDependencyGraph graph(numberOfVertices);

  /* 1 -> 2  -> 3
   *       \
   *         -> 4
   *
   *         ->  7 ->
   *       /          \
   * 5 -> 6  ->  8 ->  9
   *             ^
   *             |
   *            10 -> 11
   *
   * 12   13
   */
  graph.insertEdge(1,2);
  graph.insertEdge(2,3);
  graph.insertEdge(2,4);
  graph.insertEdge(5,6);
  graph.insertEdge(6,7);
  graph.insertEdge(6,8);
  graph.insertEdge(7,9);
  graph.insertEdge(8,9);
  graph.insertEdge(10,8);
  graph.insertEdge(10,11);

  int expectedNumberOfEdge = 10;

  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    qCritical() << "Number of vertices does not match (expected" << numberOfVertices << "got" << nov << ")";
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    qCritical() << "Number of edges does not match (expected" << expectedNumberOfEdge << "got" << noe << ")";
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();

  if( cfc == true )
    {
    qCritical() << "Cycle detection failed";
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == true )
    {
    qCritical() << "Cycle detected flag wrong";
    return EXIT_FAILURE;
    }

  QList<int> sources;
  graph.sourceVertices(sources);

  QList<int> expectedSources;
  expectedSources << 1 << 5 << 10 << 12 << 13;

  if (sources != expectedSources)
    {
    qCritical() << "Source vertices do not match (expected" << expectedSources << "got" << sources << ")";
    return EXIT_FAILURE;
    }

  QList<int> globalSort;
  graph.topologicalSort(globalSort);

  QList<int> expectedGlobalSort;
  expectedGlobalSort << 1 << 5 << 10 << 12 << 13 << 2 << 6 << 11 << 3 << 4 << 7 << 8 << 9;
  if (globalSort != expectedGlobalSort)
  {
    qCritical() << "Topological sort error (expected" << expectedGlobalSort << "got" << globalSort << ")";
    return EXIT_FAILURE;
  }

  QList<int> subSort10;
  graph.topologicalSort(subSort10, 10);

  QList<int> expectedSubSort10;
  expectedSubSort10 << 10 << 8 << 11 << 9;
  if (subSort10 != expectedSubSort10)
  {
    qCritical() << "Topological subgraph sort error (expected" << expectedSubSort10 << "got" << subSort10 << ")";
    return EXIT_FAILURE;
  }

  QList<int> subSort12;
  graph.topologicalSort(subSort12, 12);

  QList<int> expectedSubSort12;
  expectedSubSort12 << 12;
  if (subSort12 != expectedSubSort12)
  {
    qCritical() << "Topological subgraph sort error (expected" << expectedSubSort12 << "got" << subSort12 << ")";
    return EXIT_FAILURE;
  }

  }

  // check that topological ordering and paths
  // work on a null graph
  {
  const int numberOfVertices = 3;

  ctkDependencyGraph graph(numberOfVertices);

  /*
   * 1  2  3
   */
  // a null graph has no edges
  int expectedNumberOfEdge = 0;

  int nov = graph.numberOfVertices();

  if( nov != numberOfVertices )
    {
    qCritical() << "Number of vertices does not match (expected" << numberOfVertices << "got" << nov << ")";
    return EXIT_FAILURE;
    }

  int noe = graph.numberOfEdges();
  if( noe != expectedNumberOfEdge )
    {
    qCritical() << "Number of edges does not match (expected" << expectedNumberOfEdge << "got" << noe << ")";
    return EXIT_FAILURE;
    }

  bool cfc = graph.checkForCycle();

  if( cfc == true )
    {
    qCritical() << "Cycle detection failed";
    return EXIT_FAILURE;
    }

  bool cdtd = graph.cycleDetected();

  if( cdtd == true )
    {
    qCritical() << "Cycle detected flag wrong";
    return EXIT_FAILURE;
    }

  QList<int> sources;
  graph.sourceVertices(sources);

  QList<int> expectedSources;
  expectedSources << 1 << 2 << 3;

  if (sources != expectedSources)
    {
    qCritical() << "Source vertices do not match (expected" << expectedSources << "got" << sources << ")";
    return EXIT_FAILURE;
    }

  QList<int> globalSort;
  graph.topologicalSort(globalSort);

  QList<int> expectedGlobalSort;
  expectedGlobalSort << 1 << 2 << 3;
  if (globalSort != expectedGlobalSort)
  {
    qCritical() << "Topological sort error (expected" << expectedGlobalSort << "got" << globalSort << ")";
    return EXIT_FAILURE;
  }

  QList<int> subSort2;
  graph.topologicalSort(subSort2, 2);

  QList<int> expectedSubSort2;
  expectedSubSort2 << 2;
  if (subSort2 != expectedSubSort2)
  {
    qCritical() << "Topological subgraph sort error (expected" << expectedSubSort2 << "got" << subSort2 << ")";
    return EXIT_FAILURE;
  }

  }

  return EXIT_SUCCESS;
}
